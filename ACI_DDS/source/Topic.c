#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_Init
-   ��������: ��ʼ����
-   ��    ��: ����ָ��
-   ��    ��:
-   ȫ�ֱ���: 
-   ע    ��: ��ʼ����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
void DDS_Topic_Init(DDS_Topic* pstTopic)
{
    if (NULL != pstTopic)
    {
        InitHistoryCache(&pstTopic->stUserData);
        pstTopic->pstLocalWriter  = NULL;
        pstTopic->pstLocalReader  = NULL;
        pstTopic->pstDiscWriter = NULL;
        pstTopic->pstDiscReader = NULL;
        pstTopic->pNext               = NULL;
        pstTopic->tpGuid              = GUID_UNKNOWN;
#ifdef EVO_DDS
        pstTopic->topicNum = 1;
#endif
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_Uninit
-   ��������: ��������
-   ��    ��: ����ָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
void DDS_Topic_Uninit(DDS_Topic* pstTopic)
{
    if (NULL != pstTopic)
    {
        UninitHistoryCache(&pstTopic->stUserData);

        /* ��������д���� */
        DDS_DataWriter*  pstDataWriter = pstTopic->pstLocalWriter;
        while (NULL != pstDataWriter)
        {
            pstTopic->pstLocalWriter = pstDataWriter->pNext;

            DDS_DataWriter_Uninit(pstDataWriter);
            DDS_STATIC_FREE(pstDataWriter);

            pstDataWriter = pstTopic->pstLocalWriter;
        }

        /* ���������Ķ��� */
        DDS_DataReader*  pstDataReader = pstTopic->pstLocalReader;
        while (NULL != pstDataReader)
        {
            pstTopic->pstLocalReader = pstDataReader->pNext;

            DDS_DataReader_Uninit(pstDataReader);
            DDS_STATIC_FREE(pstDataReader);

            pstDataReader = pstTopic->pstLocalReader;
        }

        /* ����Զ��д���� */
        DDS_DiscoveredWriter*  pstDiscWriter = pstTopic->pstDiscWriter;
        while (NULL != pstDiscWriter)
        {
            pstTopic->pstDiscWriter = pstDiscWriter->pNext;

            DDS_DiscoveredWriter_Uninit(pstDiscWriter);
            DDS_STATIC_FREE(pstDiscWriter);

            pstDiscWriter = pstTopic->pstDiscWriter;
        }

        /* ����Զ���Ķ��� */
        DDS_DiscoveredReader*  pstDiscReader = pstTopic->pstDiscReader;
        while (NULL != pstDiscReader)
        {
            pstTopic->pstDiscReader = pstDiscReader->pNext;
            DDS_DiscoveredReader_Uninit(pstDiscReader);
            DDS_STATIC_FREE(pstDiscReader);

            pstDiscReader = pstTopic->pstDiscReader;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_default_datawriter_qos
-   ��������: ��ȡĬ��д����qos
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��ȡĬ��д����qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datawriter_qos(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos)
{
    if (NULL == pstWriterQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return DDS_RETCODE_ERROR;
    }

    *pstWriterQos = DDS_DATAWRITER_QOS_DEFAULT;
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_default_datawriter_qos
-   ��������: ��ȡĬ��д����qos
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��ȡĬ��д����qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DataWriterQos* DDS_Topic_get_default_datawriter_qos_cs(DDS_Topic* pstTopic)
{
    DDS_DataWriterQos* pstWriterQos = (DDS_DataWriterQos*)DDS_STATIC_MALLOC(sizeof(DDS_DataWriterQos));
    if (NULL == pstWriterQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return NULL;
    }

    *pstWriterQos = DDS_DATAWRITER_QOS_DEFAULT;

    return pstWriterQos;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_default_datawriter_qos
-   ��������: ��ȡĬ��д����qos
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��ȡĬ���Ķ���qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datareader_qos(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos)
{
    if (NULL == pstReaderQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return DDS_RETCODE_ERROR;
    }

    *pstReaderQos = DDS_DATAREADER_QOS_DEFAULT;
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_datawriter_qos_set_reliability
-   ��������: д�������ÿɿ���
-   ��    ��: д�������ɿ���
-   ��    ��: ����ֵ
-   ȫ�ֱ���:
-   ע    ��: д�������ÿɿ���qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_datawriter_qos_set_reliability(DDS_DataWriterQos* pstWriterQos, ReliabilityQosPolicyKind kind)
{
    if (NULL == pstWriterQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return DDS_RETCODE_ERROR;
    }

    pstWriterQos->reliability.kind = kind;

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_datareader_qos_set_reliability
-   ��������: �Ķ������ÿɿ���
-   ��    ��: �Ķ������ɿ���
-   ��    ��: ����ֵ
-   ȫ�ֱ���:
-   ע    ��: �Ķ������ÿɿ���qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_datareader_qos_set_reliability(DDS_DataReaderQos* pstReaderQos, ReliabilityQosPolicyKind kind)
{
    if (NULL == pstReaderQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return DDS_RETCODE_ERROR;
    }

    pstReaderQos->reliability.kind = kind;

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_default_datawriter_qos
-   ��������: ��ȡĬ��д����qos
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��ȡĬ���Ķ���qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DataReaderQos* DDS_Topic_get_default_datareader_qos_cs(DDS_Topic* pstTopic)
{
    DDS_DataReaderQos* pstReaderQos = (DDS_DataReaderQos*)DDS_STATIC_MALLOC(sizeof(DDS_DataReaderQos));
    if (NULL == pstReaderQos)
    {
        PrintLog(COMMLOG_ERROR, "topic: please pass in a non-null pointer of datawriter qos.\n");
        return NULL;
    }

    *pstReaderQos = DDS_DATAREADER_QOS_DEFAULT;

    return pstReaderQos;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_create_datawriter
-   ��������: ��������д����
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��������д����������������Ϣ�����Ķ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DataWriter* DDS_Topic_create_datawriter(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos, CALLBACKTIME callBackTime)
{
    MemoryBlock stMemBlock;
    DDS_DataWriter* pstTempDataWriter = NULL;
    DDS_DiscoveredReader*  pstDiscReader = NULL;

    DDS_DataWriter* pstDataWriter = (DDS_DataWriter*)DDS_STATIC_MALLOC(sizeof(DDS_DataWriter));
    if (NULL == pstDataWriter)
    {
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        DDS_STATIC_FREE(pstDataWriter);
        return NULL;
    }

    /* ���뾲̬�ڴ洴����ʷ�������� */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* ʧ���ͷ�ǰ��������ڴ� */
        DDS_STATIC_FREE(pstHistoryData);
        DDS_STATIC_FREE(pstDataWriter);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, pstHistoryData->data, NETWORK_BYTE_MAX_SIZE);

    if (NULL != pstWriterQos)
    {
		if (pstWriterQos->history.depth <= 0)
		{
			printf("HISTORY_QOS ERROR: depth must greater than 0! \n");
            DDS_STATIC_FREE(pstHistoryData->data);
			DDS_STATIC_FREE(pstHistoryData);
            DDS_STATIC_FREE(pstDataWriter);
			return NULL;
		}
        /* ���뱾��д����qos */
        pstDataWriter->stDataWriterQos = *pstWriterQos;
    }
    else
    {
        /* ���뱾��д����Ĭ��qos */
        pstDataWriter->stDataWriterQos = DDS_DATAWRITER_QOS_DEFAULT;
    }

    /* ��ʼ������д���� */
    DDS_DataWriter_Init(pstDataWriter);

    /* ���游�ڵ�,��������ϲ����� */
    pstDataWriter->pstTopic = pstTopic;

    /* ����ʱ��������ص� */
    pstDataWriter->writerCallBack = callBackTime;

	/* ����д����GUID */
	GenGuid(&pstDataWriter->guid, &pstTopic->pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_WRITER_NO_KEY);

    /* ÿ����һ������д�������ڽ�seqNum�Լ�1*/
    pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

    /* �����Ƿ�ȡ������ */
    pstHistoryData->bCancel = FALSE;

    /* �����¼seqNum��� */
    pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum;

    /* �����¼ʵ��GUID */
    pstHistoryData->stGuid = pstDataWriter->guid;

    /* ���л�д������Ϣ�� */
    DataWriterConvertNetworkByteOrder(pstTopic, pstDataWriter, &stMemBlock);

    /* ��¼���л����ĳ��� */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* ���뵽�ڽ�����д�������� */
    InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

    /* ��װ�������� */
    JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[1], NULL);

    /* Զ��reader��ӱ���writer */
    pstDiscReader = pstTopic->pstDiscReader;
    
    while (NULL != pstDiscReader)
    {
        DiscoveredReaderAddLocalWriter(pstDiscReader, &pstDataWriter->guid);
        pstDiscReader = pstDiscReader->pNext;
    }

    PrintLog(COMMLOG_INFO, "Topic: The dataWriter was created successfully.\n");

    LIST_INSERT_TAIL(pstTopic->pstLocalWriter, pstDataWriter, pstTempDataWriter);
#ifdef QYDDS_DEADLINE_QOS
	if (pstDataWriter->stDataWriterQos.deadline.period.sec != 0 || pstDataWriter->stDataWriterQos.deadline.period.nanosec != 0)
	{
		DeadLineSendTask(pstDataWriter);
	}
#endif

    //Sleep(1);
    return pstDataWriter;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_datawriter_num
-   ��������: ��ȡ�����±���д��������
-   ��    ��: ����ָ��
-   ��    ��: д��������
-   ȫ�ֱ���:
-   ע    ��: ��ȡ�����±���д��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\07\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL UINT32 DDS_Topic_get_datawriter_num(DDS_Topic* pstTopic)
{
    UINT32 uiWriterNum = 0;

    DDS_DataWriter* pstDataWriter = pstTopic->pstLocalWriter;
    while (NULL != pstDataWriter)
    {
        uiWriterNum++;

        pstDataWriter = pstDataWriter->pNext;
    }

    return uiWriterNum;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_create_datareader
-   ��������: ���������Ķ���
-   ��    ��: ����ָ�롢�Ķ���qosָ�롢�ص�����
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ��������д����������������Ϣ�����Ķ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DataReader* DDS_Topic_create_datareader(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos, CALLBACKFUN callBackFun)
{
    MemoryBlock stMemBlock;
    DDS_DataReader*  pstTempDataReader = NULL;

    DDS_DataReader*  pstDataReader = (DDS_DataReader*)DDS_STATIC_MALLOC(sizeof(DDS_DataReader));
    if (NULL == pstDataReader)
    {
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        DDS_STATIC_FREE(pstDataReader);
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* ���뾲̬�ڴ洴����ʷ�������� */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* ʧ���ͷ�ǰ��������ڴ� */
        DDS_STATIC_FREE(pstDataReader);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, pstHistoryData->data, NETWORK_BYTE_MAX_SIZE);

    if (NULL != pstReaderQos)
    {
        /* ���뱾���Ķ���qos */
        pstDataReader->stDataReaderQos = *pstReaderQos;
    }
    else
    {
        /* ����Ĭ���Ķ���qos */
        pstDataReader->stDataReaderQos = DDS_DATAREADER_QOS_DEFAULT;
    }

    if (pstDataReader->stDataReaderQos.history.depth <= 0)
    {
        DDS_STATIC_FREE(pstDataReader);
        DDS_STATIC_FREE(pstHistoryData);
        printf("ReaderQoS error: history.depth must be larger than zero! \n");
        return NULL;
    }
    /* ����ص����� */
    pstDataReader->recvCallBack = callBackFun;

    /* ��ʼ�������Ķ��� */
    DDS_DataReader_Init(pstDataReader);

    /* ���游�ڵ� */
    pstDataReader->pstTopic = pstTopic;

	/* ����д����GUID */
	GenGuid(&pstDataReader->guid, &pstTopic->pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_READER_NO_KEY);

    /* ÿ����һ�������Ķ������ڽ�seqNum�Լ�1*/
    pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

    /* �����Ƿ�ȡ������ */
    pstHistoryData->bCancel = FALSE;

    /* �����¼seqNum��� */
    pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum;

    /* �����¼ʵ��GUID */
    pstHistoryData->stGuid = pstDataReader->guid;

    /* ���л��Ķ�����Ϣ�� */
    DataReaderConvertNetworkByteOrder(pstTopic, pstDataReader, &stMemBlock);

    /* ��¼���л����ĳ��� */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* ���뵽�ڽ�����д�������� */
    InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

    /* ��װ���ı��� */
    JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[2], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataReader was created successfully.\n");

    LIST_INSERT_TAIL(pstTopic->pstLocalReader, pstDataReader, pstTempDataReader);
    return pstDataReader;
}


/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_get_datareader_num
-   ��������: ��ȡ�����±����Ķ�������
-   ��    ��: ����ָ��
-   ��    ��: �Ķ�������
-   ȫ�ֱ���:
-   ע    ��: ��ȡ�����±����Ķ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\07\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL UINT32 DDS_Topic_get_datareader_num(DDS_Topic* pstTopic)
{
    UINT32 uiReaderNum = 0;

    DDS_DataReader* pstDataReader = pstTopic->pstLocalReader;
    while (NULL != pstDataReader)
    {
        uiReaderNum++;

        pstDataReader = pstDataReader->pNext;
    }

    return uiReaderNum;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_delete_datawriter
-   ��������: ����ɾ��д����
-   ��    ��: ����ָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ����ɾ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datawriter(DDS_Topic* pstTopic)
{
    /* ��������д���� */
    DDS_DataWriter*  pstDataWriter = pstTopic->pstLocalWriter;
    while (NULL != pstDataWriter)
    {
        /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
        HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
        if (NULL == pstHistoryData)
        {
            PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
            return DDS_RETCODE_ERROR;
        }

        /* ÿ����һ������д�������ڽ�seqNum�Լ�1*/
        pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

        /* ������������ */
        pstHistoryData->data = NULL;

        /* �����Ƿ�ȡ������ */
        pstHistoryData->bCancel = TRUE;

        /* �����¼seqNum��� */
        pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum;

        /* �����¼ʵ��GUID */
        pstHistoryData->stGuid = pstDataWriter->guid;

        /* ���뵽�ڽ�����д�������� */
        InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

        /* ��װ���ı��� */
        JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[1], NULL);

        PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

        /* ����д���� */
        
        DDS_DataWriter_Uninit(pstDataWriter);
        

        pstDataWriter = pstDataWriter->pNext;
    }

    /* �ڵ�ɾ�� */
    LIST_DELETE_ALL(pstTopic->pstLocalWriter, pstDataWriter);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_delete_datawriter
-   ��������: ����ɾ��д����
-   ��    ��: ����ָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ����ɾ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DataWriter_delete_from_topic(DDS_DataWriter*  pstDataWriter)
{
    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        return DDS_RETCODE_ERROR;
    }

    /* ÿ����һ������д�������ڽ�seqNum�Լ�1*/
    pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

    /* ������������ */
    pstHistoryData->data = NULL;

    /* �����Ƿ�ȡ������ */
    pstHistoryData->bCancel = TRUE;

    /* �����¼seqNum��� */
    pstHistoryData->seqNum = pstParticipant->stBuiltinDataWriter[1].seqNum;

    /* �����¼ʵ��GUID */
    pstHistoryData->stGuid = pstDataWriter->guid;

    /* ���뵽�ڽ�����д�������� */
    InsertHistoryCacheTail(&pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

    /* ��װ���ı��� */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstParticipant->stBuiltinDataWriter[1], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

    /* ����д���� */
    DDS_DataWriter_delete(pstDataWriter);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_delete_datawriter
-   ��������: ����ɾ���Ķ���
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ����ɾ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datareader(DDS_Topic* pstTopic)
{
    /* ���������Ķ��� */
    DDS_DataReader*  pstDataReader = pstTopic->pstLocalReader;

    while (NULL != pstDataReader)
    {
        /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
        HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
        if (NULL == pstHistoryData)
        {
            PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
            return DDS_RETCODE_ERROR;
        }

        /* ÿ����һ������д�������ڽ�seqNum�Լ�1*/
        pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

        /* ������������ */
        pstHistoryData->data = NULL;

        /* �����Ƿ�ȡ������ */
        pstHistoryData->bCancel = TRUE;

        /* �����¼seqNum��� */
        pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum;

        /* �����¼ʵ��GUID */
        pstHistoryData->stGuid = pstDataReader->guid;

        /* ���뵽�ڽ�����д�������� */
        InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

        /* ��װ���ı��� */
        JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[2], NULL);

        PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

        /* ����д���� */
        DDS_DataReader_Uninit(pstDataReader);

        pstDataReader = pstDataReader->pNext;
    }

    /* �ڵ�ɾ�� */
    LIST_DELETE_ALL(pstTopic->pstLocalReader, pstDataReader);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Topic_delete_datawriter
-   ��������: ����ɾ���Ķ���
-   ��    ��: ����ָ�롢д����qosָ��
-   ��    ��: д����ָ��
-   ȫ�ֱ���:
-   ע    ��: ����ɾ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DataReader_delete_from_topic(DDS_DataReader*  pstDataReader)
{
    DDS_DomainParticipant* pstParticipant =  pstDataReader->pstTopic->pstParticipant;

    /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
        return DDS_RETCODE_ERROR;
    }

    /* ÿ����һ������д�������ڽ�seqNum�Լ�1*/
    pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

    /* ������������ */
    pstHistoryData->data = NULL;

    /* �����Ƿ�ȡ������ */
    pstHistoryData->bCancel = TRUE;

    /* �����¼seqNum��� */
    pstHistoryData->seqNum = pstParticipant->stBuiltinDataWriter[2].seqNum;

    /* �����¼ʵ��GUID */
    pstHistoryData->stGuid = pstDataReader->guid;

    /* ���뵽�ڽ�����д�������� */
    InsertHistoryCacheTail(&pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

    /* ��װ���ı��� */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstParticipant->stBuiltinDataWriter[2], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

    /* ����д���� */
    DDS_DataReader_delete(pstDataReader);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetDiscoveredWriterFromTopicByEntityId
-   ��������: ����д����
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������ͨ��Guid������ӦԶ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredWriter* GetDiscoveredWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid)
{
    if (NULL != pstTopic)
    {
        /* ���������µ�ÿ��Զ��д���� */
        DDS_DiscoveredWriter* pstDiscWriter = pstTopic->pstDiscWriter;
        while (NULL != pstDiscWriter)
        {
            if (Guid_Is_Equal(&pstDiscWriter->guid, pstGuid))
            {
                return pstDiscWriter;
            }
            pstDiscWriter = pstDiscWriter->pNext;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetDiscoveredReaderFromTopicByEntityId
-   ��������: ����Զ����
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������ͨ��Guid������ӦԶ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid)
{
    if (NULL != pstTopic)
    {
        DDS_DiscoveredReader* pstTwinDiscReader = NULL;
        /* ���������µ�ÿ��Զ���Ķ��� */
        DDS_DiscoveredReader* pstDiscReader = pstTopic->pstDiscReader;
        while (NULL != pstDiscReader)
        {
            if (Guid_Is_Equal(&pstDiscReader->guid, pstGuid))
            {
                return pstDiscReader;
            }

            pstTwinDiscReader = pstDiscReader->pstTwinDiscReader;
            while (NULL != pstTwinDiscReader)
            {
                if (Guid_Is_Equal(&pstTwinDiscReader->guid, pstGuid))
                {
                    return pstTwinDiscReader;
                }

                pstTwinDiscReader = pstTwinDiscReader->pNext;
            }

            pstDiscReader = pstDiscReader->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetDiscoveredReaderFromTopicByEntityId
-   ��������: ����Զ����
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������ͨ��Guid������ӦԶ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByPrefix(DDS_Topic* pstTopic, GuidPrefix_t* pstPrefix)
{
    if (NULL != pstTopic)
    {
        /* ���������µ�ÿ��Զ���Ķ��� */
        DDS_DiscoveredReader* pstDiscReader = pstTopic->pstDiscReader;
        while (NULL != pstDiscReader)
        {
            if (GuidPrefix_Is_Equal(&pstDiscReader->guid.prefix, pstPrefix))
            {
                return pstDiscReader;
            }
            pstDiscReader = pstDiscReader->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteDiscWriterFromTopicByGuidPrefix
-   ��������: ɾ��Զ��д����
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteDiscWriterFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix)
{
    DDS_DiscoveredWriter* pstPrevDiscoveredWriter = NULL;
    DDS_DiscoveredWriter* pstDiscWriter = pstTopic->pstDiscWriter;

    while (NULL != pstDiscWriter)
    {
        if (GuidPrefix_Is_Equal(&pstDiscWriter->guid.prefix, pstPrefix))
        {
            if (!pstPrevDiscoveredWriter)
            {
                pstDiscWriter = pstDiscWriter->pNext;
                DDS_DiscoveredWriter_Uninit(pstTopic->pstDiscWriter);
                DDS_STATIC_FREE(pstTopic->pstDiscWriter);
                pstTopic->pstDiscWriter = pstDiscWriter;
            }
            else
            {
                pstPrevDiscoveredWriter->pNext = pstDiscWriter->pNext;
                DDS_DiscoveredWriter_Uninit(pstDiscWriter);
                DDS_STATIC_FREE(pstDiscWriter);
                pstDiscWriter = pstPrevDiscoveredWriter->pNext;
            }
        }
        else
        {
            pstPrevDiscoveredWriter = pstDiscWriter;
            pstDiscWriter = pstDiscWriter->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteDiscReaderFromTopicByGuidPrefix
-   ��������: ɾ��Զ���Ķ���
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteDiscReaderFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix)
{
    DDS_DiscoveredReader* pstPrevDiscoveredReader = NULL;
    DDS_DiscoveredReader* pstDiscReader = pstTopic->pstDiscReader;

    while (NULL != pstDiscReader)
    {
        if (GuidPrefix_Is_Equal(&pstDiscReader->guid.prefix, pstPrefix))
        {
            if (!pstPrevDiscoveredReader)
            {
                pstDiscReader = pstDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstTopic->pstDiscReader);
                DDS_STATIC_FREE(pstTopic->pstDiscReader);
                pstTopic->pstDiscReader = pstDiscReader;
            }
            else
            {
                pstPrevDiscoveredReader->pNext = pstDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstDiscReader);
                DDS_STATIC_FREE(pstDiscReader);
                pstDiscReader = pstPrevDiscoveredReader->pNext;
            }
        }
        else
        {
            pstPrevDiscoveredReader = pstDiscReader;
            pstDiscReader = pstDiscReader->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteDiscWriterFromTopicByGuid
-   ��������: ɾ��Զ��д����
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\19       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteDiscWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t*  pstGuid)
{
    DDS_DiscoveredWriter* pstPrevDiscoveredWriter = NULL;
    DDS_DiscoveredWriter* pstDiscWriter = pstTopic->pstDiscWriter;

    while (NULL != pstDiscWriter)
    {
        if (Guid_Is_Equal(&pstDiscWriter->guid, pstGuid))
        {
            if (!pstPrevDiscoveredWriter)
            {
                pstDiscWriter = pstDiscWriter->pNext;
                DDS_DiscoveredWriter_Uninit(pstTopic->pstDiscWriter);
                DDS_STATIC_FREE(pstTopic->pstDiscWriter);
                pstTopic->pstDiscWriter = pstDiscWriter;
            }
            else
            {
                pstPrevDiscoveredWriter->pNext = pstDiscWriter->pNext;
                DDS_DiscoveredWriter_Uninit(pstDiscWriter);
                DDS_STATIC_FREE(pstDiscWriter);
                pstDiscWriter = pstPrevDiscoveredWriter->pNext;
            }
        }
        else
        {
            pstPrevDiscoveredWriter = pstDiscWriter;
            pstDiscWriter = pstDiscWriter->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteDiscReaderFromTopicByGuidPrefix
-   ��������: ɾ��Զ���Ķ���
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\19       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteTwinDiscReaderFromTopicByGuid(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid)
{
    DDS_DiscoveredReader* pstPrevDiscoveredReader = NULL;
    DDS_DiscoveredReader* pstTwinDiscReader = pstDiscReader->pstTwinDiscReader;

    while (NULL != pstTwinDiscReader)
    {
        if (Guid_Is_Equal(&pstTwinDiscReader->guid, pstGuid))
        {
            if (!pstPrevDiscoveredReader)
            {
                pstTwinDiscReader = pstTwinDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstDiscReader->pstTwinDiscReader);
                DDS_STATIC_FREE(pstDiscReader->pstTwinDiscReader);
                pstDiscReader->pstTwinDiscReader = pstTwinDiscReader;
            }
            else
            {
                pstPrevDiscoveredReader->pNext = pstTwinDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstTwinDiscReader);
                DDS_STATIC_FREE(pstTwinDiscReader);
                pstTwinDiscReader = pstPrevDiscoveredReader->pNext;
            }
        }
        else
        {
            pstPrevDiscoveredReader = pstTwinDiscReader;
            pstTwinDiscReader = pstTwinDiscReader->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteDiscReaderFromTopicByGuidPrefix
-   ��������: ɾ��Զ���Ķ���
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\03\19       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteDiscReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid)
{
    DDS_DiscoveredReader* pstPrevDiscoveredReader = NULL;
    DDS_DiscoveredReader* pstDiscReader = pstTopic->pstDiscReader;

    while (NULL != pstDiscReader)
    {
        DeleteTwinDiscReaderFromTopicByGuid(pstDiscReader, pstGuid);

        if (Guid_Is_Equal(&pstDiscReader->guid, pstGuid))
        {
            if (!pstPrevDiscoveredReader)
            {
                if (NULL != pstDiscReader->pstTwinDiscReader)
                {
                    TwinDiscoveredReaderChangePosition(pstDiscReader);
                    return;
                }

                pstTopic->pstDiscReader = pstDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstDiscReader);
                DDS_STATIC_FREE(pstDiscReader);
                pstDiscReader = pstTopic->pstDiscReader;
                return;
                
            }
            else
            {
                if (NULL != pstDiscReader->pstTwinDiscReader)
                {
                    TwinDiscoveredReaderChangePosition(pstDiscReader);
                    return;
                }

                pstPrevDiscoveredReader->pNext = pstDiscReader->pNext;
                DDS_DiscoveredReader_Uninit(pstDiscReader);
                DDS_STATIC_FREE(pstDiscReader);
                pstDiscReader = pstPrevDiscoveredReader->pNext;
                return;
            }
        }
        else
        {
            pstPrevDiscoveredReader = pstDiscReader;
            pstDiscReader = pstDiscReader->pNext;
        }
    }
}
