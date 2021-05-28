#include "../Include/GlobalDefine.h"

#define DataFragSize (63*1024)

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataWriter_Init
-   ��������: ��ʼ��д����
-   ��    ��: д����ָ��
-   ��    ��: 
-   ȫ�ֱ���:
-   ע    ��: ��ʼ��д����������Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: DDS_DataWriter_Uninit
-   ��������: ����д����
-   ��    ��: д����ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: DDS_DataWriter_delete
-   ��������: д����ɾ��
-   ��    ��: д����ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: д����ɾ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: JointHeartbeatMsgSendUnknownReader
-   ��������: ��װ�������ķ���δָ���Ķ���
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��װ�������ķ���ָ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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

    /* �����޻��棬���������� */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* ʱ�䱨��ͷ���� */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* ���л�ʱ�䱨��ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���л�ʱ���ӱ����� */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* ��������ͷ */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;

    /* ����������Ϣ�� */
    pstDataWriter->stHeartbeat.readerId = ENTITYID_UNKNOWN;
    pstDataWriter->stHeartbeat.writerId = pstDataWriter->guid.entityId;
    pstDataWriter->stHeartbeat.firstSN = pstDataWriter->stHistoryCache.pstHead->seqNum;
    pstDataWriter->stHeartbeat.lastSN = pstDataWriter->stHistoryCache.pstTail->seqNum;
    pstDataWriter->stHeartbeat.count++;

    /* ���л��û������ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���л��û������ӱ�����Ϣ�壬ͬʱ̽���¼readerIdλ�� */
    SERIALIZE_HEARTBEAT(pstDataWriter->stHeartbeat, stMemBlock);

    /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDataWriter->stHeartbeat.uiStartIndex;

    /* ���л��޸��ӱ��ĳ��� */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;
    while (NULL != pstDiscReader)
    {
        /* ֻ����״̬������ */
        if (RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
        {
            /* ���л��޸����Ķ���id */
            INITIAL_MEMORYBLOCK(stMemBlockMod, pstDataWriter->stHeartbeat.pcReadId, sizeof(EntityId_t));

            SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
        }

        pstDiscReader = pstDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: JointHeartbeatMsgSendAssignReader
-   ��������: ��װ�������ķ���ָ���Ķ���
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��װ�������ķ���ָ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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

    /* �����޻��棬���������� */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* ʱ�䱨��ͷ���� */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* ���л�ʱ�䱨��ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���л�ʱ���ӱ����� */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* ��������ͷ */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;

    /* ����������Ϣ�� */
    pstDataWriter->stHeartbeat.readerId = pstDiscdReader->guid.entityId;
    pstDataWriter->stHeartbeat.writerId = pstDataWriter->guid.entityId;
    pstDataWriter->stHeartbeat.firstSN = pstDataWriter->stHistoryCache.pstHead->seqNum;
    pstDataWriter->stHeartbeat.lastSN = pstDataWriter->stHistoryCache.pstTail->seqNum;
    pstDataWriter->stHeartbeat.count++;

    /* ���л��û������ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���л��û������ӱ�����Ϣ�壬ͬʱ̽���¼readerIdλ�� */
    SERIALIZE_HEARTBEAT(pstDataWriter->stHeartbeat, stMemBlock);

    /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDataWriter->stHeartbeat.uiStartIndex;

    /* ���л��޸��ӱ��ĳ��� */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscdReader->guid.prefix))
    {
        return;
    }

    /* ���͵���Ӧ��Զ���Ķ��� */
    RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscdReader->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: JointHeartbeatMsgSendAllReader
-   ��������: ��װ�������ķ��������Ķ���
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��װ�������ķ��������Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: RecoverMissSeqNum
-   ��������: ��ԭ��ʧ��Ƭ�����������ش�
-   ��    ��: ����д������Զ���Ķ�����ack����
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ԭ��ʧ��Ƭ�����������ش�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
        /* λ�������bitMap��ÿһλֵ������0˵����λ��Ч��ͨ��ƫ���������ȱʧ */
        if (0 < ((bitStart >> (uiNumbit >> 5))&(pstAckNackMsg->readerSNState.bitMap[uiNumbit >> 5])))
        {
            seqNumOffset.low = uiNumbit;
            seqNumMiss = Add_SequenceNumber(pstAckNackMsg->readerSNState.base, seqNumOffset);
            if (ISLESSTHEN_SEQUENCENUMBER(pstDataWriter->stHistoryCache.pstTail->seqNum, seqNumMiss))
            {
                break;
            }
            /* ȱʧ��Ƭ�ط� */
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
                //todo����gap����
            }
        }
    }
    //printf("sleep1\n");
    //Sleep(1);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringDataWriter_write
