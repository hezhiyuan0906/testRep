#include "../Include/GlobalDefine.h"
#define HEART_TIME 100

static int init_num = 0;
BOOL g_RapdIOValid = FALSE;
BOOL g_ConfValid = FALSE;
/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_ParticipantFactory_init
-   ��������: ��Դ��ʼ��
-   ��    ��: ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Դ��ʼ��
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_ParticipantFactory_init()
{
    if (0 == init_num)
    {
        /* ��־ģ���ʼ�� */
        InitCommLog();

        /* �����б��ʼ�� */
        InitializeSubTasks();

#if defined(USE_STATIC_MALLOC) || defined(USE_STATIC_MALLOC_T)
		/* ��̬�ڴ��ʼ�� */
		InitMemBlock();
#endif

#ifdef RAPID_IO_CHANNEL
        if (0 != InitRioNetWork())
        {
            g_RapdIOValid = TRUE;
        }
#endif
    }

    init_num++;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: InitDomainParticipant
-   ��������: ��ʼ��participant
-   ��    ��: participantָ��
-   ��    ��: 
-   ȫ�ֱ���: 
-   ע    ��: ��ʼ��participant��Ա
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitDomainParticipant(DDS_DomainParticipant* pstDomainParticipant)
{
    if (NULL != pstDomainParticipant)
    {
        DDS_MUTEX_INIT(pstDomainParticipant->threadMutex);
        DDS_MUTEX_INIT(pstDomainParticipant->m_discWtMutex);
        DDS_MUTEX_INIT(pstDomainParticipant->m_discRdMutex);
        DDS_MUTEX_INIT(pstDomainParticipant->monMutex);
		DDS_MUTEX_INIT(pstDomainParticipant->sleepMutex);
        
        pstDomainParticipant->threadSignal = TRUE;

        /* ��ʼ��Ĭ��qos */
        InitialDefaultQos();

        /* ��ʼ������ʱ�� */
        HPTB_INIT(pstDomainParticipant->heartBeatMark);
        HPTB_BEGIN(pstDomainParticipant->heartBeatMark);

        HPTB_INIT(pstDomainParticipant->timeTest);
        HPTB_BEGIN(pstDomainParticipant->timeTest);
        
        pstDomainParticipant->stRTPSMsgHeader.protocolId = PROTOCOL_RTPS_LITTLE_ENDIAN;
        pstDomainParticipant->stRTPSMsgHeader.vendorId = VENDORID_UNKNOWN;
        pstDomainParticipant->stRTPSMsgHeader.protocolVersion = PROTOCOLVERSION;

		pstDomainParticipant->sleepTime = 1000;
		pstDomainParticipant->livelinessNum = 0;
		pstDomainParticipant->livelinessDuration.sec = 0;
		pstDomainParticipant->livelinessDuration.nanosec = 0;
		pstDomainParticipant->discMinDuration = pstDomainParticipant->livelinessDuration;
        pstDomainParticipant->pstDiscParticipant = NULL;
        pstDomainParticipant->pstTopic = NULL;
        pstDomainParticipant->pstMonitor = NULL;
        pstDomainParticipant->bLoopBack = TRUE;

		pstDomainParticipant->ipAddr = 0;
		pstDomainParticipant->processId = 0;
        pstDomainParticipant->netMask = 0;

		pstDomainParticipant->pstParticipantQos = DDS_PARTICIPANT_QOS_DEFAULT;

        pstDomainParticipant->rtpsParticipantProxy.defaultUnicastLocator = NULL;
        pstDomainParticipant->rtpsParticipantProxy.defaultMulticastLocator = NULL;
        pstDomainParticipant->rtpsParticipantProxy.metatrafficMulticastLocator = NULL;
        pstDomainParticipant->rtpsParticipantProxy.metatrafficUnicastLocator = NULL;

        pstDomainParticipant->dcpsParticipantData.key.m_value[0] = 0L;
        pstDomainParticipant->dcpsParticipantData.key.m_value[1] = 0L;
        pstDomainParticipant->dcpsParticipantData.key.m_value[2] = 0L;
        pstDomainParticipant->dcpsParticipantData.key.m_value[3] = 0L;

		pstDomainParticipant->dcpsParticipantData.userData.value.length = 0;
		SetPropertyList(&pstDomainParticipant->dcpsParticipantData.propertyList);

        pstDomainParticipant->rtpsParticipantProxy.availableBuiltinEndpoints =
            DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER  |
            DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR   |
            DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER  |
            DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR   |
            DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER |
            DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR;

        UINT32 uiNum;
        /* �ڽ���д����ʼ�� */
        for (uiNum = 0; uiNum < 3; uiNum++)
        {
            pstDomainParticipant->stBuiltinDataWriter[uiNum].guid.prefix = pstDomainParticipant->stRTPSMsgHeader.guidPrefix;
            pstDomainParticipant->stBuiltinDataReader[uiNum].guid.prefix = pstDomainParticipant->stRTPSMsgHeader.guidPrefix;
            InitBuiltinDataWriter(&pstDomainParticipant->stBuiltinDataWriter[uiNum]);
            InitBuiltinDataReader(&pstDomainParticipant->stBuiltinDataReader[uiNum]);
        }

        pstDomainParticipant->stBuiltinDataWriter[0].guid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
        pstDomainParticipant->stBuiltinDataReader[0].guid.entityId = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
        pstDomainParticipant->stBuiltinDataWriter[0].pstParticipant = pstDomainParticipant;
        pstDomainParticipant->stBuiltinDataReader[0].pstParticipant = pstDomainParticipant;

        pstDomainParticipant->stBuiltinDataWriter[1].guid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
        pstDomainParticipant->stBuiltinDataReader[1].guid.entityId = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
        pstDomainParticipant->stBuiltinDataWriter[1].pstParticipant = pstDomainParticipant;
        pstDomainParticipant->stBuiltinDataReader[1].pstParticipant = pstDomainParticipant;

        pstDomainParticipant->stBuiltinDataWriter[2].guid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER;
        pstDomainParticipant->stBuiltinDataReader[2].guid.entityId = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
        pstDomainParticipant->stBuiltinDataWriter[2].pstParticipant = pstDomainParticipant;
        pstDomainParticipant->stBuiltinDataReader[2].pstParticipant = pstDomainParticipant;
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipantFactory_create_participant
-   ��������: ����participant
-   ��    ��: domainId��participantd��qos
-   ��    ��: participantָ��
-   ȫ�ֱ���: g_domainParticipant
-   ע    ��: ����participant
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DomainParticipant* DDS_DomainParticipantFactory_create_participant(USHORT domainId, DDS_DomainParticipantQos* pstDomPartQos)
{
#ifdef QYDDS_LIVELINESS_QOS
	if (pstDomPartQos != NULL)
	{
		if (pstDomPartQos->liveliness.kind != AUTOMATIC_LIVELINESS_QOS && pstDomPartQos->liveliness.lease_duration.sec == 0 && pstDomPartQos->liveliness.lease_duration.nanosec == 0)
		{
			printf("LIVELINESS lease_duration Error!\n");
			return NULL;
		}
	}
#endif
	if (domainId > 255)
	{
		printf("Error domainId, legal range: [0,255]!\n");
		return NULL;
	}

    MemoryBlock stMemBlock;
	if (pstDomPartQos != NULL)
	{
		if (strcmp(pstDomPartQos->property.ipAddr, DDS_PARTICIPANT_QOS_DEFAULT.property.ipAddr)!=0)
		{
			strncpy(g_ipAddr, pstDomPartQos->property.ipAddr, strlen(pstDomPartQos->property.ipAddr));
		}
	}

    /* �����ڴ洴��Participant */
    DDS_DomainParticipant*  pstDomainParticipant = (DDS_DomainParticipant*)DDS_STATIC_MALLOC(sizeof(DDS_DomainParticipant));
    if (NULL == pstDomainParticipant)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant: Failed to create Participant, Insufficient memory space.\n");
        return NULL;
    }

    /* ���뾲̬�ڴ洴��HistoryData����ڵ� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        DDS_STATIC_FREE(pstDomainParticipant);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* ���뾲̬�ڴ洴����ʷ�������� */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* ʧ���ͷ�ǰ��������ڴ� */
        DDS_STATIC_FREE(pstDomainParticipant);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* ��ʼ��Participant */
    InitDomainParticipant(pstDomainParticipant);

    pstDomainParticipant->processId = getProcessId();

    /* �����ʼ�� */
    if (!InitNetWorkStructure(pstDomainParticipant, domainId))
    {
		DDS_STATIC_FREE(pstHistoryData->data);
		DDS_STATIC_FREE(pstHistoryData);
		DDS_STATIC_FREE(pstDomainParticipant);
        PrintLog(COMMLOG_ERROR, "DomainParticipant: Failed to initialize NetWork!\n");
        return NULL;
    }
    /* �����ֽ����ʼ�������淢������ */
    INITIAL_MEMORYBLOCK(stMemBlock, pstHistoryData->data, NETWORK_BYTE_MAX_SIZE);

    pstDomainParticipant->dcpsParticipantData.domainID = domainId;

    /* ÿ����һ�����ز����ߣ��ڽ�seqNum�Լ�1*/
    pstDomainParticipant->stBuiltinDataWriter[0].seqNum = Add_SequenceNumber(pstDomainParticipant->stBuiltinDataWriter[0].seqNum, SEQUENCENUMBER_START);

    /* ������seqNum��� */
    pstHistoryData->seqNum = pstDomainParticipant->stBuiltinDataWriter[0].seqNum;
#ifdef QYDDS_LIVELINESS_QOS
	if (pstDomPartQos != NULL)
	{
		pstDomainParticipant->pstParticipantQos.liveliness = pstDomPartQos->liveliness;
		pstDomainParticipant->livelinessDuration = pstDomPartQos->liveliness.lease_duration;
	}
#endif

    /* ���л�spdp���� */
    ParticipantConvertNetworkByteOrder(pstDomainParticipant, &stMemBlock);

    /* ���ĳ��Ȳ��� */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* ���뵽�ڽ�����д�������� */
    InsertHistoryCacheTail(&pstDomainParticipant->stBuiltinDataWriter[0].stHistoryCache, pstHistoryData);

// #ifdef RAPID_IO_CHANNEL
//     HANDLE rioHandle = CreateThread(NULL, 0, (VOID*)&NetworkTransceiverTask, (VOID*)pstDomainParticipant, 0, NULL);
// #else
// 	NetworkSendReceiveTask(pstDomainParticipant);
// #endif
    NetworkSendReceiveTask(pstDomainParticipant);
    return pstDomainParticipant;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_ignore_participant
-   ��������: ��ֹparticipant���ػػ�
-   ��    ��: pstDomainParticipan
-   ��    ��: ��
-   ȫ�ֱ���:
-   ע    ��: ��ֹparticipant���ػػ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_DomainParticipant_not_ignore_participant(DDS_DomainParticipant* pstDomainParticipant)
{
    pstDomainParticipant->bLoopBack = TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_ignore_participant
-   ��������: ��ֹparticipant���ػػ�
-   ��    ��: pstDomainParticipan
-   ��    ��: ��
-   ȫ�ֱ���:
-   ע    ��: ��ֹparticipant���ػػ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_DomainParticipant_ignore_participant(DDS_DomainParticipant* pstDomainParticipant)
{
    pstDomainParticipant->bLoopBack = FALSE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipantFactory_delete_participant
-   ��������: ɾ��participant
-   ��    ��: pstDomainParticipan
-   ��    ��: ������
-   ȫ�ֱ���: 
-   ע    ��: ɾ��participant
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/



DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_delete_participant(DDS_DomainParticipant* pstDomainParticipant)
{
    UINT32 uiBuiltinNun = 0;

    if (NULL != pstDomainParticipant)
    {
        DDS_MUTEX_LOCK(pstDomainParticipant->threadMutex);

        TerminateThread(pstDomainParticipant->hThread[0], 0);
        TerminateThread(pstDomainParticipant->hThread[1], 0);
        TerminateThread(pstDomainParticipant->hThread[2], 0);
        TerminateThread(pstDomainParticipant->hThread[3], 0);
        TerminateThread(pstDomainParticipant->hThread[4], 0);
#ifdef QYDDS_LIVELINESS_QOS
		TerminateThread(pstDomainParticipant->hThread[5], 0);
#endif

        pstDomainParticipant->threadSignal = FALSE;

        /* ���г�Աָ������ */
        Locator_t* pstTempNode = NULL;
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.metatrafficUnicastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.metatrafficMulticastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.defaultUnicastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.defaultMulticastLocator, pstTempNode);

        /* ����Զ�˲����� */
        DDS_DiscoveredParticipant* pstDiscParticipant = pstDomainParticipant->pstDiscParticipant;
        while (NULL != pstDiscParticipant)
        {
            pstDomainParticipant->pstDiscParticipant = pstDiscParticipant->pNext;
            DDS_DiscoveredParticipant_Uninit(pstDiscParticipant);
            DDS_STATIC_FREE(pstDiscParticipant);
            pstDiscParticipant = pstDomainParticipant->pstDiscParticipant;
        }

        /* ������������ */
        DDS_Topic* pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstDomainParticipant->pstTopic = pstTopic->pNext;
            DDS_Topic_Uninit(pstTopic);
            DDS_STATIC_FREE(pstTopic);
            pstTopic = pstDomainParticipant->pstTopic;
        }

        /* �����ڽ�ʵ�� */
        for (uiBuiltinNun = 0; uiBuiltinNun < 3; uiBuiltinNun++)
        {
            /* �ڽ�д������� */
            UnInitBuiltinDataWriter(&pstDomainParticipant->stBuiltinDataWriter[uiBuiltinNun]);

            /* �ڽ��Ķ������ */
            UnInitBuiltinDataReader(&pstDomainParticipant->stBuiltinDataReader[uiBuiltinNun]);
        }

        DDS_MUTEX_UNLOCK(pstDomainParticipant->threadMutex);

        //�ȴ�һ�ٺ���������
        Sleep(100);
        DDS_STATIC_FREE(pstDomainParticipant);
    }

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_get_default_topic_qos
-   ��������: ��ȡ�������Ĭ��qos
-   ��    ��: participant������qos
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ͨ��participant��ȡһ��Ĭ������qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_get_defaul_participant_qos(DDS_DomainParticipantQos* pstDomPartQos)
{
    if (NULL == pstDomPartQos)
    {
        PrintLog(COMMLOG_ERROR, "ParticipantFactory: please pass in a non-null pointer of participant qos.\n");
        return DDS_RETCODE_ERROR;
    }

    *pstDomPartQos = DDS_PARTICIPANT_QOS_DEFAULT;
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_get_default_topic_qos
-   ��������: ��ȡ�������Ĭ��qos
-   ��    ��: participant������qos
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ͨ��participant��ȡһ��Ĭ������qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_DomainParticipantQos* DDS_DomainParticipantFactory_get_defaul_participant_qos_cs()
{
    DDS_DomainParticipantQos* pstDomPartQos = (DDS_DomainParticipantQos*)DDS_STATIC_MALLOC(sizeof(DDS_DomainParticipantQos));
    if (NULL == pstDomPartQos)
    {
        PrintLog(COMMLOG_ERROR, "ParticipantFactory: please pass in a non-null pointer of participant qos.\n");
        return NULL;
    }

    *pstDomPartQos = DDS_PARTICIPANT_QOS_DEFAULT;

    return pstDomPartQos;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_Participant_bind_network
-   ��������: ������߰�����
-   ��    ��: ������qos
-   ��    ��: ��
-   ȫ�ֱ���:
-   ע    ��: ������Ip
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2020\03\17       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_Participant_bind_network(DDS_DomainParticipantQos* pstDomPartQos, const CHAR* pcIpAddr)
{
	strncpy(pstDomPartQos->property.ipAddr, pcIpAddr, strlen(pcIpAddr));
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_create_topic
-   ��������: ��������
-   ��    ��: participant�����������������͡�����qos
-   ��    ��: ������Ϣ��ָ��
-   ȫ�ֱ���: 
-   ע    ��: ͨ��participant�������⣬������������򷵻ؿ�ָ�룬���������½����Ⲣ��ӵ�participant�µ�����������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_Topic*  DDS_DomainParticipant_create_topic(DDS_DomainParticipant* pstParticipant, const char* pcTopicName, const char* pcTopicType, DDS_TopicQos* pstTopicQos)
{
    DDS_Topic* pstTopic = pstParticipant->pstTopic;
    while (NULL != pstTopic)
    {
        if (!strcmp(pstTopic->topicName.value, pcTopicName))
        {
            /* ������Զ���Ѵ��������⣬����һ�£�ֱ�ӷ��ظ����� */
            if (Guid_Is_Equal(&pstTopic->tpGuid, &GUID_UNKNOWN) && !strcmp(pstTopic->topicType.value, pcTopicType))
            {
                /* ���ɸ����Ȿ��GUID������ͨ��GUID���ж��Ǳ��ػ���Զ������ */
                GenGuid(&pstTopic->tpGuid, &pstParticipant->stRTPSMsgHeader.guidPrefix, (ENTITYKIND_TOPIC << 24) | AUTO_ENTITYID);
                return pstTopic;
            }
			else if (!strcmp(pstTopic->topicType.value, pcTopicType))
            {
#ifdef EVO_DDS
				printf("Warning: Created a completely same topic, topicName: %s\n", pcTopicName);
				pstTopic->topicNum++;
				return pstTopic;
#endif
				printf("ERROR: Can't created a completely same topic, topicName: %s\n", pcTopicName);
                return NULL;
            }
        }
        pstTopic = pstTopic->pNext;
    }

    /* �����ڴ洴������ */
    pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
    if (NULL == pstTopic)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant_create_topic: Failed to create topic, Insufficient memory space.\n");
        return NULL;
    }

    /* ��ʼ������ */
    DDS_Topic_Init(pstTopic);

    /* ���游�ڵ�ָ��  */
    pstTopic->pstParticipant = pstParticipant;

    if (NULL != pstTopicQos)
    {
        /* ��������qos */
        pstTopic->stTopicQos = *pstTopicQos;
    }
    else
    {
        /* Ĭ������ */
        pstTopic->stTopicQos = DDS_TOPIC_QOS_DEFAULT;
    }

	/* ��������GUID */
	GenGuid(&pstTopic->tpGuid, &pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_TOPIC);
	
	/* ������������ */
	pstTopic->topicName.length = (UINT32)(strlen(pcTopicName) + 1);
	strcpy_s(pstTopic->topicName.value, pstTopic->topicName.length, pcTopicName);

    /* ������������ */
    pstTopic->topicType.length = (UINT32)(strlen(pcTopicType) + 1);
    strcpy_s(pstTopic->topicType.value, pstTopic->topicType.length, pcTopicType);

    /* ��������뵽Participant�µ��������� */
    LIST_INSERT_HEAD(pstParticipant->pstTopic, pstTopic);

    PrintLog(COMMLOG_INFO, "DomainParticipant_create_topic: create a topic successfully | topic name: %s.\n", pcTopicName);

    return pstTopic;
}

DDS_DLL DDS_Topic*  DDS_DomainParticipant_get_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType)
{
	DDS_Topic* pstTopic = pstParticipant->pstTopic;
	while (NULL != pstTopic)
	{
		if (!strcmp(pstTopic->topicName.value, pcTopicName) && !strcmp(pstTopic->topicType.value, pcTopicType))
		{
#ifdef EVO_DDS
			pstTopic->topicNum++;
#endif
			return pstTopic;
		}
		pstTopic = pstTopic->pNext;
	}
	return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_get_topic_num
-   ��������: ��ȡ�������
-   ��    ��: participant
-   ��    ��: �������
-   ȫ�ֱ���:
-   ע    ��: ��ȡ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2020\07\16       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL UINT32  DDS_DomainParticipant_get_topic_num(DDS_DomainParticipant* pstParticipant)
{
    UINT32 uiTopicNum = 0;

	DDS_Topic* pstTopic = pstParticipant->pstTopic;
	while (NULL != pstTopic)
	{
		if (!Guid_Is_Equal(&pstTopic->tpGuid, &GUID_UNKNOWN))
		{
			uiTopicNum++;
		}

        pstTopic = pstTopic->pNext;
    }

    return uiTopicNum;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_delete_topic
-   ��������: ɾ��ָ���������µ�����
-   ��    ��: participant������
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ɾ��ָ���������µ�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic(DDS_DomainParticipant* pstParticipant, DDS_Topic* pstTopic)
{
    if (pstParticipant == NULL || pstTopic == NULL)
    {
        return DDS_RETCODE_ERROR;
    }
#ifdef EVO_DDS
    if (pstTopic->topicNum > 1)
    {
        pstTopic->topicNum--;
        return DDS_RETCODE_OK;
    }
#endif
    //printf("delete topic\n");
    
    DDS_ReturnCode_t retCode;

    if (pstParticipant && pstTopic)
    {
        DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
        while (NULL != pstTempTopic)
        {
            if (pstTempTopic == pstTopic)
            {

                retCode = DDS_Topic_delete_datawriter(pstTempTopic);
                if (DDS_RETCODE_OK != retCode)
                {
                    return retCode;
                }

                retCode = DDS_Topic_delete_datareader(pstTempTopic);
                if (DDS_RETCODE_OK != retCode)
                {
                    return retCode;
                }

                pstTempTopic->tpGuid = GUID_UNKNOWN;
                break;
            }
            pstTempTopic = pstTempTopic->pNext;
        }
    }
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_delete_topic_by_name
-   ��������: ͨ����������ɾ������
-   ��    ��: participant��������
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ͨ����������ɾ������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType)
{
	DDS_ReturnCode_t retCode;

	if (pstParticipant && pcTopicName)
	{
		DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
		while (NULL != pstTempTopic)
		{
			if (!strcmp(pstTempTopic->topicName.value, pcTopicName) && !strcmp(pstTempTopic->topicType.value, pcTopicType))
			{
#ifdef EVO_DDS
				if (pstTempTopic->topicNum > 1)
				{
					pstTempTopic->topicNum--;
					return DDS_RETCODE_OK;
				}
#endif
				retCode = DDS_Topic_delete_datawriter(pstTempTopic);
				if (DDS_RETCODE_OK != retCode)
				{
					return retCode;
				}

				retCode = DDS_Topic_delete_datareader(pstTempTopic);
				if (DDS_RETCODE_OK != retCode)
				{
					return retCode;
				}

				pstTempTopic->tpGuid = GUID_UNKNOWN;

				break;
			}
			pstTempTopic = pstTempTopic->pNext;
		}
	}

	return DDS_RETCODE_OK;
}
/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_get_default_topic_qos
-   ��������: ��ȡ����Ĭ��qos
-   ��    ��: participant������qos
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ͨ��participant��ȡһ��Ĭ������qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DomainParticipant_get_default_topic_qos(DDS_DomainParticipant* pstParticipant, DDS_TopicQos* pstTopicQos)
{
    if (NULL == pstTopicQos)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant_get_default_topic_qos: please pass in a non-null pointer of topic qos.\n");
        return DDS_RETCODE_ERROR;
    }

    *pstTopicQos = DDS_TOPIC_QOS_DEFAULT;
    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DomainParticipant_get_default_topic_qos
-   ��������: ��ȡ����Ĭ��qos
-   ��    ��: participant������qos
-   ��    ��: ������
-   ȫ�ֱ���:
-   ע    ��: ͨ��participant��ȡһ��Ĭ������qos
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_TopicQos* DDS_DomainParticipant_get_default_topic_qos_cs(DDS_DomainParticipant* pstParticipant)
{
    DDS_TopicQos* pstTopicQos = (DDS_TopicQos*)DDS_STATIC_MALLOC(sizeof(DDS_TopicQos));
    if (NULL == pstTopicQos)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant_get_default_topic_qos: please pass in a non-null pointer of topic qos.\n");
        return NULL;
    }

    *pstTopicQos = DDS_TOPIC_QOS_DEFAULT;

    return pstTopicQos;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetBuiltinWriterFromParticipantByEntityId
-   ��������: �����ڽ�д����
-   ��    ��: Participant��EntityId
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participant��ͨ��EntityId������Ӧ�ڽ�д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BuiltinDataWriter* GetBuiltinWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    UINT32 i = 0;

    for (i = 0; i < 3; i++)
    {
        if (EntityId_Is_Equal(&pstDomainParticipant->stBuiltinDataWriter[i].guid.entityId, pstEntityId))
        {
            return &pstDomainParticipant->stBuiltinDataWriter[i];
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetBuiltinReaderFromParticipantByEntityId
-   ��������: �����ڽ��Ķ���
-   ��    ��: Participant��EntityId
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participant��ͨ��EntityId������Ӧ�ڽ��Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BuiltinDataReader* GetBuiltinReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    UINT32 i = 0;
    BuiltinDiscWriter* pstBuiltinDisWriter = NULL;
    for (i = 0; i < 3; i++)
    {
        pstBuiltinDisWriter = pstDomainParticipant->stBuiltinDataReader[i].pstBuiltinDataWriter;
        while (NULL != pstBuiltinDisWriter)
        {
            if (EntityId_Is_Equal(&pstBuiltinDisWriter->guid.entityId, pstEntityId))
            {
                return &pstDomainParticipant->stBuiltinDataReader[i];
            }

            pstBuiltinDisWriter = pstBuiltinDisWriter->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetDiscoveredWriterFromParticipantByEntityId
-   ��������: ����д����
-   ��    ��: Participant��Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participant��ͨ��Guid������ӦԶ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredWriter* GetDiscoveredWriterFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DiscoveredWriter* pstDiscWriter = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* ����ÿһ������ */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            /* ���������µ�ÿ��Զ��д���� */
            pstDiscWriter = GetDiscoveredWriterFromTopicByGuid(pstTopic, pstGuid);
            if (NULL != pstDiscWriter)
            {
                return pstDiscWriter;
            }

            pstTopic = pstTopic->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetDiscoveredReaderFromParticipantByEntityId
-   ��������: ����Զ����
-   ��    ��: Participant��Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participantͨ��Guid������ӦԶ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DiscoveredReader* pstDiscReader = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* ����ÿһ������ */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            /* ���������µ�ÿ��Զ���Ķ��� */
            pstDiscReader = GetDiscoveredReaderFromTopicByGuid(pstTopic, pstGuid);
            if (NULL != pstDiscReader)
            {
                return pstDiscReader;
            }

            pstTopic = pstTopic->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetLocalWriterFromParticipantByEntityId
-   ��������: ���ұ���д����
-   ��    ��: Participant��EntityId
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participantͨ��EntityId������Ӧд����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DataWriter* GetLocalWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DataWriter* pstLocalWriter = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* ����ÿһ������ */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstLocalWriter = pstTopic->pstLocalWriter;
            while (NULL != pstLocalWriter)
            {
                if (EntityId_Is_Equal(&pstLocalWriter->guid.entityId, pstEntityId))
                {
                    return pstLocalWriter;
                }

                pstLocalWriter = pstLocalWriter->pNext;
            }

            pstTopic = pstTopic->pNext;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: GetLocalWriterFromParticipantByEntityId
-   ��������: �����Ķ���
-   ��    ��: Participant��EntityId
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��Participant��ͨ��EntityId������Ӧд����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DataReader* GetLocalReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DataReader* pstLocalReader = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* ����ÿһ������ */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstLocalReader = pstTopic->pstLocalReader;
            while (NULL != pstLocalReader)
            {
                /* ���������µ�ÿ�������Ķ��� */
                if (EntityId_Is_Equal(&pstLocalReader->guid.entityId, pstEntityId))
                {
                    return pstLocalReader;
                }
                pstLocalReader = pstLocalReader->pNext;
            }

            pstTopic = pstTopic->pNext;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleAckNackMsg
-   ��������: ����Ack����
-   ��    ��: ack��Ϣ���ڴ�顢Participant
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������Ack����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\9       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleBuiltinAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant)
{
    /* ���Ҹ�EntityId���ڵı���д���� */
    BuiltinDataWriter* pstBuiltinDataWriter = GetBuiltinWriterFromParticipantByEntityId(pstParticipant, &pstAckNackMsg->writerId);
    if (NULL == pstBuiltinDataWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinAckNackMsg: Not find local data writer by entityId.\n");
        return FALSE;
    }

    /* ����ͬһ������Զ���Ķ���*/
    BuiltinDiscReader* pstBuiltinDiscReader = GetBuiltinDiscReaderFromBuiltinDataWriterByPrefix(pstBuiltinDataWriter, &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscReader)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinAckNackMsg: Not find remote data reader by entityId.\n");
        return FALSE;
    }

    /* ÿ�յ�һ��ack���ģ�������������գ�����յĻ���ÿ����һ����������һ�Σ���һ����Ŀ���ж�û��ack�ظ����Զ��Ͽ���Զ���Ķ�����ɾ���� */
    pstBuiltinDiscReader->uiSendHeartBeatNum = 0;
	DDS_DiscoveredParticipant* pstDiscParticipant = pstParticipant->pstDiscParticipant;
	while (pstDiscParticipant != NULL)
	{
		if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
		{
			DDS_MUTEX_LOCK(pstDiscParticipant->threadMutex);
			pstDiscParticipant->livelinessHeartMissNum = 0;
			pstDiscParticipant->lastHeartBeat = GetNowTime();
			DDS_MUTEX_UNLOCK(pstDiscParticipant->threadMutex);
			break;
		}
		pstDiscParticipant = pstDiscParticipant->pNext;
	}
	
	
    /* ����ack���ģ�ֱ������ */
    //if (pstAckNackMsg->count <= pstBuiltinDiscReader->stAckNack.count)
    //{
    //    return TRUE;
    //}
    
    /* ����ack������� */
    pstBuiltinDiscReader->stAckNack = *pstAckNackMsg;

    /* ��ʧ��Ƭ�ش� */
    RecoverBuiltinWriterMissSeqNum(pstBuiltinDataWriter, pstBuiltinDiscReader, pstAckNackMsg);

    /* �������ã�ackΪ���һ�ζԴ˲��ٷ����� */
    if(!pstSubMsgHeader->flags[1])
    {
        JointHeartbeatMsgSendAssignBuiltinReader(&pstBuiltinDataWriter->pstParticipant->stRTPSMsgHeader,pstBuiltinDataWriter, pstBuiltinDiscReader);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleAckNackMsg
-   ��������: ����Ack����
-   ��    ��: ack��Ϣ���ڴ�顢Participant
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��������Ack����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\9       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscRdGuid;

    /* ��ȡԶ��д����guid */
    stDiscRdGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscRdGuid.entityId = pstAckNackMsg->readerId;

    /* ���Ҹ�EntityId���ڵı���д���� */
    DDS_DataWriter* pstDataWriter = GetLocalWriterFromParticipantByEntityId(pstParticipant, &pstAckNackMsg->writerId);
    if (NULL == pstDataWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Not find local data writer by entityId.\n");
        return FALSE;
    }

    /* ����д����Ϊ��״̬����ֱ�ӷ��أ��������ack���� */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleAckNackMsg: Local reader is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* ����ͬһ������Զ���Ķ���*/
    DDS_DiscoveredReader* pstDiscReader = GetDiscoveredReaderFromTopicByGuid(pstDataWriter->pstTopic, &stDiscRdGuid);
    if (NULL == pstDiscReader)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Not find remote data reader by entityId.\n");
        return FALSE;
    }

    DDS_LocalWriter* pstLocalWriter = GetDiscoveredReaderLocalWriterByGuid(pstDiscReader, &pstDataWriter->guid);
    if (NULL == pstLocalWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Not find local data writer by entityId.\n");
        return FALSE;
    }

    /*  Զ���Ķ���Ϊ��״̬����ֱ�ӷ��أ��������ack���� */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Remote writer is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* ÿ�յ�һ��ack���ģ�������������գ�����յĻ���ÿ����һ����������һ�Σ���һ����Ŀ���ж�û��ack�ظ����Զ��Ͽ���Զ���Ķ�����ɾ���� */
    pstLocalWriter->uiSendHeartBeatNum = 0;

    /* ����ack���ģ�ֱ������ */
    //if (pstAckNackMsg->count <= pstLocalWriter->stAckNack.count)
    //{
    //    return TRUE;
    //}

    /* ����ack������� */
    pstLocalWriter->stAckNack.count = pstAckNackMsg->count;

    /* ����ÿ��Զ���Ķ����������¸�seqNum���Ա㱾�ػ������ */
    pstLocalWriter->stAckNack.readerSNState.base = pstAckNackMsg->readerSNState.base;

	/* ���ڻ������ */
	if (pstDataWriter->stDataWriterQos.durability.kind != TRANSIENT_DURABILITY_QOS)
	{
		//DelDataWriterHistoryCache(pstDataWriter);
	}

    /* ��ʧ��Ƭ�ش� */
    RecoverWriterMissSeqNum(pstDataWriter, pstDiscReader, pstAckNackMsg);

    /* ack����������ȱʧ���ش����������������Ա����ݿ����ش� */
    if (!pstSubMsgHeader->flags[1])
    {
        JointHeartbeatMsgSendAssignReader(pstDataWriter, pstDiscReader);
    }

    return TRUE;
}

/*--------------------------------------------------------------------------------
-   �� �� ��: HandleBuiltinHeartbeatMsg
-   ��������: �ڽ�����������
-   ��    ��: �ӱ�����Ϣͷ���������ݡ�Participant
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: �ڽ���������.�Ƿ���Ҫ�����ش�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleBuiltinHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant)
{
    /* ��Ч���������账�� */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstHeartbeatMsg->firstSN))
    {
        return TRUE;
    }
    /* ���ұ����ڽ��Ķ��� */
    BuiltinDataReader* pstBuiltinDataReader = GetBuiltinReaderFromParticipantByEntityId(pstParticipant, &pstHeartbeatMsg->writerId);
    if (NULL == pstBuiltinDataReader)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinHeartbeatMsg: Cannot discovered local dataWriter.\n");
        return FALSE;
    }

    /* ���ұ����ڽ��Ķ�����Ӧ��Զ���ڽ�д���� */
    BuiltinDiscWriter* pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(pstBuiltinDataReader, &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinHeartbeatMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    /* �������ڣ����账�� */
    if (pstHeartbeatMsg->count <= pstBuiltinDiscWriter->stHeartbeat.count)
    {
        return TRUE;
    }
    else
    {
        /* ��������ͬ�� */
        pstBuiltinDiscWriter->stHeartbeat.count = pstHeartbeatMsg->count;
    }

    /* ����ȱʧ�ڽ����ݰ� */
    GetBuiltinReaderMissSeqNum(pstBuiltinDataReader, pstBuiltinDiscWriter, pstHeartbeatMsg);

    /* ���л�ack���ķ��� */
    JointAckNackMsgSendAssignBuiltinWriter(pstBuiltinDataReader, pstBuiltinDiscWriter);

	//Liveliness��������
	DDS_DiscoveredParticipant* pstDiscParticipant = pstParticipant->pstDiscParticipant;
	while (pstDiscParticipant != NULL)
	{
		if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstRTPSMsgHeader->guidPrefix))
		{
			DDS_MUTEX_LOCK(pstDiscParticipant->threadMutex);
			pstDiscParticipant->livelinessHeartMissNum = 0;
			pstDiscParticipant->lastHeartBeat = GetNowTime();
			DDS_MUTEX_UNLOCK(pstDiscParticipant->threadMutex);
			break;
		}
		pstDiscParticipant = pstDiscParticipant->pNext;
	}

    return TRUE;
}

/*--------------------------------------------------------------------------------
-   �� �� ��: HandleHeartbeatMsg
-   ��������: ����������
-   ��    ��: �ӱ�����Ϣͷ���������ݡ�Participant
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ��������.�Ƿ���Ҫ�����ش�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;
    /* ��Ч���������账�� */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstHeartbeatMsg->firstSN))
    {
        return TRUE;
    }

    /* ��ȡԶ��д����guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstHeartbeatMsg->writerId;

    /* ���Ҹ��û����ݶ�Ӧ��Զ��д���� */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    /* �������ڣ����账�� */
    if (pstHeartbeatMsg->count <= pstDiscWriter->stHeartbeat.count)
    {
        return TRUE;
    }
    else
    {
        pstDiscWriter->stHeartbeat.count = pstHeartbeatMsg->count;
    }

    /* û��ָ���Ķ���Id��������Ϊͬһ�����µ��Ķ��� */
    if (EntityId_Is_Equal(&ENTITYID_UNKNOWN, &pstHeartbeatMsg->readerId))
    {
        pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
        if (NULL == pstDataReader)
        {
            PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Cannot discovered local dataReader.\n");
            return FALSE;
        }
    }
    else
    {
        pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
        while (NULL != pstDataReader)
        {
            /* ָ�����Ķ���id����ҪУ���Ƿ�һ�� */
            if (EntityId_Is_Equal(&pstDataReader->guid.entityId, &pstHeartbeatMsg->readerId))
            {
                break;
            }
            pstDataReader = pstDataReader->pNext;
        }

        if (NULL == pstDataReader)
        {
            PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Cannot discovered local dataReader.\n");
            return FALSE;
        }
    }

    /* �����Ķ���Ϊ��״̬����ֱ�ӷ��أ���������������� */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Local reader is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /*  Զ��д����Ϊ��״̬����ֱ�ӷ��أ���������������� */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Remote writer is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* ����ֵ����С��seqNum��С����������Ϊ�����ж�, ���ͷ������������������ֵ����Ϊ��СseqNum����ʧ������� */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDiscWriter->stAckNack.readerSNState.base, pstHeartbeatMsg->firstSN))
    {
        DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN(pstDataReader, pstDiscWriter, pstHeartbeatMsg->firstSN);
        PrintLog(COMMLOG_WARN, "HandleHeartbeatMsg: Beause of network interruption, the data maybe missed.\n");
    }

    /* ����ȱʧ���ύȱʧ��ţ��Զ˻�ȡack���ĺ������ش���ack���Ķ���Ҳû��ϵ���ȴ��´�������У�� */
    GetDataReaderMissSeqNum(pstDataReader, pstDiscWriter, pstHeartbeatMsg->lastSN);

    /* ���л�ack���ķ��� */
    JointAckNackMsgSendAssignWriter(pstDataReader, pstDiscWriter);

    return FALSE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: SendHeartbeatTasksOrder
