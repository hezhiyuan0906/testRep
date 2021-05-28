#include "../Include/GlobalDefine.h"

#define DATA_SIZE                   (64 * 1024)       /* 分片负载量 */
#define FRAG_SIZE                   (64 * 1024 - 1024)   /* 用户数据负载量 */

/*---------------------------------------------------------------------------------
-   函 数 名: JointUSERDataMsgSendUnknownReader
-   功能描述: 组装用户数据报文发送未指定阅读器
-   输    入: 本地写入器、缓存
-   返    回:
-   全局变量:
-   注    释: 组装用户数据发送报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_ReturnCode_t JointUSERDataMsgSendUnknownReader(DDS_DataWriter* pstDataWriter, HistoryData* pstHistoryData)
{
    DDS_DiscoveredReader* pstDiscReader;
    MemoryBlock           stMemBlock;
    MemoryBlock           stMemBlockMod;
    SubMessageHeader      stSubMsgHeader;
    ParameterHead         stParamHead;
    InfoTimestamp         stInfoTS;
    InfoDestination       stInfoDest;
    DataFrag              stDataFrag;
    Data                  stData;
    CHAR                  cUserBuffer[DATA_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

	pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;
	if (pstDiscReader == NULL)
	{
		return DDS_RETCODE_NO_AVAILABLE_READER;
	}

    /* 分片待修复 */
    /* 是否需要分片发送 */
    if (pstHistoryData->uiDataLen > FRAG_SIZE)
    {
        /* 网络字节序初始化，缓存发布报文 */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

        /* 第一步序列化RTPS报文头 */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* 时间报文头设置 */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;
        /* 序列化时间报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* 序列化时间子报文体 */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;
        /* 第二步序列化子报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* 第三步序列化子报文消息体 */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* 用户数据报文头设置 */
        stSubMsgHeader.submessageId = DATA_FRAG;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* 用户数据报文体设置 */
        stDataFrag.extraFlags[0] = 0x00;
        stDataFrag.extraFlags[1] = 0x00;
        stDataFrag.octetsToInlineQos = 0x0010;
        stDataFrag.readerId = ENTITYID_UNKNOWN;
        stDataFrag.writerId = pstDataWriter->guid.entityId;
        stDataFrag.writerSN = pstHistoryData->seqNum;
        
        stDataFrag.serializedPayload.m_cdrType = CDR_LE;  //小端用户数据设置
        stDataFrag.serializedPayload.m_option = 0x0000;
        stDataFrag.fragmentStartingNum = pstHistoryData->pstFragData->usFragIndex;
        stDataFrag.fragmentsInSubmessage = pstHistoryData->uiFragTotal;
        stDataFrag.dataSize = pstHistoryData->uiDataLen;
        stDataFrag.fragmentSize = pstHistoryData->pstFragData->usDataLen;

        /* 序列化用户数据报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* 序列化用户数据报文体 */
        SERIALIZE_DATAFRAG(stDataFrag, stMemBlock);

        /* 序列化参数列表 */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* 序列化参数列表结尾 */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* 序列化的有效载荷 */
        SERIALIZE_SERIALIZEDDATA(stDataFrag.serializedPayload, stMemBlock);

        /*for (int i = 0; i < pstHistoryData->uiFragLen; i++)
        {
            printf("%c", pstHistoryData->pstFragData[i]);
        }
        printf("\n");*/
        /* 序列化用户数据内容 */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->pstFragData->data, pstHistoryData->pstFragData->usDataLen);

        /* 计算子报文消息体长度，不带消息头 */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stDataFrag.uiStartIndex;
        
        printf("seqNum: %u, submsgLength: %u\n", stDataFrag.writerSN.low, stSubMsgHeader.submessageLength);
        /* 序列化修改子报文长度 */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

        while (NULL != pstDiscReader)
        {
            if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
            {
                PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* 修改目的地GuidPrefix */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
            SERIALIZE_GUID_PREFIX(pstDiscReader->guid.prefix, stMemBlockMod);

            ///* 修改目的地GuidEntityId */
            //INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
            //SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            /* 发送到对应的每个远端阅读器 */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
            pstDiscReader = pstDiscReader->pNext;
        }
        return DDS_RETCODE_OK;
    }
    else
    {
        /* 网络字节序初始化，缓存发布报文 */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

        /* 第一步序列化RTPS报文头 */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* 时间报文头设置 */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;
        /* 序列化时间报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* 序列化时间子报文体 */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;
        /* 第二步序列化子报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* 第三步序列化子报文消息体 */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* 用户数据报文头设置 */
        stSubMsgHeader.submessageId = DATA;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* 用户数据报文体设置 */
        stData.extraFlags[0] = 0x00;
        stData.extraFlags[1] = 0x00;
        stData.octetsToInlineQos = 0x0010;
        stData.readerId = ENTITYID_UNKNOWN;
        stData.writerId = pstDataWriter->guid.entityId;
        stData.writerSN = pstHistoryData->seqNum;
        stData.serializedPayload.m_cdrType = CDR_LE;  //小端用户数据设置
        stData.serializedPayload.m_option = 0x0000;

        /* 序列化用户数据报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* 序列化用户数据报文体 */
        SERIALIZE_DATA(stData, stMemBlock);

        /* 序列化参数列表 */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* 序列化参数列表结尾 */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* 序列化的有效载荷 */
        SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

        /* 序列化用户数据内容 */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

        /* 计算子报文消息体长度，不带消息头 */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

        /* 序列化修改子报文长度 */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);


        while (NULL != pstDiscReader)
        {
            if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
            {
                PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* 修改目的地GuidPrefix */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
            SERIALIZE_GUID_PREFIX(pstDiscReader->guid.prefix, stMemBlockMod);

            /* 修改目的地GuidEntityId */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
            SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            /* 发送到对应的每个远端阅读器 */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
            pstDiscReader = pstDiscReader->pNext;
        }
        return DDS_RETCODE_OK;
#ifdef DDS_MONITOR
        /* 发送监控消息 */
        DDS_MUTEX_LOCK(pstParticipant->monMutex);
        UserMsgSendMonitor(pstParticipant, &pstDataWriter->pstTopic->tpGuid, &stMemBlock, 0x00);
        DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointUSERDataMsgSendAssignReader
-   功能描述: 组装用户数据报文发送指定阅读器
-   输    入: 本地写入器、缓存
-   返    回:
-   全局变量:
-   注    释: 组装用户数据报文发送指定阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointUSERDataMsgSendAssignReader(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader*  pstDiscReader, HistoryData* pstHistoryData)
{
    MemoryBlock           stMemBlock;
    MemoryBlock           stMemBlockMod;
    SubMessageHeader      stSubMsgHeader;
    InfoTimestamp         stInfoTS;
    ParameterHead         stParamHead;
    DataFrag              stDataFrag;
    InfoDestination       stInfoDest;
    Data                  stData;
    USHORT                usFragSize;
    UINT32                uiFragNum;
    CHAR                  cUserBuffer[DATA_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* 是否需要分片发送 */
    if (pstHistoryData->uiDataLen > FRAG_SIZE)
    {
        /* 计算分片个数 */
        if (0 < pstHistoryData->uiDataLen % FRAG_SIZE)
        {
            stDataFrag.fragmentsInSubmessage = (pstHistoryData->uiDataLen / FRAG_SIZE) + 1;
        }
        else
        {
            stDataFrag.fragmentsInSubmessage = (pstHistoryData->uiDataLen / FRAG_SIZE);
        }

        for (uiFragNum = 0; uiFragNum < stDataFrag.fragmentsInSubmessage; uiFragNum++)
        {
            /* 计算当前分片长度 */
            if (stDataFrag.fragmentsInSubmessage - 1 == uiFragNum)
            {
                usFragSize = pstHistoryData->uiDataLen - (uiFragNum * FRAG_SIZE);
            }
            else
            {
                usFragSize = FRAG_SIZE;
            }
            /* 网络字节序初始化，缓存发布报文 */
            INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

            /* 第一步序列化RTPS报文头 */
            SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

            stSubMsgHeader.flags[0] = TRUE;    //littleEndian
            stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
            stSubMsgHeader.flags[2] = TRUE;    //dataFlag
            stSubMsgHeader.flags[3] = FALSE;   //keyFlag

            /* 序列化时间 */
            stSubMsgHeader.submessageId = INFO_TS;
            stSubMsgHeader.submessageLength = sizeof(Time_t);
            stInfoTS.value = pstHistoryData->timeInfo;

            /* 序列化时间子报文头 */
            SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

            /* 序列化时间子报文消息体 */
            SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

            /* 序列化用户数据 */
            stSubMsgHeader.submessageId = DATA_FRAG;
            stDataFrag.extraFlags[0] = 0x00;
            stDataFrag.extraFlags[1] = 0x00;
            stDataFrag.octetsToInlineQos = 0x0010;
            stDataFrag.readerId = pstDiscReader->guid.entityId;;
            stDataFrag.writerId = pstDataWriter->guid.entityId;
            stDataFrag.writerSN = pstHistoryData->seqNum;
            stDataFrag.fragmentStartingNum = uiFragNum + 1;
            stDataFrag.dataSize = pstHistoryData->uiDataLen;
            stDataFrag.fragmentSize = usFragSize;

            /* 序列化用户数据子报文头 */
            SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

            /* 序列化用户数据子报文消息体，同时探针记录readerId位置 */
            SERIALIZE_DATAFRAG(stDataFrag, stMemBlock);

            /* dataFlag或者keyFlag为TRUE，加上这个 */
            SERIALIZE_SERIALIZEDDATA(stDataFrag.serializedPayload, stMemBlock);

            /* 序列化用户数据内容 */
            PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data + (uiFragNum * FRAG_SIZE), usFragSize);

            /* 计算子报文消息体长度，不带消息头 */
            stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stDataFrag.uiStartIndex;

            /* 序列化修改子报文长度 */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

            PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

            /* 发送到对应的每个远端阅读器 */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
        }
    }
    else
    {
        /* 网络字节序初始化，缓存发布报文 */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, FRAG_SIZE);

        /* 第一步序列化RTPS报文头 */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* 时间报文头设置 */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;

        /* 序列化时间报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* 序列化时间子报文体 */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        /* InfoDT报文 */
        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = pstDiscReader->guid.prefix;

        /* 第二步序列化子报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* 第三步序列化子报文消息体 */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* 用户数据报文头设置 */
        stSubMsgHeader.submessageId = DATA;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* 用户数据报文体设置 */
        stData.extraFlags[0] = 0x00;
        stData.extraFlags[1] = 0x00;
        stData.octetsToInlineQos = 0x0010;
        stData.readerId = pstDiscReader->guid.entityId;
        stData.writerId = pstDataWriter->guid.entityId;
        stData.writerSN = pstHistoryData->seqNum;
        stData.serializedPayload.m_cdrType = CDR_LE;  //小端用户数据设置
        stData.serializedPayload.m_option = 0x0000;

        /* 序列化用户数据报文头 */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* 序列化用户数据报文体 */
        SERIALIZE_DATA(stData, stMemBlock);

        /* 序列化参数列表 */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* 序列化参数列表结尾 */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* 序列化的有效载荷 */
        SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

        /* 序列化用户数据内容 */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

        /* 计算子报文消息体长度，不带消息头 */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

        /* 序列化修改子报文长度 */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

        if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
        {
            PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
            return;
        }

        if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
        {
            return;
        }

        /* 发送到对应的每个远端阅读器 */
        RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleUSERWriterDataMsg
-   功能描述: 处理用户数据报文
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 处理用户数据报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\13       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleUSERWriterDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* 获取远端写入器guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstDataMsg->writerId;

    /* 在线qos暂时不处理，直接过滤*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* 判断是否为用户信息*/
    if (!(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: There are no UserData!\n");
        return FALSE;
    }

    /* 查找该用户数据对应的远端写入器 */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }


    
#ifdef DDS_MONITOR
    /* 发送监控消息 */
    DDS_MUTEX_LOCK(pstParticipant->monMutex);
    UserMsgSendMonitor(pstParticipant, &pstDiscWriter->pstTopic->tpGuid, pstMemBlock, 0x01);
    DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    /* 缓存数据结构内存申请 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData, Insufficient memory space.\n");
        return FALSE;
    }

    /* 反序列化装载信息 */
    DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));

    if (pstMemBlock->totalSize < pstMemBlock->readIndex)
    {
        return FALSE;
    }

    /* 用户数据时间戳 */
    pstHistoryData->timeInfo = pstTimestamp->value;
	pstHistoryData->uiFragTotal = 0;
	pstHistoryData->uiFragNum = 0;

    /* 计算用户数据长度 */
    pstHistoryData->uiDataLen = pstMemBlock->totalSize - pstMemBlock->readIndex;

    /* 用户数据内存申请 */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(pstHistoryData->uiDataLen);
    if (NULL == pstHistoryData->data)
    {
        /* 用户数据申请内存失败需释放之前申请 */
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData->data, Insufficient memory space.\n");
        return FALSE;
    }

    /* 记录当前数据seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* 反序列化用户数据 */
    GET_CHAR_ARRAY((*pstMemBlock), pstHistoryData->data, (INT)pstHistoryData->uiDataLen);

    /* 如果数据seqNum小于期望，则为过期数据，直接跳过 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataMsg->writerSN, pstDiscWriter->stAckNack.readerSNState.base))
    {
        if (strcmp(pstHistoryData->data, "DeadLine MSG.") == 0)
        {
            DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        }
        return TRUE;
    }

    /* 本地为无状态阅读器，无论远端写入器状态，直接向上提交 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        DeilverStatuslessUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        return TRUE;
    }

    /* 本地阅读器为有状态，远端写入器无状态，不匹配，直接跳过 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg:Reader and writer states do not match.\n");
        return TRUE;
    }

    /* 本地与远端都为有状态提交数据 */
    DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);

    return TRUE;
}