-   ��������: �û����ݷ��ͽӿ�
-   ��    ��: д�������û����ݡ����ݳ���
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �û����ݷ��ͽӿ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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

	/* ���뾲̬�ڴ洴��HistoryData����ڵ� */
	HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
	if (NULL == pstHistoryData)
	{
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
		return DDS_RETCODE_ERROR;
	}
	InitHistoryData(pstHistoryData);

	if (length <= DataFragSize)
	{
		/* ���뾲̬�ڴ洴����ʷ�������� */
        
		pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(length);
		
		if (NULL == pstHistoryData->data)
		{
			/* ʧ���ͷ�ǰ��������ڴ� */
			DDS_STATIC_FREE(pstHistoryData);
			PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
			return DDS_RETCODE_ERROR;
		}

		/* ���ݿ����������� */
		memcpy(pstHistoryData->data, buff, length);
	}

    /* ���û������ݳ��� */
    pstHistoryData->uiDataLen = length;

    /* ÿд��һ������д������ǰseqNum�Լ�1 */
    pstDataWriter->seqNum = Add_SequenceNumber(pstDataWriter->seqNum, SEQUENCENUMBER_START);

    /* �������ݼ�¼��ǰseqNum����Ա������ش� */
    pstHistoryData->seqNum = pstDataWriter->seqNum;

    /* �������ʱ��� */
    if (pstDataWriter->writerCallBack)
    {
        /* �û��Զ���ʱ��ص� */
        pstDataWriter->writerCallBack(&pstHistoryData->timeInfo);
    }
    else
    {
        /* Ĭ��ʱ��ص� */
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
                /* ʧ���ͷ�ǰ��������ڴ� */
                DDS_STATIC_FREE(pstHistoryData->pstFragData->data);
				DDS_STATIC_FREE(pstHistoryData);
                PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData->pstFragData, Insufficient memory space.\n");
                return DDS_RETCODE_ERROR;
            }
            pstHistoryData->uiFragLen = pstLength;
            pstHistoryData->uiFragNum = i;
            pstHistoryData->pstFragData->usFragIndex = i;
            pstHistoryData->pstFragData->usDataLen = pstLength;
            /* ���ݿ����������� */
            memcpy(pstHistoryData->pstFragData->data, buff + (i - 1)*DataFragSize, pstLength);
            /* ��װ�������ķ��͸����ĵ������Ķ��� */
            DDS_MUTEX_LOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
            retCode = JointUSERDataMsgSendUnknownReader(pstDataWriter, pstHistoryData);
            DDS_MUTEX_UNLOCK(pstDataWriter->pstTopic->pstParticipant->m_discRdMutex);
            DDS_STATIC_FREE(pstHistoryData->pstFragData->data);
        }
    }
    else
    {
        /* ��װ�������ķ��͸����ĵ������Ķ��� */
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
		/* ������ݲ��뵽���ػ����� */
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
		//todo �����ݴ��浽�ļ�ϵͳ�У�Ӳ���ϣ�
	}