-   ��������: ��ʱ��������
-   ��    ��: pstParticipant
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ���Ͷ�ʱ��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SendHeartbeatTasksOrder(DDS_DomainParticipant* pstParticipant)
{
    UINT32 uiNum;
    DDS_DataWriter*  pstLocalWriter = NULL;

    /* ����ʱ�� */
    HPTB_END(pstParticipant->heartBeatMark);

    /* �����������(us)*/
    ULONG ulHeartbeat = HPTB_INTERVAL_MICROSEC(pstParticipant->heartBeatMark);
    if (ulHeartbeat > 1 * HEART_TIME * 1000 * 1)
	{
		/* �ڽ�ʵ���������� */
		int init = 1;
#ifdef QYDDS_LIVELINESS_QOS
		init = 0;
#endif
		for (uiNum = init; uiNum < 3; uiNum++)
		{
			JointHeartbeatMsgSendAllBuiltinReader(&pstParticipant->stRTPSMsgHeader, &pstParticipant->stBuiltinDataWriter[uiNum]);
		}

		/* �û������������� */
		DDS_Topic* pstTopic = pstParticipant->pstTopic;
		while (NULL != pstTopic)
		{
			pstLocalWriter = pstTopic->pstLocalWriter;
			while (NULL != pstLocalWriter)
			{
				if (RELIABLE_RELIABILITY_QOS == pstLocalWriter->stDataWriterQos.reliability.kind)
				{
					JointHeartbeatMsgSendUnknownReader(pstLocalWriter);
				}

				pstLocalWriter = pstLocalWriter->pNext;
			}

			pstTopic = pstTopic->pNext;
		}

		HPTB_BEGIN(pstParticipant->heartBeatMark);
	}
}

