#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: JointSPDPDataMsg
-   功能描述: 组装SPDP报文
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 处理SPDP报文，发现新的远端Participant插入
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointMonitorDataMsg(DDS_DomainParticipant* pstParticipant, DDS_Monitor*  pstMonitor, MemoryBlock* pstMemBlock)
{
    MemoryBlock       stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    Data              stData;

    if (NULL == pstParticipant->pstMonitor)
    {
        return;
    }

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, (*pstMemBlock));

    /* data报文头设置 */
    stSubMsgHeader.submessageId = MONITOR;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data报文体设置 */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
    stData.writerId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
    stData.writerSN = SEQUENCENUMBER_START;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* 第三步序列化子报文消息体 */
    SERIALIZE_DATA(stData, (*pstMemBlock));

    /* 序列化的有效载荷 */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, (*pstMemBlock));

    /* 序列化监控报文 */
    MonParticipantConvertNetworkByteOrder(pstParticipant, pstMemBlock);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = (*pstMemBlock).writeIndex - stData.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* 发送到对应的远端监控 */
    RTPSSendMsg(&(*pstMemBlock), &pstParticipant->socketList.spdp_data_sender, pstMonitor->unicastLocator);
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointSPDPDataMsg
-   功能描述: 组装SPDP报文
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 处理SPDP报文，发现新的远端Participant插入
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointSPDPDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, HistoryData* pstHistoryData, BuiltinDataWriter* pstBuiltinDataWriter, MemoryBlock* pstMemBlock)
{
    MemoryBlock       stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    Data              stData;

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), (*pstMemBlock));

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, (*pstMemBlock));

    /* data报文头设置 */
    stSubMsgHeader.submessageId = DATA;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data报文体设置 */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_UNKNOWN;
    stData.writerId = pstBuiltinDataWriter->guid.entityId;
    stData.writerSN = pstHistoryData->seqNum;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* 第三步序列化子报文消息体 */
    SERIALIZE_DATA(stData, (*pstMemBlock));

    /* 序列化的有效载荷 */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, (*pstMemBlock));

    /* 序列化写入器信息 */
    PUT_BYTE_ARRAY((*pstMemBlock), pstHistoryData->data, pstHistoryData->uiDataLen);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = pstMemBlock->writeIndex - stData.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleSPDPDataMsg
-   功能描述: 处理SPDP报文
-   输    入: 子报文头指针、Data指针、内存块指针、本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 处理SPDP报文，发现新的远端Participant插入
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleSPDPDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t pstGuid;

    if (!pstSubMsgHeader->flags[1] && !(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData:  SubMsg header is abnormal !\n");
        return FALSE;
    }

#ifdef RAPID_IO_CHANNEL

    /* 校验是否是本地Participant */
    if (GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: Itself local Participant, skip.\n");
        return TRUE;
    }

#endif

    /* 校验本地是否存在该远端Participant */
    DDS_DiscoveredParticipant * pstTempDiscParticipant = pstParticipant->pstDiscParticipant;
    while (NULL != pstTempDiscParticipant)
    {
        /* 远端Participant已存在，直接返回 */
        if (GuidPrefix_Is_Equal(&pstTempDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
        {
            pstMemBlock->readIndex = pstMemBlock->totalSize;
            PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: DiscoveredParticipant already exists.\n");
            return TRUE;
        }
        pstTempDiscParticipant = pstTempDiscParticipant->pNext;
    }

    /* 申请内存用于发现远端Participant */
    DDS_DiscoveredParticipant * pstNewDiscParticipant = (DDS_DiscoveredParticipant*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredParticipant));
    if (NULL == pstNewDiscParticipant)
    {
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Failed to create DiscoveredParticipant, Insufficient memory space.\n");
        return FALSE;
    }

    /* 初始化远端Participant */
    DDS_DiscoveredParticipant_Init(pstNewDiscParticipant);

    /* 序列化内建Participant信息 */
    if (pstSubMsgHeader->flags[1])
    {
        /* 反序列化失败需释放内存，并直接返回 */
        if (!NetworkByteOrderConvertDiscParticipant(pstMemBlock, pstNewDiscParticipant))
        {
            DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
            DDS_STATIC_FREE(pstNewDiscParticipant);
            PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Deserialization failure.\n");
            return FALSE;
        }
    }

    /* 序列化Participant信息 */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));

        /* 反序列化失败需释放内存，并直接返回 */
        if (!NetworkByteOrderConvertDiscParticipant(pstMemBlock, pstNewDiscParticipant))
        {
            DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
            DDS_STATIC_FREE(pstNewDiscParticipant);
            PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Deserialization failure.\n");
            return FALSE;
        }
        else
        {
            int remoteIP[4];
            UINT32 addr = pstNewDiscParticipant->IPAddr;

            remoteIP[0] = addr & 0xFF;
            addr >>= 8;

            remoteIP[1] = addr & 0xFF;
            addr >>= 8;

            remoteIP[2] = addr & 0xFF;
            addr >>= 8;

            remoteIP[3] = addr & 0xFF;

            int localIP[4];
            addr = pstParticipant->ipAddr;

            localIP[0] = addr & 0xFF;
            addr >>= 8;

            localIP[1] = addr & 0xFF;
            addr >>= 8;

            localIP[2] = addr & 0xFF;
            addr >>= 8;

            localIP[3] = addr & 0xFF;

            int netMask[4];
            addr = pstParticipant->netMask;

            netMask[0] = addr & 0xFF;
            addr >>= 8;

            netMask[1] = addr & 0xFF;
            addr >>= 8;

            netMask[2] = addr & 0xFF;
            addr >>= 8;

            netMask[3] = addr & 0xFF;

            for (int i = 0; i < 4; i++)
            {
                remoteIP[i] &= netMask[i];
                localIP[i] &= netMask[i];
            }

            if (!((localIP[0] == remoteIP[0]) && (localIP[1] == remoteIP[1]) && (localIP[2] == remoteIP[2]) && (pstParticipant->netMask == pstNewDiscParticipant->netMask)))
            {
                DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
                DDS_STATIC_FREE(pstNewDiscParticipant);
                PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Remote participant is in a different network segment.\n");
                return FALSE;
            }
        }
    }

    /* 记录已发现得远端参与者，用于心跳校验时查找 */
    HistoryData* stHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == stHistoryData)
    {
        DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
        DDS_STATIC_FREE(pstNewDiscParticipant);
        PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create HistoryCache, Insufficient memory space.\n");
        return FALSE;
    }

	InitHistoryData(stHistoryData);
    /* 记录seqNum */
    stHistoryData->seqNum = pstDataMsg->writerSN;

    /* 远端Participant插入到本地Participant */
    LIST_INSERT_HEAD(pstParticipant->pstDiscParticipant, pstNewDiscParticipant);

    BuiltinEndpointSet_t stBuiltinEpSet = pstNewDiscParticipant->rtpsParticipantProxy.availableBuiltinEndpoints;

    pstGuid.prefix = pstRTPSMsgHeader->guidPrefix;

    /* 插入对端域内建reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[0], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 插入对端发布内建reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[1], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 插入对端订阅内建reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[2], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 插入对端域内建writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[0], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 插入对端发布内建writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[1], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 插入对端订阅内建writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[2], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* 内建订阅者reader */
    BuiltinDiscWriter*  pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(&pstParticipant->stBuiltinDataReader[0], &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
        DDS_STATIC_FREE(stHistoryData);
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Not found PAR BuiltinDiscWriter !.\n");
        return FALSE;
    }

    /* 插入内建发布者reader,失败则清理 */
    if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, stHistoryData))
    {
        DDS_STATIC_FREE(stHistoryData);
    }

    BuiltinDiscReader* pstBuiltinDiscReader = GetBuiltinDiscReaderFromBuiltinDataWriterByPrefix(&pstParticipant->stBuiltinDataWriter[0], &pstRTPSMsgHeader->guidPrefix);
    if (pstBuiltinDiscReader == NULL)
    {
        return FALSE;
    }
	pstBuiltinDiscReader->livelinessDuration = pstNewDiscParticipant->leaseDuration;
	if (pstBuiltinDiscReader->livelinessDuration.sec != 0 || pstBuiltinDiscReader->livelinessDuration.nanosec != 0)
	{
		pstParticipant->livelinessNum++;
	}
	if (((Compare_Duration(pstParticipant->discMinDuration, pstBuiltinDiscReader->livelinessDuration) == TRUE) && (pstBuiltinDiscReader->livelinessDuration.sec != 0 || pstBuiltinDiscReader->livelinessDuration.nanosec != 0)) || ((pstParticipant->discMinDuration.sec == 0 && pstParticipant->discMinDuration.nanosec == 0) && (pstBuiltinDiscReader->livelinessDuration.sec != 0 || pstBuiltinDiscReader->livelinessDuration.nanosec != 0)))
	{
		if (!GuidPrefix_Is_Equal(&pstRTPSMsgHeader->guidPrefix, &pstParticipant->stRTPSMsgHeader.guidPrefix))
		{
			DDS_MUTEX_LOCK(pstParticipant->sleepMutex);
			pstParticipant->discMinDuration = pstBuiltinDiscReader->livelinessDuration;
			pstParticipant->sleepTime = pstParticipant->discMinDuration.sec * 1000;
			pstParticipant->sleepTime += pstParticipant->discMinDuration.nanosec / 1000 / 1000;
			DDS_MUTEX_UNLOCK(pstParticipant->sleepMutex);
		}
	}

    /* 加速发现过程 */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], pstBuiltinDiscReader);
    //printf("discovered remote participant, processId: %d\n", pstNewDiscParticipant->processID);
    PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: Discovered remote participant, insert Successfully.\n");

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleMonitorDataMsg
-   功能描述: 处理监控报文
-   输    入: 内存块、pstMonitor
-   返    回:
-   全局变量:
-   注    释: 网络字节序转换为pstMonitor
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleMonitorDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    MemoryBlock stMemBlock;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* 校验本地是否存在该远端Participant */
    DDS_Monitor*  pstMonitor = pstParticipant->pstMonitor;
    while (NULL != pstMonitor)
    {
        /* 远端Participant已存在，直接返回 */
        if (GuidPrefix_Is_Equal(&pstMonitor->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
        {
            /* 更新时间 */
            pstMonitor->deadtime = time(NULL);
            pstMemBlock->readIndex = pstMemBlock->totalSize;

            INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
            JointMonitorDataMsg(pstParticipant, pstMonitor, &stMemBlock);

            PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: DiscoveredParticipant already exists.\n");
            return TRUE;
        }
        pstMonitor = pstMonitor->pNext;
    }

    /* 申请内存用于发现远端Participant */
    pstMonitor = (DDS_Monitor*)DDS_STATIC_MALLOC(sizeof(DDS_Monitor));
    if (NULL == pstMonitor)
    {
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Failed to create DiscoveredParticipant, Insufficient memory space.\n");
        return FALSE;
    }

    InitMonitor(pstMonitor);

    if (!NetworkByteOrderConvertMonitor(pstMemBlock, pstMonitor))
    {
        return FALSE;
    }

    LIST_INSERT_HEAD(pstParticipant->pstMonitor, pstMonitor);
    pstMemBlock->readIndex = pstMemBlock->totalSize;

    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
    JointMonitorDataMsg(pstParticipant, pstMonitor, &stMemBlock);

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleTopicDataMsg
-   功能描述: 主题监控状态设置
-   输    入: 内存块、pstMonitor
-   返    回:
-   全局变量:
-   注    释: 主题监控状态设置
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleTopicDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    DDS_TopicMon* pstTempTopicMon = NULL;
    DDS_TopicMon* pstPreTopicMon = NULL;

    /* 校验本地是否存在该远端Participant */
    DDS_Monitor*  pstMonitor = pstParticipant->pstMonitor;
    while (NULL != pstMonitor)
    {
        /* 远端Participant已存在，直接返回 */
        if (GuidPrefix_Is_Equal(&pstMonitor->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
        {
            DDS_TopicMon* pstTopicMon = (DDS_TopicMon*)DDS_STATIC_MALLOC(sizeof(DDS_TopicMon));
            if (NULL == pstTopicMon)
            {
                return FALSE;
            }

            if (NetworkByteOrderConvertTopic(pstMemBlock, pstTopicMon))
            {
                pstTempTopicMon = pstMonitor->pstTopicMon;
                while (NULL != pstTempTopicMon)
                {
                    if (Guid_Is_Equal(&pstTempTopicMon->tpGuid, &pstTopicMon->tpGuid))
                    {
                        DDS_STATIC_FREE(pstTopicMon);
                        return TRUE;
                    }

                    pstTempTopicMon = pstTempTopicMon->pNext;
                }

                LIST_INSERT_HEAD(pstMonitor->pstTopicMon, pstTopicMon);
            }
            else
            {
                pstTempTopicMon = pstMonitor->pstTopicMon;
                while (NULL != pstTempTopicMon)
                {
                    if (Guid_Is_Equal(&pstTempTopicMon->tpGuid, &pstTopicMon->tpGuid))
                    {
                        if (NULL == pstPreTopicMon)
                        {
                            pstMonitor->pstTopicMon = pstTempTopicMon->pNext;
                            DDS_STATIC_FREE(pstTopicMon);
                            pstTempTopicMon = pstMonitor->pstTopicMon;
                        }
                        else
                        {
                            pstPreTopicMon->pNext = pstTempTopicMon->pNext;
                            DDS_STATIC_FREE(pstTopicMon);
                            pstTempTopicMon = pstPreTopicMon->pNext;
                        }
                    }
                    else
                    {
                        pstPreTopicMon = pstTempTopicMon;
                        pstTempTopicMon = pstTempTopicMon->pNext;
                    }
                }
            }
        }
        pstMonitor = pstMonitor->pNext;
    }

    pstMemBlock->readIndex = pstMemBlock->totalSize;
	DDS_STATIC_FREE(pstTempTopicMon);
	DDS_STATIC_FREE(pstPreTopicMon);
    return TRUE;
}