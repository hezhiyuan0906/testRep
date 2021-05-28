#include "../Include/GlobalDefine.h"
/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataReader_Init
-   ��������: �Ķ�����ʼ��
-   ��    ��: �Ķ���ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �Ķ�����ʼ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataReader_Init(DDS_DataReader* pstDataReader)
{
    if (NULL != pstDataReader)
    {
        pstDataReader->lastRecvTime.sec = 0;
        pstDataReader->lastRecvTime.nanosec = 0;
        pstDataReader->totHistoryNum = 0;
    //    InitHistoryCache(&pstDataReader->stUserData);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataReader_Uninit
-   ��������: д������ʼ��
-   ��    ��: д����ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: д������ʼ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataReader_Uninit(DDS_DataReader* pstDataReader)
{
    if (NULL != pstDataReader)
    {
    //    UninitHistoryCache(&pstDataReader->stUserData);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataReader_delete
-   ��������: �Ķ���ɾ��
-   ��    ��: �Ķ���ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �Ķ���ɾ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DataReader_delete(DDS_DataReader* pstDataReader)
{
    DDS_DataReader* pstPreDataReader = NULL;
    DDS_Topic* pstTopic = pstDataReader->pstTopic;

    DDS_DataReader* pstCurDataReader = pstTopic->pstLocalReader;
    while (NULL != pstCurDataReader)
    {
        if (pstCurDataReader == pstDataReader)
        {
            if (NULL == pstPreDataReader)
            {
                if (pstCurDataReader->pNext != NULL)
                {
                    pstTopic->pstLocalReader = pstCurDataReader->pNext;
                }
                DDS_DataReader_Uninit(pstCurDataReader);
                pstCurDataReader = pstTopic->pstLocalReader;
            }
            else
            {
                pstPreDataReader->pNext = pstCurDataReader->pNext;
                DDS_DataReader_Uninit(pstCurDataReader);
                pstCurDataReader = pstPreDataReader->pNext;
            }
            break;
        }
        else
        {
            pstPreDataReader = pstCurDataReader;
            pstCurDataReader = pstCurDataReader->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: JointAckNackMsgSendAssignWriter
-   ��������: ��Զ��writer�ظ�ack����
-   ��    ��: ����д����ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Զ��writer�ظ�ack����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointAckNackMsgSendAssignWriter(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoDestination   stInfoDest;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* �����ֽ����ʼ�� */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* ��һ�����л�RTPS����ͷ */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstDiscWriter->pstTopic->pstParticipant->stRTPSMsgHeader, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = pstDiscWriter->guid.prefix;

    /* �ڶ������л��ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���������л��ӱ�����Ϣ�� */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* ����ACK����ͷ */
    stSubMsgHeader.submessageId = ACKNACK;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //finalFlag
    if (pstDiscWriter->stAckNack.readerSNState.numBits)
    {
        stSubMsgHeader.flags[1] = FALSE;
    }
    else
    {
        stSubMsgHeader.flags[1] = TRUE;
    }

    /* ����ACK������Ϣ�� */
    pstDiscWriter->stAckNack.readerId = pstDataReader->guid.entityId;
    pstDiscWriter->stAckNack.writerId = pstDiscWriter->guid.entityId;
    pstDiscWriter->stAckNack.count++;

    /* �ڶ������л��ӱ���ͷ */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* ���������л��ӱ�����Ϣ�� */
    SERIALIZE_ACKNACK(pstDiscWriter->stAckNack, stMemBlock);
    

    /* �ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDiscWriter->stAckNack.uiStartIndex;

    /* ���л��޸��ӱ��ĳ��� */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* ����ack���� */
    RTPSSendMsg(&stMemBlock, &pstDataReader->pstTopic->pstParticipant->socketList.user_data_sender, pstDiscWriter->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetMissSeqNum
-   ��������: ��ȡ��ʧƬ��
-   ��    ��: �����Ķ�����Զ��д�������������seqNum
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ȡ��ʧƬ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID GetDataReaderMissSeqNum(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, SequenceNumber_t lastSN)
{
    /* HistoryCache��seqNum����ɢ�������У�expectSeqNum��lastSN����ɢ�����������У�������һ���ԣ��鲢�Ƚ�һ�μ����ҳ�����ȱʧƬ�Σ������ݵݹ顢�����˱����ɣ�ɧ�� �м��м� */
    HistoryData* pstHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    UINT32 uiNumbit = 0;

    /* ������գ�ÿһλ����ʾ��ǰ����Ƿ�ȱʧ */
    memset(pstDiscWriter->stAckNack.readerSNState.bitMap, 0, SEQUENCE_BIT_NUMBER);

    SequenceNumber_t tempSN = pstDiscWriter->stAckNack.readerSNState.base;

    /* ���ֵ������ֵС��˵��δ��ʧ */
    if (ISLESSTHEN_SEQUENCENUMBER(lastSN, tempSN))
    {
        pstDiscWriter->stAckNack.readerSNState.numBits = 0;
        return;
    }

    while (!ISEQUAL_SEQUENCENUMBER(tempSN, Add_SequenceNumber(lastSN, SEQUENCENUMBER_START)))
    {
        if (NULL != pstHistoryData)
        {
            /* ���������������ݣ����� */
            if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, tempSN))
            {
                pstHistoryData = pstHistoryData->pNext;
            }
            /* ������Ҳû���������ݣ���¼ȱʧ */
            else
            {
                /* ÿһλ��¼�Ƿ���ȱʧ����base��ʼ�������¼����256���Ƿ���ȱʧ, λ���������ͳ�ĳ�����ȡģ���� */
                pstDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));;
            }
        }
        else
        {
            /* ��������꣬ʣ��ȫ��ȱʧ����¼ȱʧ */
            pstDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));;
        }
        pstDiscWriter->stAckNack.readerSNState.numBits = uiNumbit + 1;

        tempSN = Add_SequenceNumber(tempSN, SEQUENCENUMBER_START);

        /* ������������256�����������ȱʧ���ȴ��´��������� */
        uiNumbit++;
        if (256 == uiNumbit)
        {
            return;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: CorrectAllHistoryCache
-   ��������: reader�ܻ���ﵽdepth���޺���������writer����ɻ���
-   ��    ��: �����Ķ���
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: 
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\12\08       1.0        Frank       Created
----------------------------------------------------------------------------------*/
VOID CorrectOldHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter)
{
    HistoryData* pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    while (NULL != pstTempHistoryData)
    {
        /*���������ٵ���depth�������£������ύ*/
        if (pstDiscWriter->stHistoryCache.uiHistoryDataNum <= pstDataReader->stDataReaderQos.history.depth)
        {
            return ;
        }
        else
        {
            /* ���������޳� */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* �ύ���û� */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* ÿ�ύһ�Σ��û����ݸ������� */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* ���������Լ� */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

            /*��������*/
            pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstTempHistoryData->seqNum, SEQUENCENUMBER_START);
        }
        pstTempHistoryData = pstTempHistoryData->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: CorrectDiscoveredWriterHistoryCache
-   ��������: ����Զ��д�����������ύ
-   ��    ��: �����Ķ�����Զ��д����
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����Զ��д�����������ύ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID CorrectDiscoveredWriterHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter)
{
    /* Զ��д��������Ϊ��ɢ������������һ���Կ����ύ�����Լ��������� */
    HistoryData* pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    while (NULL != pstTempHistoryData)
    {
        /* ��������С������ֵ������˳��ȫ���ύ */
        if (ISLESSTHEN_SEQUENCENUMBER(pstTempHistoryData->seqNum, pstDiscWriter->stAckNack.readerSNState.base))
        {
            /* ���ʱע��β�ڵ� */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* ���������޳� */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* �ύ���û� */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* ÿ�ύһ�Σ��û����ݸ������� */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* ���������Լ� */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;
        }
        /* �������ݵ�������ֵ����ɢ�������������ύ */
        else if (ISEQUAL_SEQUENCENUMBER(pstTempHistoryData->seqNum, pstDiscWriter->stAckNack.readerSNState.base))
        {
            /* ���ʱע��β�ڵ� */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* ���������޳� */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* �ύ���û� */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* ÿ�ύһ�Σ��û����ݸ������� */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* ���������Լ� */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

            /* �����´�������seqNum */
            pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
			if (pstDiscWriter->stDataWriterQos.history.kind == KEEP_ALL_HISTORY_QOS && pstDiscWriter->stDataWriterQos.durability.kind == TRANSIENT_DURABILITY_QOS)
			{
				if (pstTempHistoryData->seqNum.low == pstDiscWriter->stDataWriterQos.history.depth)
				{
					pstDiscWriter->durabilityFlag = TRUE;
					//pstDiscWriter->stAckNack.readerSNState.base = pstTempHistoryData->seqNum;
				}
			}

        }
        /* �������ݴ�������ֵ*/
        else
        {
			//�����KEEP_ALL��������ֵ����Ϊ��ǰseqNum
			if (pstDiscWriter->stDataWriterQos.history.kind == KEEP_ALL_HISTORY_QOS && pstDiscWriter->stDataWriterQos.durability.kind == TRANSIENT_DURABILITY_QOS)
			{
				/* �����´�������seqNum */
				if (pstTempHistoryData->seqNum.low == pstDiscWriter->stDataWriterQos.history.depth)
				{
					pstDiscWriter->durabilityFlag = TRUE;
					pstDiscWriter->stAckNack.readerSNState.base = pstTempHistoryData->seqNum;
				}
				else if (pstTempHistoryData->seqNum.low < pstDiscWriter->stDataWriterQos.history.depth)
				{
					/*pstDiscWriter->stAckNack.readerSNState.base = pstTempHistoryData->seqNum;
					pstDiscWriter->stAckNack.readerSNState.base.low += 1;*/
				}
				else if (pstDiscWriter->durabilityFlag == TRUE)
				{
                    /* ���������޳� */
                    pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

                    /* �ύ���û� */
                    DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

                    /* ÿ�ύһ�Σ��û����ݸ������� */
                    pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

                    /* ���������Լ� */
                    pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

					pstDiscWriter->stAckNack.readerSNState.base = pstTempHistoryData->seqNum;
                    //pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
				}
			}
            /*�����KEEP_LAST����ȴ������ش���ֱ��return*/
			else
			{
				return;
			}

        }
        //pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
        pstTempHistoryData = pstTempHistoryData->pNext;
    }
}

VOID CorrectDiscoveredWriterGapMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, Gap* pstGapMsg)
{
    HistoryData* pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    SequenceNumber_t gapEnd = pstGapMsg->gapStart;
    gapEnd.low += pstGapMsg->gapList.numBits;
    while (NULL != pstTempHistoryData)
    {
        /*��Gap���������ʧ����SeqNumС�ģ�ȫ���ύ*/
        if (ISLESSTHEN_SEQUENCENUMBER(pstTempHistoryData->seqNum, gapEnd))
        {
            /* ���ʱע��β�ڵ� */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* ���������޳� */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* �ύ���û� */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* ÿ�ύһ�Σ��û����ݸ������� */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* ���������Լ� */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;
            pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
            continue;
        }
        else
        {
            pstDiscWriter->stAckNack.readerSNState.base = gapEnd;
            break;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeilverStatuslessUserMsg
-   ��������: �ύ��״̬�Ķ�������
-   ��    ��: �Ķ�����Զ��д�������û���������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �ύ��״̬�Ķ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeilverStatuslessUserMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, HistoryData* pstHistoryData)
{
    if (pstDiscWriter == NULL)
    {
        return;
    }
    if (NULL != pstDataReader->pstTopic->stUserData.pstHead && pstHistoryData->uiFragTotal == 0)
    {
        printf("After reading the data, Please clean up the memory");
        exit(EXIT_FAILURE);
    }

    if (pstHistoryData->uiFragTotal > 0)
    {
        
        if (pstHistoryData->uiFragNum == 1)
        {
            do{
                    pstHistoryData->pNext = NULL;
                    if (NULL == pstDataReader->pstTopic->stUserData.pstHead || NULL == pstDataReader->pstTopic->stUserData.pstTail)
                    {                                               
                        pstDataReader->pstTopic->stUserData.pstHead = pstHistoryData;
                        pstDataReader->pstTopic->stUserData.pstTail = pstHistoryData;
                    }
                    else
                    {
                        pstDataReader->pstTopic->stUserData.pstTail->pNext = pstHistoryData;
                        pstDataReader->pstTopic->stUserData.pstTail = pstHistoryData;
                    }
            } while (0);
            //return;
        }
        else
        {
            if (!(NULL == pstDataReader->pstTopic->stUserData.pstHead || NULL == pstDataReader->pstTopic->stUserData.pstTail))
            {
                //printf("dataLen: %d\n", strlen(pstHistoryData->data));
                if (pstDataReader->pstTopic->stUserData.pstTail->uiFragNum == (pstHistoryData->uiFragNum - 1))
                {
                    pstDataReader->pstTopic->stUserData.pstTail->uiFragNum = pstHistoryData->uiFragNum;
                    pstDataReader->pstTopic->stUserData.pstTail->uiFragLen = pstHistoryData->uiFragLen;
                    pstDataReader->pstTopic->stUserData.pstTail->seqNum = pstHistoryData->seqNum;
                    memcpy(pstDataReader->pstTopic->stUserData.pstTail->data + (63 * 1024) * (pstHistoryData->uiFragNum - 1), pstHistoryData->data, pstHistoryData->uiFragLen);
                }
                else
                {
                    HistoryData* tmpHistoryData = pstDataReader->pstTopic->stUserData.pstHead;
                    do 
                    {
                        if (ISEQUAL_SEQUENCENUMBER(tmpHistoryData->seqNum, pstHistoryData->seqNum))
                        {
                            if ((tmpHistoryData->uiFragNum + 1) == pstHistoryData->uiFragNum)
                            {
                                
                                memcpy(tmpHistoryData->data + (63 * 1024)*tmpHistoryData->uiFragNum, pstHistoryData->data, pstHistoryData->uiFragLen);
                            }
                            else
                            {
                                UninitHistoryCache(&pstDiscWriter->pstTopic->stUserData);
                                return;
                            }
                        }
                    } while (tmpHistoryData->pNext != NULL);
                }
            }            
        }
    }
    else
    {
        /* �ύ���û� */
        DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstHistoryData);
    }

    if (pstHistoryData->uiFragTotal == 0 || (pstHistoryData->uiFragTotal == pstHistoryData->uiFragNum))
    {
        /* ÿ�ύһ�Σ��û����ݸ������� */
        pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;
        /* �����´�������seqNum */
        pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
    }
    while (NULL != pstDataReader)
    {
        /* �ص������������û����� */
        if (NULL != pstDataReader->recvCallBack && ((pstHistoryData->uiFragTotal == pstHistoryData->uiFragNum)))
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
    if (pstHistoryData->uiFragTotal == 0 || (pstHistoryData->uiFragTotal == pstHistoryData->uiFragNum))
    {
        UninitHistoryCache(&pstDiscWriter->pstTopic->stUserData);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeilverStatusFulUserMsg
-   ��������: �ύ��״̬�Ķ�������
-   ��    ��: �Ķ�����Զ��д�������û���������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �ύ��״̬�Ķ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeilverStatusFulUserMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, HistoryData* pstHistoryData)
{
    if (pstDiscWriter == NULL)
    {
        return;
    }
    if (NULL != pstDataReader->pstTopic->stUserData.pstHead)
    {
        printf("After reading the data, Please clean up the memory");
        exit(EXIT_FAILURE);
    }

    /*DeadLine����ֱ���ύ*/
    if (strcmp(pstHistoryData->data, "DeadLine MSG.") == 0)
    {
        /* �ύ���û� */
        if (NULL != pstDataReader)
        {
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstHistoryData);
        }
        while (NULL != pstDataReader)
        {

            /* �ص������������û����� */
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
        return;
    }

    /* ������������ֱ��ɾ����û����� */
    if (!InsertHistoryCacheOrder(&pstDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData->data);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_INFO, "HandleUSERWriterDataMsg:User data was not inserted, Because user data already exists.\n");
    }
    
    /*����history���ޣ�������ɻ���*/
    if ((pstDataReader->stDataReaderQos.history.kind == KEEP_LAST_HISTORY_QOS) && (pstDataReader->stDataReaderQos.history.depth != 0)
        && (pstDiscWriter->stHistoryCache.uiHistoryDataNum > pstDataReader->stDataReaderQos.history.depth))
    {
        CorrectOldHistoryCache(pstDataReader, pstDiscWriter);
    }
    else
    {
        /* �������� */
        CorrectDiscoveredWriterHistoryCache(pstDataReader, pstDiscWriter);
    }

    

    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {

            /* �ص������������û����� */
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
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN
-   ��������: �ύ��״̬�Ķ������ݣ�������seqNum������firstSN��С
-   ��    ��: �Ķ�����Զ��д�������û���������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �ύ��״̬�Ķ������ݣ�������seqNum������firstSN��С
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\15       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, SequenceNumber_t firstSN)
{
    if (pstDiscWriter == NULL)
    {
        return;
    }
    if (NULL != pstDataReader->pstTopic->stUserData.pstHead)
    {
        printf("After reading the data, Please clean up the memory");
        exit(EXIT_FAILURE);
    }

    /* ��������ֵ��С��Ϊ������Сֵ */
    pstDiscWriter->stAckNack.readerSNState.base = firstSN;

    /* ��������*/
    CorrectDiscoveredWriterHistoryCache(pstDataReader, pstDiscWriter);
    if (NULL == pstDataReader)
    {
        return;
    }
    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {
            /* �ص������������û����� */
            if (NULL != pstDataReader->recvCallBack)
            {
                (pstDataReader->recvCallBack)(pstDataReader);
            }

            pstDataReader = pstDataReader->pNext;
        }

        UninitHistoryCache(&pstDiscWriter->pstTopic->stUserData);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataReader_take
-   ��������: ��ȡ�û�����
-   ��    ��: �Ķ�����DataSeq
-   ��    ��: DDS_RETCODE_OK��DDS_RETCODE_NO_DATA
-   ȫ�ֱ���:
-   ע    ��: ��ȡ�û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_StringDataReader_take(DDS_DataReader* pstDataReader, DataSeq* pstDataSeq)
{
    if (NULL == pstDataReader->pstTopic->stUserData.pstHead)
    {
        return DDS_RETCODE_NO_DATA;
    }
    HistoryData* tmpHistoryData = pstDataReader->pstTopic->stUserData.pstHead;
    while (tmpHistoryData->uiFragTotal != tmpHistoryData->uiFragNum)
    {
        if (tmpHistoryData->pNext != NULL)
        {
            tmpHistoryData = tmpHistoryData->pNext;
        }
    }
    /* �������ݸ�����ֵ */
    if (strcmp(tmpHistoryData->data, "DeadLine MSG.") == 0)
    {
        pstDataSeq->length = 1;
    }
    else
    {
        pstDataSeq->length = pstDataReader->pstTopic->stUserData.uiHistoryDataNum;
    }
    /* ���������׵�ַ */
    pstDataSeq->pstHead = tmpHistoryData;

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DataReader_take
-   ��������: ��ȡ�û�����
-   ��    ��: �Ķ�����DataSeq
-   ��    ��: DDS_RETCODE_OK��DDS_RETCODE_NO_DATA
-   ȫ�ֱ���:
-   ע    ��: ��ȡ�û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DataSeq*  DDS_StringDataReader_take_cs(DDS_DataReader* pstDataReader)
{
    if (NULL == pstDataReader->pstTopic->stUserData.pstHead)
    {
        return NULL;
    }

    DataSeq* pstDataSeq = (DataSeq*)DDS_STATIC_MALLOC(sizeof(DataSeq));
    if (NULL == pstDataSeq)
    {
        return NULL;
    }

    /* �������ݸ�����ֵ */
    pstDataSeq->length = pstDataReader->pstTopic->stUserData.uiHistoryDataNum;

    /* ���������׵�ַ */
    pstDataSeq->pstHead = pstDataReader->pstTopic->stUserData.pstHead;

    return pstDataSeq;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringSeq_length
-   ��������: �����û�����
-   ��    ��: DataSeq�����ݱ��
-   ��    ��: �û�����ָ��
-   ȫ�ֱ���:
-   ע    ��: �����û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL UINT32 DDS_StringSeq_length(DataSeq* pstDataSeq)
{
    if (NULL == pstDataSeq)
    {
        return 0;
    }

    return pstDataSeq->length;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringSeq_get
-   ��������: �����û�����
-   ��    ��: DataSeq�����ݱ��
-   ��    ��: �û�����ָ��
-   ȫ�ֱ���:
-   ע    ��: �����û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL CHAR* DDS_StringSeq_get(DataSeq* pstDataSeq, UINT32 num, DataLen* pDataLen)
{
    UINT32 uiNum;

    HistoryData* pstHistoryData = pstDataSeq->pstHead;

    for (uiNum = 0; uiNum < pstDataSeq->length; uiNum++)
    {
        if (uiNum == num)
        {
            *pDataLen = pstHistoryData->uiDataLen;
            return pstHistoryData->data;
        }

        pstHistoryData = pstHistoryData->pNext;
    }
    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringSeq_get
-   ��������: �����û�����
-   ��    ��: DataSeq�����ݱ��
-   ��    ��: �û�����ָ��
-   ȫ�ֱ���:
-   ע    ��: �����û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL BOOL DDS_TimeSeq_get(DataSeq* pstDataSeq, UINT32 num, Time_t* pstTimeInfo)
{
    UINT32 uiNum;

    HistoryData* pstHistoryData = pstDataSeq->pstHead;

    for (uiNum = 0; uiNum < pstDataSeq->length; uiNum++)
    {
        if (uiNum == num)
        {
            *pstTimeInfo = pstHistoryData->timeInfo;
            return TRUE;
        }

        pstHistoryData = pstHistoryData->pNext;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringSeq_get
-   ��������: �����û�����
-   ��    ��: DataSeq�����ݱ��
-   ��    ��: �û�����ָ��
-   ȫ�ֱ���:
-   ע    ��: �����û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_StringSeq_get_cs(DataSeq* pstDataSeq, UINT32 num,  CHAR* msg)
{
    UINT32 uiNum;

    HistoryData* pstHistoryData = pstDataSeq->pstHead;

    for (uiNum = 0; uiNum < pstDataSeq->length; uiNum++)
    {
        if (uiNum == num)
        {
            memcpy(msg, pstHistoryData->data, pstHistoryData->uiDataLen);

            return;
        }

        pstHistoryData = pstHistoryData->pNext;
    }

    return ;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringSeq_get
-   ��������: �����û�����
-   ��    ��: DataSeq�����ݱ��
-   ��    ��: �û�����ָ��
-   ȫ�ֱ���:
-   ע    ��: �����û�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL UINT32 DDS_StringSeq_len_cs(DataSeq* pstDataSeq, UINT32 num)
{
    UINT32 uiNum;

    HistoryData* pstHistoryData = pstDataSeq->pstHead;

    for (uiNum = 0; uiNum < pstDataSeq->length; uiNum++)
    {
        if (uiNum == num)
        {
            return  pstHistoryData->uiDataLen;

        }

        pstHistoryData = pstHistoryData->pNext;
    }

    return 0;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_StringDataReader_return_loan
-   ��������: �û�������Դ�ͷ�
-   ��    ��: �����Ķ���
-   ��    ��: DDS_RETCODE_OK��DDS_RETCODE_NO_DATA
-   ȫ�ֱ���:
-   ע    ��: �û�������Դ�ͷ�, ��ȡ�û����ݺ��������ͷţ��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_StringDataReader_return_loan(DDS_DataReader* pstDataReader, DataSeq* pstDataSeq)
{
    UINT32 uiNum;
    HistoryData* pstDelHistoryData = NULL;

    HistoryData* pstHistoryData = pstDataSeq->pstHead;

    for (uiNum = 0; uiNum < pstDataSeq->length; uiNum++)
    {
        pstDelHistoryData = pstHistoryData;

        pstHistoryData = pstHistoryData->pNext;

        /* �ͷŻ������е������� */
        DDS_STATIC_FREE(pstDelHistoryData->data);

        /* �ͷŻ����� */
        DDS_STATIC_FREE(pstDelHistoryData);
    }

    /* �����ʼ�� */
    InitHistoryCache(&pstDataReader->pstTopic->stUserData);

    return DDS_RETCODE_OK;
}