/*---------------------------------------------------------------------------------
-   �� �� ��: NetworkTransceiverTask
-   ��������: �����շ�����
-   ��    ��: pstParticipant
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: �����շ�����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID NetworkTransceiverTask(DDS_DomainParticipant* pstParticipant)
{
	CHAR buffer[65536];
	INT length = 0;
	ULONG microSecInterval;
	MemoryBlock stMemBlock;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    while (1)
    {
        DDS_MUTEX_LOCK(pstParticipant->threadMutex);

        if (!pstParticipant->threadSignal)
        {
            DDS_MUTEX_UNLOCK(pstParticipant->threadMutex);
            break;
        }

        if (g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].active)
        {
            /* SPDP �������� */
            HPTB_END(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);
            microSecInterval = HPTB_INTERVAL_MICROSEC(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);

            if (microSecInterval > g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].miliSecPeriod * 1000)
            {
                /* �����ֽ����ʼ�������淢������ */
                INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
                JointSPDPDataMsg(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], &stMemBlock);
                SendMulticastMsg(&stMemBlock, &pstParticipant->socketList.spdp_data_sender, pstParticipant->rtpsParticipantProxy.metatrafficMulticastLocator);

                //���¿�ʼ��ʱ
                HPTB_BEGIN(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);
            }
        }

        /* SPDP �������� */
        if (g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.spdp_data_receiver, buffer);

            if (length > 0)
            {
                ParseAllNetworkOrder(buffer, length, pstParticipant);
            }
        }

        /* SEDP �������� */
        if (g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.sedp_data_receiver, buffer);

			if (length > 0)
			{
                ParseAllNetworkOrder(buffer, length, pstParticipant);
			}
		}

        /* USER �������� */
        if (g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.user_data_receiver, buffer);

			if (length > 0)
			{
                ParseAllNetworkOrder(buffer, length, pstParticipant);
			}
		}

        /* �������� */
        if (g_sub_task_ctrl_blocks[SUB_TASK_HEARTBEAT].active)
        {
            SendHeartbeatTasksOrder(pstParticipant);
        }

		if (length <= 0)
		{
			Sleep(1);
		}

		DDS_MUTEX_UNLOCK(pstParticipant->threadMutex);
	}

    return;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: SPDPSendTask
