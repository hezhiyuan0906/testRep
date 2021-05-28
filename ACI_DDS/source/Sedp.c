#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleSEDPDiscoveredWriterDataMsg
-   ��������: ����SEDP����д��������
-   ��    ��: �ӱ�����Ϣͷ��Data���ڴ�顢Participant
-   ��    ��:
-   ȫ�ֱ���: 
-   ע    ��: ����SEDP����д��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\8\05       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleSEDPDiscoveredWriterDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    DDS_DiscoveredWriter* pstTempDiscWriter = NULL;

    if (!pstSubMsgHeader->flags[1] && !(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "SEDPDiscoveredWriter: SubMsg header is abnormal!\n");
        return FALSE;
    }

    /* �ڽ�������writer */
    BuiltinDiscWriter*  pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(&pstParticipant->stBuiltinDataReader[1], &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "SEDPDiscoveredWriter: Not found SUB BuiltinDiscWriter !.\n");
        return FALSE;
    }

    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create HistoryCache, Insufficient memory space.\n");
        return FALSE;
    }

    /* ��ʼ����ʷ���� */
    InitHistoryData(pstHistoryData);

    /* ��¼seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* ��������qos */
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
        if (stInlineQos.disposed)
        {
            DDS_Topic* pstTopic = pstParticipant->pstTopic;
            while (NULL != pstTopic)
            {
                /* Զ��д������� */
                DDS_MUTEX_LOCK(pstParticipant->m_discWtMutex);
                DeleteDiscWriterFromTopicByGuid(pstTopic, &stInlineQos.guid);
                DDS_MUTEX_UNLOCK(pstParticipant->m_discWtMutex);

                pstTopic = pstTopic->pNext;
            }
        }
    }

    /* ���������� */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        /* �������������ڴ� */
        DDS_Topic* pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
        if (NULL == pstTopic)
        {
            DDS_STATIC_FREE(pstHistoryData);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create remote topic, Insufficient memory space.\n");
            return FALSE;
        }

        /* ��ʼ������ */
        DDS_Topic_Init(pstTopic);

        /* ����Զ��д���������ڴ� */
        DDS_DiscoveredWriter*  pstDiscWriter = (DDS_DiscoveredWriter*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredWriter));
        if (NULL == pstDiscWriter)
        {
            DDS_STATIC_FREE(pstHistoryData);
            DDS_STATIC_FREE(pstTopic);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create remote DiscoveredWriter, Insufficient memory space.\n");
            return FALSE;
        }

        /* ��ʼ��д���� */
        DDS_DiscoveredWriter_Init(pstDiscWriter);

        /* ���л�data present */
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));
        NetworkByteOrderConvertDiscDataWriter(pstMemBlock, pstTopic, pstDiscWriter);

        /* Participant��ַ�ҵ���Ӧ���Ķ����� */
        DDS_DiscoveredParticipant* pstDiscParticipant = pstParticipant->pstDiscParticipant;
        while (NULL != pstDiscParticipant)
        {
            if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscWriter->guid.prefix))
            {
                pstDiscWriter->pstUnicastLocator = pstDiscParticipant->rtpsParticipantProxy.defaultUnicastLocator;
                break;
            }

            pstDiscParticipant = pstDiscParticipant->pNext;
        }

        /* У�鱾���Ƿ���ڸ����� */
        DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
        while (NULL != pstTempTopic)
        {
            /* ���Ȿ���Ƿ��Ѵ��� */
            if (!strcmp(pstTopic->topicName.value, pstTempTopic->topicName.value) && !strcmp(pstTopic->topicType.value, pstTempTopic->topicType.value))
            {
                /* �����������Ѵ��ڸ�Զ��д���������ͷ��ڴ�ֱ�ӷŻ� */
                pstTempDiscWriter = GetDiscoveredWriterFromTopicByGuid(pstTempTopic, &pstDiscWriter->guid);
                if (NULL != pstTempDiscWriter)
                {
                    PrintLog(COMMLOG_INFO, "SEDPDiscoveredWriter: The discovered writer is exist | topic name: %s.\n", pstTopic->topicName.value);
                    DDS_STATIC_FREE(pstTopic);
                    DDS_STATIC_FREE(pstDiscWriter);
                    DDS_STATIC_FREE(pstHistoryData);
                    return TRUE;
                }
                /* �ͷ��½������� */
                DDS_STATIC_FREE(pstTopic);

                /* �����ڽ�������reader,ʧ�������� */
                if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
                {
                    DDS_STATIC_FREE(pstHistoryData);
                }

                /* ���븸�ڵ㣬��������ϲ����ݽṹ */
                pstDiscWriter->pstTopic = pstTempTopic;

                /* Զ��д���������ڣ������ */
                LIST_INSERT_HEAD(pstTempTopic->pstDiscWriter, pstDiscWriter);

                return TRUE;
            }
            pstTempTopic = pstTempTopic->pNext;
        }

        /* ���븸�ڵ㣬��������ϲ����ݽṹ */
        pstDiscWriter->pstTopic = pstTopic;

        /* Զ��д�������뵽��������*/
        LIST_INSERT_HEAD(pstTopic->pstDiscWriter, pstDiscWriter);

        /* ���븸�ڵ㣬��������ϲ����ݽṹ */
        pstTopic->pstParticipant = pstParticipant;

        /* ����������뵽Participant�µ��������� */
        LIST_INSERT_HEAD(pstParticipant->pstTopic, pstTopic);
    }

    /* �����ڽ�������reader,ʧ�������� */
    if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleSEDPDiscoveredReaderDataMsg
