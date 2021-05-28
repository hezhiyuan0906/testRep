#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: JointSPDPDataMsg
-   ��������: ��װSPDP����
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����SPDP���ģ������µ�Զ��Participant����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, (*pstMemBlock));

    /* data����ͷ���� */
    stSubMsgHeader.submessageId = MONITOR;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data���������� */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
    stData.writerId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
    stData.writerSN = SEQUENCENUMBER_START;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* �ڶ������л��ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* ���������л��ӱ�����Ϣ�� */
    SERIALIZE_DATA(stData, (*pstMemBlock));

    /* ���л�����Ч�غ� */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, (*pstMemBlock));

    /* ���л���ر��� */
    MonParticipantConvertNetworkByteOrder(pstParticipant, pstMemBlock);

    /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
    stSubMsgHeader.submessageLength = (*pstMemBlock).writeIndex - stData.uiStartIndex;

    /* ���л��޸��ӱ��ĳ��� */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* ���͵���Ӧ��Զ�˼�� */
    RTPSSendMsg(&(*pstMemBlock), &pstParticipant->socketList.spdp_data_sender, pstMonitor->unicastLocator);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: JointSPDPDataMsg
-   ��������: ��װSPDP����
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����SPDP���ģ������µ�Զ��Participant����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointSPDPDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, HistoryData* pstHistoryData, BuiltinDataWriter* pstBuiltinDataWriter, MemoryBlock* pstMemBlock)
{
    MemoryBlock       stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    Data              stData;

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), (*pstMemBlock));

    /* ʱ�䱨��ͷ���� */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* ���л�ʱ�䱨��ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* ���л�ʱ���ӱ����� */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, (*pstMemBlock));

    /* data����ͷ���� */
    stSubMsgHeader.submessageId = DATA;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data���������� */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_UNKNOWN;
    stData.writerId = pstBuiltinDataWriter->guid.entityId;
    stData.writerSN = pstHistoryData->seqNum;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* �ڶ������л��ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, (*pstMemBlock));

    /* ���������л��ӱ�����Ϣ�� */
    SERIALIZE_DATA(stData, (*pstMemBlock));

    /* ���л�����Ч�غ� */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, (*pstMemBlock));

    /* ���л�д������Ϣ */
    PUT_BYTE_ARRAY((*pstMemBlock), pstHistoryData->data, pstHistoryData->uiDataLen);

    /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
    stSubMsgHeader.submessageLength = pstMemBlock->writeIndex - stData.uiStartIndex;

    /* ���л��޸��ӱ��ĳ��� */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleSPDPDataMsg
-   ��������: ����SPDP����
-   ��    ��: �ӱ���ͷָ�롢Dataָ�롢�ڴ��ָ�롢����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����SPDP���ģ������µ�Զ��Participant����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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

    /* У���Ƿ��Ǳ���Participant */
    if (GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: Itself local Participant, skip.\n");
        return TRUE;
    }