-   ��������: �����߷������ݷ�������
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �����߷������ݷ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SPDPSendTask(DDS_DomainParticipant* pstParticipant)
{
    MemoryBlock stMemBlock;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];
    /* �����ֽ����ʼ�������淢������ */
    while (TRUE)
    {
        INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
        JointSPDPDataMsg(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], &stMemBlock);
        SendMulticastMsg(&stMemBlock, &pstParticipant->socketList.spdp_data_sender, pstParticipant->rtpsParticipantProxy.metatrafficMulticastLocator);
        Sleep(1000);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: SEDPReceiveTask
-   ��������: �����߷������ݽ�������
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �����߷������ݽ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SPDPReceiveTask(DDS_DomainParticipant* pstParticipant)
{
    INT length;
    CHAR buffer[NETWORK_BYTE_MAX_SIZE];
    while (TRUE)
    {
        length = RTPSReceiveMsg(&pstParticipant->socketList.spdp_data_receiver, buffer);

        if (length > 0)
        {
            ParseAllNetworkOrder(buffer, length, pstParticipant);
        }

        Sleep(200);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: SEDPReceiveTask
-   ��������: �ڽ�ʵ�����ݽ�������
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �ڽ�ʵ�����ݽ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SEDPReceiveTask(DDS_DomainParticipant* pstParticipant)
{
    INT length;
    CHAR buffer[MAX_UDP_MSG_SIZE];
    while (TRUE)
    {
        length = RTPSReceiveMsg(&pstParticipant->socketList.sedp_data_receiver, buffer);

        if (length > 0)
        {
            ParseAllNetworkOrder(buffer, length, pstParticipant);
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: USERReceiveTask
-   ��������: �û����ݽ�������
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �û����ݽ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID USERReceiveTask(DDS_DomainParticipant* pstParticipant)
{
    INT length;
    CHAR buffer[MAX_UDP_MSG_SIZE];
    while (TRUE)
    {
        length = RTPSReceiveMsg(&pstParticipant->socketList.user_data_receiver, buffer);

        if (length > 0)
        {
            ParseAllNetworkOrder(buffer, length, pstParticipant);
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HEARTSendTask
-   ��������: ������������
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID HEARTSendTask(DDS_DomainParticipant* pstParticipant)
{
    UINT32 uiNum;
    DDS_DataWriter*  pstLocalWriter = NULL;
    int sleepTime = HEART_TIME;
#ifdef QYDDS_LIVELINESS_QOS
	if (pstParticipant->pstParticipantQos.liveliness.kind == MANUAL_BY_PARTICIPANT_LIVELINESS_QOS)
	{
		sleepTime = pstParticipant->pstParticipantQos.liveliness.lease_duration.sec * 1000;
		sleepTime += pstParticipant->pstParticipantQos.liveliness.lease_duration.nanosec / 1000 / 1000;
	}
	else
	{
		sleepTime = 100;
	}
#endif
    while (TRUE)
    {
		int init = 1;
#ifdef QYDDS_LIVELINESS_QOS
		init = 0;
#endif
        /* �ڽ�ʵ���������� */
		for (uiNum = init; uiNum < 3; uiNum++)
        {
            JointHeartbeatMsgSendAllBuiltinReader(&pstParticipant->stRTPSMsgHeader, &pstParticipant->stBuiltinDataWriter[uiNum]);
        }

        /* �û������������� */
        DDS_Topic* pstTopic = pstParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstLocalWriter = pstTopic->pstLocalWriter;
            while (NULL != pstLocalWriter)
            {
                if (RELIABLE_RELIABILITY_QOS == pstLocalWriter->stDataWriterQos.reliability.kind)
                {
                    JointHeartbeatMsgSendUnknownReader(pstLocalWriter);
                }
                pstLocalWriter = pstLocalWriter->pNext;
            }

            pstTopic = pstTopic->pNext;
        }
		Sleep(sleepTime);
    }
}

VOID DiscHEARTCheckTask(DDS_DomainParticipant* pstParticipant)
{
	UINT32 uiNum;
	Sleep(1000);
	while (TRUE)
	{
		if (pstParticipant->livelinessNum == 0)
		{
			Sleep(HEART_TIME);
			continue;
		}
		DDS_DiscoveredParticipant* pstDiscParticipant = pstParticipant->pstDiscParticipant;
		while (NULL != pstDiscParticipant)
		{
			if (Compare_Duration(pstParticipant->discMinDuration, pstDiscParticipant->leaseDuration) == TRUE && (pstDiscParticipant->leaseDuration.sec != 0 || pstDiscParticipant->leaseDuration.nanosec != 0))
			{
				DDS_MUTEX_LOCK(pstParticipant->sleepMutex);
				pstParticipant->sleepTime = pstDiscParticipant->leaseDuration.sec * 1000;
				pstParticipant->sleepTime += pstDiscParticipant->leaseDuration.nanosec / 1000 / 1000;
				pstParticipant->discMinDuration = pstDiscParticipant->leaseDuration;
				DDS_MUTEX_UNLOCK(pstParticipant->sleepMutex);
			}
			Duration_t nowTime = GetNowTime();
			DDS_MUTEX_LOCK(pstDiscParticipant->threadMutex);
			//printf("nowTime: %d:%d, lastTime: %d:%d\n", nowTime.sec, nowTime.nanosec, pstDiscParticipant->lastHeartBeat.sec, pstDiscParticipant->lastHeartBeat.nanosec);
			nowTime = Sub_Duration(nowTime, pstDiscParticipant->lastHeartBeat);
			//printf("HeartusedTime: %d, %d\n", nowTime.sec, nowTime.nanosec);
			if (Compare_Duration(nowTime, pstDiscParticipant->leaseDuration) == TRUE && (pstDiscParticipant->leaseDuration.nanosec != 0 || pstDiscParticipant->leaseDuration.sec != 0))
			{
				pstDiscParticipant->livelinessHeartMissNum++;
				//printf("livelinessNum: %d, duration: %d, sleepTime: %d\n", pstDiscParticipant->livelinessNum, pstDiscParticipant->leaseDuration.nanosec,pstParticipant->sleepTime);
			}
			if (pstDiscParticipant->livelinessHeartMissNum > 3)
			{
				DDS_DiscoveredParticipant* delDiscParticipant = pstDiscParticipant;
				DDS_MUTEX_UNLOCK(pstDiscParticipant->threadMutex);
				pstDiscParticipant = pstDiscParticipant->pNext;
				
				int k[4];
				UINT32 addr = delDiscParticipant->IPAddr;

				k[0] = addr & 0xFF;
				addr >>= 8;

				k[1] = addr & 0xFF;
				addr >>= 8;

				k[2] = addr & 0xFF;
				addr >>= 8;

				k[3] = addr & 0xFF;

				printf("The remote participant disconnect! IPAddr: %d.%d.%d.%d, ProcessID: %d.\n", k[0], k[1], k[2], k[3], delDiscParticipant->processID);
				DeleteALLDiscoveredFromByGuidPrefix(pstParticipant, delDiscParticipant->stRTPSMsgHeader.guidPrefix);
				pstParticipant->sleepTime = 1000;
				pstParticipant->discMinDuration.sec = 1;
				pstParticipant->discMinDuration.nanosec = 0;
				pstParticipant->livelinessNum--;
				continue;
			}
            else
            {
                DDS_MUTEX_UNLOCK(pstDiscParticipant->threadMutex);
            }
			pstDiscParticipant = pstDiscParticipant->pNext;
			
		}
		Sleep(pstParticipant->sleepTime);
	}
}

/*---------------------------------------------------------------------------------
-   �� �� ��: NetworkSendReceiveTask
-   ��������: ���������շ�
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ���������շ�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID NetworkSendReceiveTask(DDS_DomainParticipant* pstParticipant)
{
    /* SPDP���ͱ��� */
    pstParticipant->hThread[0] = CreateThread(NULL, 0, (VOID*)&SPDPSendTask, (VOID*)pstParticipant, 0, NULL);
    /* SPDP���ձ��� */
    pstParticipant->hThread[1] = CreateThread(NULL, 0, (VOID*)&SPDPReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* SEDP���ձ��� */
    pstParticipant->hThread[2] = CreateThread(NULL, 0, (VOID*)&SEDPReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* USER���ձ��� */
    pstParticipant->hThread[3] = CreateThread(NULL, 0, (VOID*)&USERReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* �������ͱ��� */
    pstParticipant->hThread[4] = CreateThread(NULL, 0, (VOID*)&HEARTSendTask, (VOID*)pstParticipant, 0, NULL);
#ifdef QYDDS_LIVELINESS_QOS
	/*Զ��������ⱨ��*/
	pstParticipant->hThread[5] = CreateThread(NULL, 0, (VOID*)&DiscHEARTCheckTask, (VOID*)pstParticipant, 0, NULL);
#endif
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix
-   ��������: ɾ��Զ���ڽ�������
-   ��    ��: ���⡢Guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ɾ��ָ��Զ���ڽ�������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t*  pstPrefix)
{
    DDS_DiscoveredParticipant*  pstPrevDiscParticipant = NULL;
    DDS_DiscoveredParticipant*  pstDiscParticipant = pstParticipant->pstDiscParticipant;

    while (NULL != pstDiscParticipant)
    {
        if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, pstPrefix))
        {
            if (!pstPrevDiscParticipant)
            {
                pstDiscParticipant = pstDiscParticipant->pNext;
                DDS_DiscoveredParticipant_Uninit(pstParticipant->pstDiscParticipant);
                DDS_STATIC_FREE(pstParticipant->pstDiscParticipant);
                pstParticipant->pstDiscParticipant = pstDiscParticipant;
            }
            else
            {
                pstPrevDiscParticipant->pNext = pstDiscParticipant->pNext;
                DDS_DiscoveredParticipant_Uninit(pstDiscParticipant);
                DDS_STATIC_FREE(pstDiscParticipant);
                pstDiscParticipant = pstPrevDiscParticipant->pNext;
            }
        }
        else
        {
            pstPrevDiscParticipant = pstDiscParticipant;
            pstDiscParticipant = pstDiscParticipant->pNext;
        }
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DeleteALLDiscoveredFromByGuidPrefix
-   ��������: ɾ��Զ������������Ϣ
-   ��    ��: �����ߡ�pstPrefix
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ɾ��Զ������������Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteALLDiscoveredFromByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t stPrefix)
{
    UINT32 uiBuiltinNun = 0;

    for (uiBuiltinNun = 0; uiBuiltinNun < 3; uiBuiltinNun++)
    {
        /* Զ���ڽ�д������� */
        DeleteBuiltinDiscReaderFromBuiltinWriterByGuidPrefix(&pstParticipant->stBuiltinDataWriter[uiBuiltinNun], &stPrefix);

        /* Զ���ڽ��Ķ������ */
        DeleteBuiltinDiscWriterFromBuiltinReaderByGuidPrefix(&pstParticipant->stBuiltinDataReader[uiBuiltinNun], &stPrefix);
    }

    DDS_Topic* pstTopic = pstParticipant->pstTopic;

    while (NULL != pstTopic)
    {
        /* Զ��д������� */
		DDS_MUTEX_LOCK(pstParticipant->m_discWtMutex);
		DeleteDiscWriterFromTopicByGuidPrefix(pstTopic, &stPrefix);
		DDS_MUTEX_UNLOCK(pstParticipant->m_discWtMutex);

        /* Զ���Ķ������ */
        DDS_MUTEX_LOCK(pstParticipant->m_discRdMutex);
        DeleteDiscReaderFromTopicByGuidPrefix(pstTopic, &stPrefix);
        DDS_MUTEX_UNLOCK(pstParticipant->m_discRdMutex);

        pstTopic = pstTopic->pNext;
    }

    /* ����ڽ�Զ�˲����� */
    DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix(pstParticipant, &stPrefix);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: UserMsgSendMonitor
-   ��������: ɾ��Զ������������Ϣ
-   ��    ��: �����ߡ�pstPrefix
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ɾ��Զ������������Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UserMsgSendMonitor(DDS_DomainParticipant* pstParticipant, GUID_t* tpGuid, MemoryBlock* pstMemBlock, UINT8 type)
{
    DDS_TopicMon* pstTopicMon = NULL;
    DDS_Monitor* pstPreMonitor = NULL;
    DDS_Monitor* pstMonitor = pstParticipant->pstMonitor;
    while (NULL != pstMonitor)
    {
        /*  ����3�������ؽڵ� */
        if (time(NULL) - pstMonitor->deadtime > 3)
        {
            if (NULL == pstPreMonitor)
            {
                pstParticipant->pstMonitor = pstMonitor->pNext;
                /* ������ؽڵ� */
                UninitMonitor(pstMonitor);
                DDS_STATIC_FREE(pstMonitor);
                pstMonitor = pstParticipant->pstMonitor;
            }
            else
            {
                pstPreMonitor->pNext = pstMonitor->pNext;
                /* ������ؽڵ� */
                UninitMonitor(pstMonitor);
                DDS_STATIC_FREE(pstMonitor);
                pstMonitor = pstPreMonitor->pNext;
            }
        }
        else
        {
            pstTopicMon = pstMonitor->pstTopicMon;
            while (NULL != pstTopicMon)
            {
                if (Guid_Is_Equal(&pstTopicMon->tpGuid, tpGuid))
                {
                    /* ������Ϣ��־λ */
                    pstMemBlock->base[6] = type;
                    /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
                    RTPSSendMsg(pstMemBlock, &pstParticipant->socketList.user_data_sender, pstMonitor->unicastLocator);
                }

                pstTopicMon = pstTopicMon->pNext;
            }

			pstPreMonitor = pstMonitor;
            pstMonitor = pstMonitor->pNext;
        }
    }

}