-   ��������: ����SEDP�����Ķ�������
-   ��    ��: �ӱ�����Ϣͷ��Data���ڴ�顢Participant
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����SEDP�����Ķ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\8\05       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleSEDPDiscoveredReaderDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    DDS_DiscoveredReader* pstTempDiscReader = NULL;
    DDS_DiscoveredReader* pstTwinDiscReader = NULL;
    DDS_DataWriter*  pstLocalWriter = NULL;

    if (!pstSubMsgHeader->flags[1] && !(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "SEDPDiscoveredReader:  SubMsg header is abnormal !\n");
        return FALSE;
    }

    /* �ڽ�������reader */
    BuiltinDiscWriter*  pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(&pstParticipant->stBuiltinDataReader[2], &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "SEDPDiscoveredReader: Not found SUB BuiltinDiscWriter !\n");
        return FALSE;
    }

    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "SEDPDiscoveredReader: Failed to create HistoryCache, Insufficient memory space.\n");
        return FALSE;
    }

    /* ��ʼ����ʷ���� */
    InitHistoryData(pstHistoryData);

    /* ��¼seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* �����л�����qos��Ϣ */
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0};
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);

        if (stInlineQos.disposed)
        {
            DDS_Topic* pstTopic = pstParticipant->pstTopic;
            while (NULL != pstTopic)
            {
                /* Զ���Ķ������ */
                DDS_MUTEX_LOCK(pstParticipant->m_discRdMutex);
                DeleteDiscReaderFromTopicByGuid(pstTopic, &stInlineQos.guid);
                DDS_MUTEX_UNLOCK(pstParticipant->m_discRdMutex);

                pstTopic = pstTopic->pNext;
            }
        }
    }

    /* �����ı��� */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        /* �������������ڴ� */
        DDS_Topic* pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
        if (NULL == pstTopic)
        {
            DDS_STATIC_FREE(pstHistoryData);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredReader: Failed to create remote topic, Insufficient memory space.\n");
            return FALSE;
        }

        /* ��ʼ������ */
        DDS_Topic_Init(pstTopic);

        /* ����Զ��д���������ڴ� */
        DDS_DiscoveredReader*  pstDiscReader = (DDS_DiscoveredReader*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredReader));
        if (NULL == pstDiscReader)
        {
            /* ʧ�����ͷ�֮ǰ���������Ⲣ���� */
            DDS_STATIC_FREE(pstHistoryData);
            DDS_STATIC_FREE(pstTopic);

            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredReader: Failed to create remote DiscoveredWriter, Insufficient memory space.\n");
            return FALSE;
        }

        /* ��ʼ��Զ���Ķ��� */
        DDS_DiscoveredReader_Init(pstDiscReader);

        /* �����л�data present */
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));
        NetworkByteOrderConvertDiscDataReader(pstMemBlock, pstTopic, pstDiscReader);
    
        /* Participant��ַ�ҵ���Ӧ���Ķ����� */
        DDS_DiscoveredParticipant* pstDiscParticipant = pstParticipant->pstDiscParticipant;
        while (NULL != pstDiscParticipant)
        {
            if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader->pstUnicastLocator = pstDiscParticipant->rtpsParticipantProxy.defaultUnicastLocator;
                break;
            }

            pstDiscParticipant = pstDiscParticipant->pNext;
        }

        /* У�鱾���Ƿ���ڸ����� */
        DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
        while (NULL != pstTempTopic)
        {
            /* ���ֱ����Ѵ��������⣬У�����������Ƿ�һ��*/
            if (!strcmp(pstTopic->topicName.value, pstTempTopic->topicName.value) && !strcmp(pstTopic->topicType.value, pstTempTopic->topicType.value))
            {
                /* �����������Ѵ��ڸ�Զ��д���������ͷ��ڴ�ֱ�ӷŻ� */
                pstTempDiscReader = GetDiscoveredReaderFromTopicByGuid(pstTempTopic, &pstDiscReader->guid);
                if (NULL != pstTempDiscReader)
                {
                    PrintLog(COMMLOG_INFO, "SEDPDiscoveredReader: The discovered reader is exist | topic name: %s.\n", pstTopic->topicName.value);
                    DDS_STATIC_FREE(pstTopic);
                    DDS_STATIC_FREE(pstDiscReader);
                    DDS_STATIC_FREE(pstHistoryData);
                    return TRUE;
                }
                /* �ͷ��½������� */
                DDS_STATIC_FREE(pstTopic);

                /* �����ڽ�������reader,�Ѵ��������� */
                if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
                {
                    DDS_STATIC_FREE(pstHistoryData);
                }

                /* ����Զ��reader���� */
                pstTempDiscReader =  GetDiscoveredReaderFromTopicByPrefix(pstTempTopic, &pstDiscReader->guid.prefix);
                if (NULL != pstTempDiscReader)
                {
                    LIST_INSERT_TAIL(pstTempDiscReader->pstTwinDiscReader, pstDiscReader, pstTwinDiscReader);
                    return TRUE;
                }

                /* ���븸�ڵ㣬��������ϲ����ݽṹ */
                pstDiscReader->pstTopic = pstTempTopic;

                /* Զ��д���������ڣ������ */
                LIST_INSERT_HEAD(pstTempTopic->pstDiscReader, pstDiscReader);

                /* Զ��reader��ӱ���writer */
                
                pstLocalWriter = pstTempTopic->pstLocalWriter;
                while (NULL != pstLocalWriter)
                {
                    DiscoveredReaderAddLocalWriter(pstDiscReader, &pstLocalWriter->guid);
                    pstLocalWriter = pstLocalWriter->pNext;
                }
                return TRUE;
            }
            pstTempTopic = pstTempTopic->pNext;
        }

        /* ���븸�ڵ㣬��������ϲ����ݽṹ */
        pstDiscReader->pstTopic = pstTempTopic;

        /* Զ��д�������뵽��������*/
        LIST_INSERT_HEAD(pstTopic->pstDiscReader, pstDiscReader);

        /* ���븸�ڵ㣬��������ϲ����ݽṹ */
        pstTopic->pstParticipant = pstParticipant;

        /* ����������뵽Participant�µ��������� */
        LIST_INSERT_HEAD(pstParticipant->pstTopic, pstTopic);

    }

    /* �����ڽ�������reader, �Ѵ��������� */
    if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData);
    }

    return TRUE;
}