BOOL HandleUSERWriterDataFrag(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, DataFrag* pstDataFrag, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* 获取远端写入器guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstDataFrag->writerId;

    /* 在线qos暂时不处理，直接过滤*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN, 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* 判断是否为用户信息*/
    if (!(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: There are no UserData!\n");
        return FALSE;
    }

    /* 查找该用户数据对应的远端写入器 */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }

    /* 没有指定阅读器Id，即关联为同一主题下的阅读器 */
    //if (EntityId_Is_Equal(&ENTITYID_UNKNOWN, &pstDataMsg->readerId))
    //{
    //    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    //    if (NULL == pstDataReader)
    //    {
    //        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg: Cannot discovered local dataReader.\n");
    //        return FALSE;
    //    }
    //}
    //else
    //{
    //    /* 指定了阅读器id，需要校验是否一致 */
    //    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    //    while (NULL != pstDataReader)
    //    {
    //        if (EntityId_Is_Equal(&pstDataReader->guid.entityId, &pstDataMsg->readerId))
    //        {
    //            break;
    //        }
    //        pstDataReader = pstDataReader->pNext;
    //    }

    //    if (NULL == pstDataReader)
    //    {
    //        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg: Cannot discovered local dataReader.\n");
    //        return FALSE;
    //    }
    //}

    /* 如果数据seqNum小于期望，则为过期数据，直接跳过 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataFrag->writerSN, pstDiscWriter->stAckNack.readerSNState.base))
    {
        return TRUE;
    }

#ifdef DDS_MONITOR
    /* 发送监控消息 */
    DDS_MUTEX_LOCK(pstParticipant->monMutex);
    UserMsgSendMonitor(pstParticipant, &pstDiscWriter->pstTopic->tpGuid, pstMemBlock, 0x01);
    DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    /* 缓存数据结构内存申请 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData, Insufficient memory space.\n");
        return FALSE;
    }

    /* 反序列化装载信息 */
    DESERIALIZE_SERIALIZEDDATA(pstDataFrag->serializedPayload, (*pstMemBlock));

    if (pstMemBlock->totalSize < pstMemBlock->readIndex)
    {
        return FALSE;
    }

    /* 用户数据时间戳 */
    pstHistoryData->timeInfo = pstTimestamp->value;

    /* 计算用户数据长度 */
    pstHistoryData->uiDataLen = pstDataFrag->dataSize;//pstMemBlock->totalSize - pstMemBlock->readIndex;

    /* 用户数据内存申请 */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(pstHistoryData->uiDataLen);

    pstHistoryData->uiFragLen = pstDataFrag->fragmentSize;
    if (NULL == pstHistoryData->data)
    {
        /* 用户数据申请内存失败需释放之前申请 */
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData->data, Insufficient memory space.\n");
        return FALSE;
    }

    /* 记录当前数据seqNum */
    pstHistoryData->seqNum = pstDataFrag->writerSN;

    /* 反序列化用户数据 */
    GET_CHAR_ARRAY((*pstMemBlock), pstHistoryData->data, (INT)pstHistoryData->uiFragLen);

    pstHistoryData->uiFragTotal = pstDataFrag->fragmentsInSubmessage;
    pstHistoryData->uiFragNum = pstDataFrag->fragmentStartingNum;
    pstHistoryData->uiFragLen = pstDataFrag->fragmentSize;


    /* 本地为无状态阅读器，无论远端写入器状态，直接向上提交 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        DeilverStatuslessUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        return TRUE;
    }

    /* 本地阅读器为有状态，远端写入器无状态，不匹配，直接跳过 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg:Reader and writer states do not match.\n");
        return TRUE;
    }

    /* 本地与远端都为有状态提交数据 */
    DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);

    return FALSE;
}