#endif
    /* ��״̬����Ҫ�������ݣ��Ա������ش� */
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
    /* ��״̬���ͷ��ڴ� */
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
-   �� �� ��: DataWriterSendCacheMsg
-   ��������: ���������ش�
-   ��    ��: д������Զ���Ķ�����ȱʧseqNum
-   ��    ��: DDS_RETCODE_OK��DDS_RETCODE_ERROR
-   ȫ�ֱ���:
-   ע    ��: ���������ش�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_ReturnCode_t DataWriterSendCacheMsg(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader* pstDiscReader, SequenceNumber_t seqNum)
{
    SequenceNumber_t   seqNumOffset;
    UINT32             uiOffset;

    /* �����޻������ݻ���ack�����޶�ʧ��ֱ�ӷ��ز����� */
    if (NULL == pstDataWriter->stHistoryCache.pstHead)
    {
        //printf("1\n");
        return DDS_RETCODE_ERROR;
    }

    /* ȱʧ���Ĳ��ڻ������䣬ֱ�ӷ��ز����� */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataWriter->stHistoryCache.pstTail->seqNum, seqNum) || ISLESSTHEN_SEQUENCENUMBER(seqNum, pstDataWriter->stHistoryCache.pstHead->seqNum))
    {
        //printf("2\n");
		//printf("cache missed, seqNum: %d, headNum: %d, tailNum: %d.\n", seqNum.low, pstDataWriter->stHistoryCache.pstHead->seqNum.low, pstDataWriter->stHistoryCache.pstTail->seqNum.low);
        return DDS_RETCODE_ERROR;
    }

    /* ȱʧ���汨��ƫ���� */
    seqNumOffset = Sub_SequenceNumber(seqNum, pstDataWriter->stHistoryCache.pstHead->seqNum);

    HistoryData* pstHistoryData = pstDataWriter->stHistoryCache.pstHead;

    /* ͨ��ƫ��������ȱʧ���ģ���Ϊ�����������ģ�����һ�����Ƚϣ�ֱ���ҵ���Ӧλ�� */
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

    /* ƫ�������ҵ��Ļ��汨�ĶԲ��ϣ��������߼����� */
    if (!ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, seqNum))
    {
        printf("DataWriterSendCacheMsg: There is a problem with the history cache linked list, inconsistency of data, expect: %d, actually: %d.\n",seqNum.low, pstHistoryData->seqNum.low);
        printf("4\n");
        return DDS_RETCODE_ERROR;
    }

    /* ��ʼ���ͱ��� */
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

    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, 1024);

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

    /* ʱ�䱨��ͷ���� */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* ���л�ʱ�䱨��ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���л�ʱ���ӱ����� */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* InfoDT���� */
    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = pstDiscReader->guid.prefix;

    /* �ڶ������л��ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���������л��ӱ�����Ϣ�� */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    stSubMsgHeader.submessageId = GAP;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;

    /* �û����ݱ��������� */
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
    /* ���л�Gap����ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
    /* ���л�Gap������ */
    SERIALIZE_GAP(stGapMsg, stMemBlock);

    RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DelDataWriterHistoryCache
-   ��������: ���ڻ������
-   ��    ��: ����
-   ��    ��: 
-   ȫ�ֱ���:
-   ע    ��: ���ڻ������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
            /* ������С����seqNum���ȱ���д������󻺴��1����Զ���Ķ�������seqNum�����ڵ���������򻺴�ȫ����� */
            seqNumMin = Add_SequenceNumber(pstDataWriter->stHistoryCache.pstTail->seqNum, SEQUENCENUMBER_START);

            /* ��ȡÿ��������Զ���Ķ�����С������seqNum */
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
            /* ����д����������ɾ����С����seqNum֮ǰ�������� */
            psHistoryData = pstDataWriter->stHistoryCache.pstHead;
            while (NULL != psHistoryData)
            {
                if (ISLESSTHEN_SEQUENCENUMBER(psHistoryData->seqNum, seqNumMin))
                {
                    psHistoryDataDel = psHistoryData;
                }
                /* ���ڵ�����СseqNumֵ������ */
                else
                {
                    break;
                }
                psHistoryData = psHistoryData->pNext;

                /* �����ͷ� */
                DDS_STATIC_FREE(psHistoryDataDel->data);
                DDS_STATIC_FREE(psHistoryDataDel);

                /* ͷ�ڵ�ƫ�� */
                pstDataWriter->stHistoryCache.pstHead = psHistoryData;
                pstDataWriter->stHistoryCache.uiHistoryDataNum--;
            }
			//printf("headSeqNum: %d\n", pstDataWriter->stHistoryCache.pstHead->seqNum.low);
            /* ͷ�ڵ�Ϊ�գ���Ϊ������β�ڵ�Ҳ���ÿ� */
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
	/* ���뾲̬�ڴ洴����ʷ�������� */
	pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(len);

	if (NULL == pstHistoryData->data)
	{
		/* ʧ���ͷ�ǰ��������ڴ� */
		DDS_STATIC_FREE(pstHistoryData);
		PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
		return DDS_RETCODE_ERROR;
	}

	/* ���ݿ����������� */
	memcpy(pstHistoryData->data, buf, len);

	/* ���û������ݳ��� */
	pstHistoryData->uiDataLen = len;

	/* �������ʱ��� */
	if (pstDataWriter->writerCallBack)
	{
		/* �û��Զ���ʱ��ص� */
		pstDataWriter->writerCallBack(&pstHistoryData->timeInfo);
	}
	else
	{
		/* Ĭ��ʱ��ص� */
		pstHistoryData->timeInfo = GetNowTime();
	}
	DDS_ReturnCode_t retCode;
	/* ��װ�������ķ��͸����ĵ������Ķ��� */
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

