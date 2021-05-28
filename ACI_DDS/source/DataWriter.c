#include "../Include/GlobalDefine.h"

#define DataFragSize (63*1024)

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DataWriter_Init
-   功能描述: 初始化写入器
-   输    入: 写入器指针
-   返    回: 
-   全局变量:
-   注    释: 初始化写入器属性信息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataWriter_Init(DDS_DataWriter* pstDataWriter)
{
    if (NULL != pstDataWriter)
    {
        pstDataWriter->seqNum = SEQUENCENUMBER_ZERO;
        pstDataWriter->stHeartbeat.count = 0;
        InitHistoryCache(&pstDataWriter->stHistoryCache);
		pstDataWriter->lastWriteTime.sec = 0;
        pstDataWriter->lastWriteTime.nanosec = 0;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DataWriter_Uninit
-   功能描述: 析构写入器
-   输    入: 写入器指针
-   返    回:
-   全局变量:
-   注    释: 析构写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataWriter_Uninit(DDS_DataWriter* pstDataWriter)
{
    if (NULL != pstDataWriter)
    {
        pstDataWriter->seqNum = SEQUENCENUMBER_ZERO;
        pstDataWriter->stHeartbeat.count = 0;
		TerminateThread(pstDataWriter->hThread, 0);
        UninitHistoryCache(&pstDataWriter->stHistoryCache);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DataWriter_delete
-   功能描述: 写入器删除
-   输    入: 写入器指针
-   返    回:
-   全局变量:
-   注    释: 写入器删除
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataWriter_delete(DDS_DataWriter* pstDataWriter)
{
    DDS_DataWriter* pstPreDataWriter = NULL;
    DDS_Topic* pstTopic = pstDataWriter->pstTopic;

    DDS_DataWriter* pstCurDataWriter = pstTopic->pstLocalWriter;
    while (NULL != pstCurDataWriter)
    {
        if (pstCurDataWriter == pstDataWriter)
        {
            if (NULL == pstPreDataWriter)
            {
                pstTopic->pstLocalWriter = pstCurDataWriter->pNext;
                DDS_DataWriter_Uninit(pstCurDataWriter);
                pstCurDataWriter = pstTopic->pstLocalWriter;
            }
            else
            {
                pstPreDataWriter->pNext = pstCurDataWriter->pNext;
                DDS_DataWriter_Uninit(pstCurDataWriter);
                pstCurDataWriter = pstPreDataWriter->pNext;
            }
            break;
        }
        else
        {
            pstPreDataWriter = pstCurDataWriter;
            pstCurDataWriter = pstCurDataWriter->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointHeartbeatMsgSendUnknownReader
-   功能描述: 组装心跳报文发送未指定阅读器
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 组装心跳报文发送指定阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\15       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointHeartbeatMsgSendUnknownReader(DDS_DataWriter* pstDataWriter)
{
    DDS_DiscoveredReader*  pstDiscReader;
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* 本地无缓存，不发送心跳 */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* 网络字节序初始化，缓存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* 心跳报文头 */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;

    /* 心跳报文消息体 */
    pstDataWriter->stHeartbeat.readerId = ENTITYID_UNKNOWN;
    pstDataWriter->stHeartbeat.writerId = pstDataWriter->guid.entityId;
    pstDataWriter->stHeartbeat.firstSN = pstDataWriter->stHistoryCache.pstHead->seqNum;
    pstDataWriter->stHeartbeat.lastSN = pstDataWriter->stHistoryCache.pstTail->seqNum;
    pstDataWriter->stHeartbeat.count++;

    /* 序列化用户数据子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化用户数据子报文消息体，同时探针记录readerId位置 */
    SERIALIZE_HEARTBEAT(pstDataWriter->stHeartbeat, stMemBlock);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDataWriter->stHeartbeat.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;
    while (NULL != pstDiscReader)
    {
        /* 只给有状态发心跳 */
        if (RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
        {
            /* 序列化修改子阅读器id */
            INITIAL_MEMORYBLOCK(stMemBlockMod, pstDataWriter->stHeartbeat.pcReadId, sizeof(EntityId_t));

            SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* 发送到对应的每个远端阅读器 */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
        }

        pstDiscReader = pstDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointHeartbeatMsgSendAssignReader
-   功能描述: 组装心跳报文发送指定阅读器
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 组装心跳报文发送指定阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\15       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointHeartbeatMsgSendAssignReader(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader* pstDiscdReader)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* 本地无缓存，不发送心跳 */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* 网络字节序初始化，缓存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* 心跳报文头 */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;

    /* 心跳报文消息体 */
    pstDataWriter->stHeartbeat.readerId = pstDiscdReader->guid.entityId;
    pstDataWriter->stHeartbeat.writerId = pstDataWriter->guid.entityId;
    pstDataWriter->stHeartbeat.firstSN = pstDataWriter->stHistoryCache.pstHead->seqNum;
    pstDataWriter->stHeartbeat.lastSN = pstDataWriter->stHistoryCache.pstTail->seqNum;
    pstDataWriter->stHeartbeat.count++;

    /* 序列化用户数据子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化用户数据子报文消息体，同时探针记录readerId位置 */
    SERIALIZE_HEARTBEAT(pstDataWriter->stHeartbeat, stMemBlock);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDataWriter->stHeartbeat.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscdReader->guid.prefix))
    {
        return;
    }

    /* 发送到对应的远端阅读器 */
    RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscdReader->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointHeartbeatMsgSendAllReader
-   功能描述: 组装心跳报文发送所有阅读器
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 组装心跳报文发送所有阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\15       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointHeartbeatMsgSendAllReader(DDS_DataWriter* pstDataWriter)
{
    DDS_DiscoveredReader* pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;

    while (NULL != pstDiscReader)
    {
        JointHeartbeatMsgSendAssignReader(pstDataWriter, pstDiscReader);
        pstDiscReader = pstDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: RecoverMissSeqNum
-   功能描述: 复原丢失分片，进行网络重传
-   输    入: 本地写入器、远端阅读器，ack报文
-   返    回:
-   全局变量:
-   注    释: 复原丢失分片，进行网络重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID RecoverWriterMissSeqNum(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader* pstDiscReader, AckNack* pstAckNackMsg)
{
    SequenceNumber_t seqNumMiss = SEQUENCENUMBER_ZERO;
    SequenceNumber_t seqNumOffset = SEQUENCENUMBER_ZERO;
    DDS_ReturnCode_t returnCode = DDS_RETCODE_OK;
    const UINT32  bitStart = 0x80000000;
    UINT32 uiNumbit = 0;
	SequenceNumber_t nowNum = SEQUENCENUMBER_ZERO;
    for (uiNumbit = 0; uiNumbit < pstAckNackMsg->readerSNState.numBits; uiNumbit++)
    {
        /* 位运算计算bitMap中每一位值，大于0说明该位有效，通过偏移量计算出缺失 */
        if (0 < ((bitStart >> (uiNumbit >> 5))&(pstAckNackMsg->readerSNState.bitMap[uiNumbit >> 5])))
        {
            seqNumOffset.low = uiNumbit;
            seqNumMiss = Add_SequenceNumber(pstAckNackMsg->readerSNState.base, seqNumOffset);
            if (ISLESSTHEN_SEQUENCENUMBER(pstDataWriter->stHistoryCache.pstTail->seqNum, seqNumMiss))
            {
                break;
            }
            /* 缺失分片重发 */
            returnCode = DataWriterSendCacheMsg(pstDataWriter, pstDiscReader, seqNumMiss);
            if (returnCode == DDS_RETCODE_OK)
            {
                continue;
            }
			if (nowNum.low != 0 && seqNumMiss.low <= nowNum.low)
			{
				continue;
			}
            if (pstDataWriter->stHistoryCache.pstHead->seqNum.low < seqNumMiss.low)
            {
                printf("HeadSeq: %u, missSeq: %u.\n", pstDataWriter->stHistoryCache.pstHead->seqNum.low, seqNumMiss.low);
            }
            if (DDS_RETCODE_OK != returnCode)
            {
                //DataWriterSendGapMsg(pstDataWriter, pstDiscReader, seqNumMiss, &nowNum);
//                 printf("%d gap send 339\n",seqNumMiss.low);
                //todo发送gap报文
            }
        }
    }
    //printf("sleep1\n");
    //Sleep(1);
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_StringDataWriter_write
-   功能描述: 用户数据发送接口
-   输    入: 写入器、用户数据、数据长度
-   返    回:
-   全局变量:
-   注    释: 用户数据发送接口
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_StringDataWriter_write(DDS_DataWriter* pstDataWriter, const CHAR* buff, UINT32 length)
{
	if (length <= 0)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to send message, length error.\n");
		return DDS_RETCODE_ERROR;
	}
	if (NULL == pstDataWriter)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to send message, dataWriter is not exist.\n");
		return DDS_RETCODE_ERROR;
	}

	/* 申请静态内存创建HistoryData链表节点 */
	HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
	if (NULL == pstHistoryData)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
		return DDS_RETCODE_ERROR;
	}
	InitHistoryData(pstHistoryData);

	if (length <= DataFragSize)
	{
		/* 申请静态内存创建历史缓存数据 */
        
		pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(length);
		
		if (NULL == pstHistoryData->data)
		{
			/* 失败释放前面申请的内存 */
			DDS_STATIC_FREE(pstHistoryData);
			PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
			return DDS_RETCODE_ERROR;
		}

		/* 数据拷贝到缓存区 */
		memcpy(pstHistoryData->data, buff, length);
	}

    /* 设置缓存数据长度 */
    pstHistoryData->uiDataLen = length;

    /* 每写入一条数据写入器当前seqNum自加1 */
    pstDataWriter->seqNum = Add_SequenceNumber(pstDataWriter->seqNum, SEQUENCENUMBER_START);

    /* 缓存数据记录当前seqNum编号以便数据重传 */
    pstHistoryData->seqNum = pstDataWriter->seqNum;

    /* 数据添加时间戳 */
    if (pstDataWriter->writerCallBack)
    {
        /* 用户自定义时间回调 */
        pstDataWriter->writerCallBack(&pstHistoryData->timeInfo);
    }
    else
    {
        /* 默认时间回调 */
        pstHistoryData->timeInfo = GetNowTime();
    }
    DDS_ReturnCode_t retCode;
    if (pstHistoryData->uiDataLen > DataFragSize)
    {
        pstHistoryData->uiFragTotal = pstHistoryData->uiDataLen / DataFragSize;
        if (pstHistoryData->uiDataLen % DataFragSize != 0)
        {
            pstHistoryData->uiFragTotal += 1;
        }
        pstHistoryData->pstFragData = (FragData*)DDS_STATIC_MALLOC_T(sizeof(FragData));
        if (NULL == pstHistoryData->pstFragData)
        {
			DDS_STATIC_FREE(pstHistoryData);
            PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryDataFrag, Insufficient memory space.\n");
            return DDS_RETCODE_ERROR;
        }
        
        for (int i = 1; i <= pstHistoryData->uiFragTotal; i++)
        {
            int pstLength = 0;
            if (i == pstHistoryData->uiFragTotal)
            {
                pstLength = pstHistoryData->uiDataLen % DataFragSize;
            }
            if (pstLength == 0)
            {
                pstLength = DataFragSize;
            }
			pstHistoryData->pstFragData->data = (CHAR*)DDS_STATIC_MALLOC_T(pstLength);
            if (NULL == pstHistoryData->pstFragData->data)
            {
                /* 失败释放前面申请的内存 */
                DDS_STATIC_FREE(pstHistoryData->pstFragData->data);
				DDS_STATIC_FREE(pstHistoryData);
                PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData->pstFragData, Insufficient memory space.\n");
                return DDS_RETCODE_ERROR;
            }
            pstHistoryData->uiFragLen = pstLength;
            pstHistoryData->uiFragNum = i;
            pstHistoryData->pstFragData->usFragIndex = i;
            pstHistoryData->pstFragData->usDataLen = pstLength;
            /* 数据拷贝到缓存区 */
            memcpy(pstHistoryData->pstFragData->data, buff + (i - 1)*DataFragSize, pstLength);
            /* 组装发布报文发送给订阅的所有阅读者 */
            DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
            retCode = JointUSERDataMsgSendUnknownReader(pstDataWriter, pstHistoryData);
            DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
            DDS_STATIC_FREE(pstHistoryData->pstFragData->data);
        }
    }
    else
    {
        /* 组装发布报文发送给订阅的所有阅读者 */
        DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
        retCode = JointUSERDataMsgSendUnknownReader(pstDataWriter, pstHistoryData);
        DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
    }

    BOOL freeFlag = TRUE;

#ifdef QYDDS_DURABILITY_QOS
	if (pstDataWriter->stDataWriterQos.durability.kind == VOLATILE_DURABILITY_QOS)
	{
	}
	else if (pstDataWriter->stDataWriterQos.durability.kind == TRANSIENT_LOCAL_DURABILITY_QOS)
	{
	}
	else if (pstDataWriter->stDataWriterQos.durability.kind == TRANSIENT_DURABILITY_QOS)
	{
		/* 数缓存据插入到本地缓存链 */
		if (pstDataWriter->stHistoryCache.uiHistoryDataNum < pstDataWriter->stDataWriterQos.history.depth)
		{
			DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
			InsertHistoryCacheTail(&pstDataWriter->stHistoryCache, pstHistoryData);
			DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
		}
		else
		{
			if (pstDataWriter->stDataWriterQos.history.kind == KEEP_LAST_HISTORY_QOS)
			{

				DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
				InsertHistoryCacheTail(&pstDataWriter->stHistoryCache, pstHistoryData);
                HistoryData* tmpHead = pstDataWriter->stHistoryCache.pstHead;
				pstDataWriter->stHistoryCache.pstHead = pstDataWriter->stHistoryCache.pstHead->pNext;
				DDS_STATIC_FREE(tmpHead->data);
				DDS_STATIC_FREE(tmpHead);
				pstDataWriter->stHistoryCache.uiHistoryDataNum--;
				DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
			}
			else
			{
				//DDS_STATIC_FREE(tmpHead);
			}
		}
		freeFlag = FALSE;
	}
	else
	{
		//todo 将数据储存到文件系统中（硬盘上）
	}
#endif
    /* 有状态的需要缓存数据，以便数据重传 */
    if ((RELIABLE_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind) && (retCode != DDS_RETCODE_NO_AVAILABLE_READER) && freeFlag == TRUE)
    {
		if (pstDataWriter->stHistoryCache.uiHistoryDataNum < pstDataWriter->stDataWriterQos.history.depth)
		{
			InsertHistoryCacheTail(&pstDataWriter->stHistoryCache, pstHistoryData);
		}
		else
		{
			if (pstDataWriter->stDataWriterQos.history.kind == KEEP_LAST_HISTORY_QOS)
			{
				DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
				InsertHistoryCacheTail(&pstDataWriter->stHistoryCache, pstHistoryData);
				
                HistoryData* tmpHead = pstDataWriter->stHistoryCache.pstHead;
				pstDataWriter->stHistoryCache.pstHead = pstDataWriter->stHistoryCache.pstHead->pNext;
				DDS_STATIC_FREE(tmpHead->data);
				DDS_STATIC_FREE(tmpHead);
				pstDataWriter->stHistoryCache.uiHistoryDataNum--;
				DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
			}
			else
			{
			}
		}
		freeFlag = FALSE;
    }
    /* 无状态需释放内存 */
    else
    {
        if (freeFlag == TRUE)
        {
			if (length > DataFragSize)
			{
				DDS_STATIC_FREE(pstHistoryData->pstFragData);
			}
			else
			{
				DDS_STATIC_FREE(pstHistoryData->data);
			}
            DDS_STATIC_FREE(pstHistoryData);
        }
    }
#ifdef QYDDS_DEADLINE_QOS
    if (retCode == DDS_RETCODE_OK && pstDataWriter->stDataWriterQos.deadline.period.sec != 0 || pstDataWriter->stDataWriterQos.deadline.period.nanosec != 0)
	{
		pstDataWriter->lastWriteTime = GetNowTime();
	}
#endif
    return retCode;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DataWriterSendCacheMsg
-   功能描述: 缓存数据重传
-   输    入: 写入器、远端阅读器，缺失seqNum
-   返    回: DDS_RETCODE_OK、DDS_RETCODE_ERROR
-   全局变量:
-   注    释: 缓存数据重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_ReturnCode_t DataWriterSendCacheMsg(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader* pstDiscReader, SequenceNumber_t seqNum)
{
    SequenceNumber_t   seqNumOffset;
    UINT32             uiOffset;

    /* 本地无缓存数据或者ack报文无丢失，直接返回不处理 */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        //printf("1\n");
        return DDS_RETCODE_ERROR;
    }

    /* 缺失报文不在缓存区间，直接返回不处理 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataWriter->stHistoryCache.pstTail->seqNum, seqNum) || ISLESSTHEN_SEQUENCENUMBER(seqNum, pstDataWriter->stHistoryCache.pstHead->seqNum))
    {
        //printf("2\n");
		//printf("cache missed, seqNum: %d, headNum: %d, tailNum: %d.\n", seqNum.low, pstDataWriter->stHistoryCache.pstHead->seqNum.low, pstDataWriter->stHistoryCache.pstTail->seqNum.low);
        return DDS_RETCODE_ERROR;
    }

    /* 缺失缓存报文偏移量 */
    seqNumOffset = Sub_SequenceNumber(seqNum, pstDataWriter->stHistoryCache.pstHead->seqNum);

    HistoryData* pstHistoryData = pstDataWriter->stHistoryCache.pstHead;

    /* 通过偏移量查找缺失报文，因为缓存是连续的，不用一个个比较，直接找到对应位置 */
    for (uiOffset = 0; uiOffset < seqNumOffset.low; uiOffset++)
    {
        if (NULL != pstHistoryData)
        {
            pstHistoryData = pstHistoryData->pNext;
        }
        else
        {
            printf("DataWriterSendCacheMsg: There is a problem with the history cache linked list.\n");
            printf("3\n");
            return DDS_RETCODE_ERROR;
        }
    }

    /* 偏移量查找到的缓存报文对不上，检查程序逻辑问题 */
    if (!ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, seqNum))
    {
        printf("DataWriterSendCacheMsg: There is a problem with the history cache linked list, inconsistency of data, expect: %d, actually: %d.\n",seqNum.low, pstHistoryData->seqNum.low);
        printf("4\n");
        return DDS_RETCODE_ERROR;
    }

    /* 开始发送报文 */
    JointUSERDataMsgSendAssignReader(pstDataWriter, pstDiscReader, pstHistoryData);

    return DDS_RETCODE_OK;
}

extern DDS_ReturnCode_t DataWriterSendGapMsg(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader* pstDiscReader, SequenceNumber_t seqNum, SequenceNumber_t *nowNum)
{
    MemoryBlock           stMemBlock;
    MemoryBlock           stMemBlockMod;
    SubMessageHeader      stSubMsgHeader;
    InfoTimestamp         stInfoTS;
    ParameterHead         stParamHead;
    InfoDestination       stInfoDest;
    Gap                   stGapMsg;
    CHAR                  cUserBuffer[1024];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* 网络字节序初始化，缓存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, 1024);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

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

    stSubMsgHeader.submessageId = GAP;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;

    /* 用户数据报文体设置 */
	stGapMsg.subMsgHeader = stSubMsgHeader;
    /*memset(stGapMsg.gapList.bitMap, 0, SEQUENCE_BIT_NUMBER);*/
	for (int i = 0; i < 8; i++)
	{
		stGapMsg.gapList.bitMap[i] = 0;
	}
    stGapMsg.gapStart = seqNum;
    stGapMsg.readerId = pstDiscReader->guid.entityId;
    stGapMsg.writerId = pstDataWriter->guid.entityId;
    stGapMsg.gapList.base = seqNum;
    stGapMsg.gapList.numBits = pstDataWriter->stHistoryCache.pstHead->seqNum.low - seqNum.low;
	nowNum->low = pstDataWriter->stHistoryCache.pstHead->seqNum.low;
    /* 序列化Gap报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
    /* 序列化Gap报文体 */
    SERIALIZE_GAP(stGapMsg, stMemBlock);

    RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DelDataWriterHistoryCache
-   功能描述: 过期缓存清除
-   输    入: 主题
-   返    回: 
-   全局变量:
-   注    释: 过期缓存清除
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DelDataWriterHistoryCache(DDS_DataWriter* pstDataWriter)
{
	if (pstDataWriter == NULL)
	{
		return;
	}
    HistoryData* psHistoryData = NULL;
    HistoryData* psHistoryDataDel = NULL;
    DDS_DiscoveredReader*  pstDiscReader = NULL;;
    DDS_LocalWriter* pstLocalWriter = NULL;
    SequenceNumber_t seqNumMin;
    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    if (NULL != pstDataWriter)
    {
        if (RELIABLE_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind)
        {
            if (NULL == pstDataWriter->stHistoryCache.pstTail)
            {
                return;
            }
            /* 设置最小期望seqNum，比本地写入器最大缓存大1，若远端阅读器期望seqNum都大于等于这个，则缓存全部清除 */
            seqNumMin = Add_SequenceNumber(pstDataWriter->stHistoryCache.pstTail->seqNum, SEQUENCENUMBER_START);

            /* 获取每个主题下远端阅读器最小的期望seqNum */
            pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;
            while (NULL != pstDiscReader)
            {
                if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
                {
                    pstDiscReader = pstDiscReader->pNext;
                    continue;
                }

                if (RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
                {
                    pstLocalWriter = GetDiscoveredReaderLocalWriterByGuid(pstDiscReader, &pstDataWriter->guid);
					if (pstLocalWriter != NULL)
					{
						if (ISLESSTHEN_SEQUENCENUMBER(pstLocalWriter->stAckNack.readerSNState.base, seqNumMin))
						{
							seqNumMin = pstLocalWriter->stAckNack.readerSNState.base;
						}
					}
                }

                pstDiscReader = pstDiscReader->pNext;
            }
            /* 本地写入器缓存中删除最小期望seqNum之前所有数据 */
            psHistoryData = pstDataWriter->stHistoryCache.pstHead;
            while (NULL != psHistoryData)
            {
                if (ISLESSTHEN_SEQUENCENUMBER(psHistoryData->seqNum, seqNumMin))
                {
                    psHistoryDataDel = psHistoryData;
                }
                /* 大于等于最小seqNum值，保留 */
                else
                {
                    break;
                }
                psHistoryData = psHistoryData->pNext;

                /* 缓存释放 */
                DDS_STATIC_FREE(psHistoryDataDel->data);
                DDS_STATIC_FREE(psHistoryDataDel);

                /* 头节点偏移 */
                pstDataWriter->stHistoryCache.pstHead = psHistoryData;
                pstDataWriter->stHistoryCache.uiHistoryDataNum--;
            }
			//printf("headSeqNum: %d\n", pstDataWriter->stHistoryCache.pstHead->seqNum.low);
            /* 头节点为空，此为空链表，尾节点也需置空 */
            if (NULL == pstDataWriter->stHistoryCache.pstHead)
            {
                pstDataWriter->stHistoryCache.pstTail = NULL;
            }
        }
    }
}

DDS_DLL DDS_ReturnCode_t DDS_DeadLine_msg_write(DDS_DataWriter* pstDataWriter)
{
	char* buf = "DeadLine MSG.";
	int len = strlen(buf) + 1;
	//strcpy(buf, "DeadLine MSG.", len);
	if (NULL == pstDataWriter)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to send message, dataWriter is not exist.\n");
		return DDS_RETCODE_ERROR;
	}

	HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
	if (NULL == pstHistoryData)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
		return DDS_RETCODE_ERROR;
	}
	InitHistoryData(pstHistoryData);
	/* 申请静态内存创建历史缓存数据 */
	pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(len);

	if (NULL == pstHistoryData->data)
	{
		/* 失败释放前面申请的内存 */
		DDS_STATIC_FREE(pstHistoryData);
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
		return DDS_RETCODE_ERROR;
	}

	/* 数据拷贝到缓存区 */
	memcpy(pstHistoryData->data, buf, len);

	/* 设置缓存数据长度 */
	pstHistoryData->uiDataLen = len;

	/* 数据添加时间戳 */
	if (pstDataWriter->writerCallBack)
	{
		/* 用户自定义时间回调 */
		pstDataWriter->writerCallBack(&pstHistoryData->timeInfo);
	}
	else
	{
		/* 默认时间回调 */
		pstHistoryData->timeInfo = GetNowTime();
	}
	DDS_ReturnCode_t retCode;
	/* 组装发布报文发送给订阅的所有阅读者 */
	DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
	retCode = JointUSERDataMsgSendUnknownReader(pstDataWriter, pstHistoryData);
	DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
	DDS_STATIC_FREE(pstHistoryData->data);
	DDS_STATIC_FREE(pstHistoryData);
	pstDataWriter->lastWriteTime = GetNowTime();
	return retCode;
}

VOID DeadLineTask(DDS_DataWriter* pstDataWriter)
{
	int	sleepTime = pstDataWriter->stDataWriterQos.deadline.period.sec * 1000;
	sleepTime += pstDataWriter->stDataWriterQos.deadline.period.nanosec / 1000 / 1000;
	Duration_t nowTime;
	while (TRUE)
	{
		nowTime = GetNowTime();
		nowTime = Sub_Duration(nowTime, pstDataWriter->lastWriteTime);
        if (Compare_Duration(nowTime, pstDataWriter->stDataWriterQos.deadline.period) || Duration_IsEqual(nowTime, pstDataWriter->stDataWriterQos.deadline.period))
		{
			DDS_DeadLine_msg_write(pstDataWriter);
			pstDataWriter->lastWriteTime = GetNowTime();
            Sleep(sleepTime);
		}
        else
        {
            Duration_t restTime = Sub_Duration(pstDataWriter->stDataWriterQos.deadline.period, nowTime);
            int tmpSleepTime = restTime.sec * 1000;
            tmpSleepTime += restTime.nanosec / 1000 / 1000;
            Sleep(tmpSleepTime);
        }
	}
}

VOID DeadLineSendTask(DDS_DataWriter* pstDataWriter)
{
	pstDataWriter->hThread = CreateThread(NULL, 0, (VOID*)&DeadLineTask, (VOID*)pstDataWriter, 0, NULL);
}