#endif

    /* У�鱾���Ƿ���ڸ�Զ��Participant */
    DDS_DiscoveredParticipant * pstTempDiscParticipant = pstParticipant->pstDiscParticipant;
    while (NULL != pstTempDiscParticipant)
    {
        /* Զ��Participant�Ѵ��ڣ�ֱ�ӷ��� */
        if (GuidPrefix_Is_Equal(&pstTempDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
        {
            pstMemBlock->readIndex = pstMemBlock->totalSize;
            PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: DiscoveredParticipant already exists.\n");
            return TRUE;
        }
        pstTempDiscParticipant = pstTempDiscParticipant->pNext;
    }

    /* �����ڴ����ڷ���Զ��Participant */
    DDS_DiscoveredParticipant * pstNewDiscParticipant = (DDS_DiscoveredParticipant*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredParticipant));
    if (NULL == pstNewDiscParticipant)
    {
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Failed to create DiscoveredParticipant, Insufficient memory space.\n");
        return FALSE;
    }

    /* ��ʼ��Զ��Participant */
    DDS_DiscoveredParticipant_Init(pstNewDiscParticipant);

    /* ���л��ڽ�Participant��Ϣ */
    if (pstSubMsgHeader->flags[1])
    {
        /* �����л�ʧ�����ͷ��ڴ棬��ֱ�ӷ��� */
        if (!NetworkByteOrderConvertDiscParticipant(pstMemBlock, pstNewDiscParticipant))
        {
            DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
            DDS_STATIC_FREE(pstNewDiscParticipant);
            PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Deserialization failure.\n");
            return FALSE;
        }
    }

    /* ���л�Participant��Ϣ */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));

        /* �����л�ʧ�����ͷ��ڴ棬��ֱ�ӷ��� */
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

    /* ��¼�ѷ��ֵ�Զ�˲����ߣ���������У��ʱ���� */
    HistoryData* stHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == stHistoryData)
    {
        DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
        DDS_STATIC_FREE(pstNewDiscParticipant);
        PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create HistoryCache, Insufficient memory space.\n");
        return FALSE;
    }

	InitHistoryData(stHistoryData);
    /* ��¼seqNum */
    stHistoryData->seqNum = pstDataMsg->writerSN;

    /* Զ��Participant���뵽����Participant */
    LIST_INSERT_HEAD(pstParticipant->pstDiscParticipant, pstNewDiscParticipant);

    BuiltinEndpointSet_t stBuiltinEpSet = pstNewDiscParticipant->rtpsParticipantProxy.availableBuiltinEndpoints;

    pstGuid.prefix = pstRTPSMsgHeader->guidPrefix;

    /* ����Զ����ڽ�reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[0], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* ����Զ˷����ڽ�reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[1], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* ����Զ˶����ڽ�reader */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
        BuiltinDataWriterInsertDiscDataReader(&pstParticipant->stBuiltinDataWriter[2], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* ����Զ����ڽ�writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[0], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* ����Զ˷����ڽ�writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[1], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* ����Զ˶����ڽ�writer */
    if (BuiltinEndpointSetContains(stBuiltinEpSet, DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER))
    {
        pstGuid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER;
        BuiltinDataReaderInsertDiscDataWriter(&pstParticipant->stBuiltinDataReader[2], &pstGuid, pstNewDiscParticipant->rtpsParticipantProxy.metatrafficUnicastLocator);
    }

    /* �ڽ�������reader */
    BuiltinDiscWriter*  pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(&pstParticipant->stBuiltinDataReader[0], &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        DDS_DiscoveredParticipant_Uninit(pstNewDiscParticipant);
        DDS_STATIC_FREE(stHistoryData);
        PrintLog(COMMLOG_ERROR, "SPDPDiscoveredParticipantData: Not found PAR BuiltinDiscWriter !.\n");
        return FALSE;
    }

    /* �����ڽ�������reader,ʧ�������� */
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

    /* ���ٷ��ֹ��� */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], pstBuiltinDiscReader);
    //printf("discovered remote participant, processId: %d\n", pstNewDiscParticipant->processID);
    PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: Discovered remote participant, insert Successfully.\n");

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleMonitorDataMsg
-   ��������: �����ر���
-   ��    ��: �ڴ�顢pstMonitor
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �����ֽ���ת��ΪpstMonitor
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleMonitorDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    MemoryBlock stMemBlock;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* У�鱾���Ƿ���ڸ�Զ��Participant */
    DDS_Monitor*  pstMonitor = pstParticipant->pstMonitor;
    while (NULL != pstMonitor)
    {
        /* Զ��Participant�Ѵ��ڣ�ֱ�ӷ��� */
        if (GuidPrefix_Is_Equal(&pstMonitor->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
        {
            /* ����ʱ�� */
            pstMonitor->deadtime = time(NULL);
            pstMemBlock->readIndex = pstMemBlock->totalSize;

            INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
            JointMonitorDataMsg(pstParticipant, pstMonitor, &stMemBlock);

            PrintLog(COMMLOG_INFO, "SPDPDiscoveredParticipantData: DiscoveredParticipant already exists.\n");
            return TRUE;
        }
        pstMonitor = pstMonitor->pNext;
    }

    /* �����ڴ����ڷ���Զ��Participant */
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
-   �� �� ��: HandleTopicDataMsg
-   ��������: ������״̬����
-   ��    ��: �ڴ�顢pstMonitor
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������״̬����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleTopicDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    DDS_TopicMon* pstTempTopicMon = NULL;
    DDS_TopicMon* pstPreTopicMon = NULL;

    /* У�鱾���Ƿ���ڸ�Զ��Participant */
    DDS_Monitor*  pstMonitor = pstParticipant->pstMonitor;
    while (NULL != pstMonitor)
    {
        /* Զ��Participant�Ѵ��ڣ�ֱ�ӷ��� */
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