BOOL HandleGapMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Gap* pstGapMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* 获取远端写入器guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstGapMsg->writerId;

    /* 在线qos暂时不处理，直接过滤*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN, 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* 查找该用户数据对应的远端写入器 */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }


    CorrectDiscoveredWriterGapMsg(pstDataReader, pstDiscWriter, pstGapMsg);

    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {

            /* 回调函数，处理用户数据 */
            if (NULL != pstDataReader->recvCallBack)
            {
#ifdef QYDDS_TIME_BASED_FILTER_QOS
                if (pstDataReader->stDataReaderQos.time_based_filter.minimum_separation.sec != 0 || pstDataReader->stDataReaderQos.time_based_filter.minimum_separation.nanosec != 0)
                {
                    Time_t nowTime = GetNowTime();
                    Duration_t timeUsed = Sub_Duration(nowTime, pstDataReader->lastRecvTime);
                    if (Compare_Duration(timeUsed, pstDataReader->stDataReaderQos.time_based_filter.minimum_separation))
                    {
                        (pstDataReader->recvCallBack)(pstDataReader);
                        pstDataReader->lastRecvTime = nowTime;
                    }
                }
                else
                {
                    (pstDataReader->recvCallBack)(pstDataReader);
                }
            
#else
                (pstDataReader->recvCallBack)(pstDataReader);
#endif
            }
                pstDataReader = pstDataReader->pNext;

        }

            UninitHistoryCache(&pstDiscWriter->pstTopic->stUserData);
    }
    return TRUE;
}
