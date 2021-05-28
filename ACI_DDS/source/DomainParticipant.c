#include "../Include/GlobalDefine.h"
#define HEART_TIME 100

static int init_num = 0;
BOOL g_RapdIOValid = FALSE;
BOOL g_ConfValid = FALSE;
/*---------------------------------------------------------------------------------
-   函 数 名: DDS_ParticipantFactory_init
-   功能描述: 资源初始化
-   输    入: 无
-   返    回:
-   全局变量:
-   注    释: 资源初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\23       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_ParticipantFactory_init()
{
    if (0 == init_num)
    {
        /* 日志模块初始化 */
        InitCommLog();

        /* 任务列表初始化 */
        InitializeSubTasks();

#if defined(USE_STATIC_MALLOC) || defined(USE_STATIC_MALLOC_T)
		/* 静态内存初始化 */
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
-   函 数 名: InitDomainParticipant
-   功能描述: 初始化participant
-   输    入: participant指针
-   返    回: 
-   全局变量: 
-   注    释: 初始化participant成员
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

        /* 初始化默认qos */
        InitialDefaultQos();

        /* 初始化心跳时标 */
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
        /* 内建读写器初始化 */
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
-   函 数 名: DDS_DomainParticipantFactory_create_participant
-   功能描述: 创建participant
-   输    入: domainId、participantd的qos
-   返    回: participant指针
-   全局变量: g_domainParticipant
-   注    释: 创建participant
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 申请内存创建Participant */
    DDS_DomainParticipant*  pstDomainParticipant = (DDS_DomainParticipant*)DDS_STATIC_MALLOC(sizeof(DDS_DomainParticipant));
    if (NULL == pstDomainParticipant)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant: Failed to create Participant, Insufficient memory space.\n");
        return NULL;
    }

    /* 申请静态内存创建HistoryData链表节点 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        DDS_STATIC_FREE(pstDomainParticipant);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* 申请静态内存创建历史缓存数据 */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* 失败释放前面申请的内存 */
        DDS_STATIC_FREE(pstDomainParticipant);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* 初始化Participant */
    InitDomainParticipant(pstDomainParticipant);

    pstDomainParticipant->processId = getProcessId();

    /* 网络初始化 */
    if (!InitNetWorkStructure(pstDomainParticipant, domainId))
    {
		DDS_STATIC_FREE(pstHistoryData->data);
		DDS_STATIC_FREE(pstHistoryData);
		DDS_STATIC_FREE(pstDomainParticipant);
        PrintLog(COMMLOG_ERROR, "DomainParticipant: Failed to initialize NetWork!\n");
        return NULL;
    }
    /* 网络字节序初始化，保存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, pstHistoryData->data, NETWORK_BYTE_MAX_SIZE);

    pstDomainParticipant->dcpsParticipantData.domainID = domainId;

    /* 每创建一个本地参与者，内建seqNum自加1*/
    pstDomainParticipant->stBuiltinDataWriter[0].seqNum = Add_SequenceNumber(pstDomainParticipant->stBuiltinDataWriter[0].seqNum, SEQUENCENUMBER_START);

    /* 参与者seqNum编号 */
    pstHistoryData->seqNum = pstDomainParticipant->stBuiltinDataWriter[0].seqNum;
#ifdef QYDDS_LIVELINESS_QOS
	if (pstDomPartQos != NULL)
	{
		pstDomainParticipant->pstParticipantQos.liveliness = pstDomPartQos->liveliness;
		pstDomainParticipant->livelinessDuration = pstDomPartQos->liveliness.lease_duration;
	}
#endif

    /* 序列化spdp报文 */
    ParticipantConvertNetworkByteOrder(pstDomainParticipant, &stMemBlock);

    /* 报文长度插入 */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* 插入到内建发布写入器缓存 */
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
-   函 数 名: DDS_DomainParticipant_ignore_participant
-   功能描述: 阻止participant本地回环
-   输    入: pstDomainParticipan
-   返    回: 无
-   全局变量:
-   注    释: 阻止participant本地回环
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_DomainParticipant_not_ignore_participant(DDS_DomainParticipant* pstDomainParticipant)
{
    pstDomainParticipant->bLoopBack = TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DomainParticipant_ignore_participant
-   功能描述: 阻止participant本地回环
-   输    入: pstDomainParticipan
-   返    回: 无
-   全局变量:
-   注    释: 阻止participant本地回环
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_DomainParticipant_ignore_participant(DDS_DomainParticipant* pstDomainParticipant)
{
    pstDomainParticipant->bLoopBack = FALSE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DomainParticipantFactory_delete_participant
-   功能描述: 删除participant
-   输    入: pstDomainParticipan
-   返    回: 返回码
-   全局变量: 
-   注    释: 删除participant
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

        /* 所有成员指针析构 */
        Locator_t* pstTempNode = NULL;
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.metatrafficUnicastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.metatrafficMulticastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.defaultUnicastLocator, pstTempNode);
		LIST_DELETE_ALL(pstDomainParticipant->rtpsParticipantProxy.defaultMulticastLocator, pstTempNode);

        /* 析构远端参与者 */
        DDS_DiscoveredParticipant* pstDiscParticipant = pstDomainParticipant->pstDiscParticipant;
        while (NULL != pstDiscParticipant)
        {
            pstDomainParticipant->pstDiscParticipant = pstDiscParticipant->pNext;
            DDS_DiscoveredParticipant_Uninit(pstDiscParticipant);
            DDS_STATIC_FREE(pstDiscParticipant);
            pstDiscParticipant = pstDomainParticipant->pstDiscParticipant;
        }

        /* 析构所有主题 */
        DDS_Topic* pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstDomainParticipant->pstTopic = pstTopic->pNext;
            DDS_Topic_Uninit(pstTopic);
            DDS_STATIC_FREE(pstTopic);
            pstTopic = pstDomainParticipant->pstTopic;
        }

        /* 析构内建实体 */
        for (uiBuiltinNun = 0; uiBuiltinNun < 3; uiBuiltinNun++)
        {
            /* 内建写入器清除 */
            UnInitBuiltinDataWriter(&pstDomainParticipant->stBuiltinDataWriter[uiBuiltinNun]);

            /* 内建阅读器清除 */
            UnInitBuiltinDataReader(&pstDomainParticipant->stBuiltinDataReader[uiBuiltinNun]);
        }

        DDS_MUTEX_UNLOCK(pstDomainParticipant->threadMutex);

        //等待一百毫秒再析构
        Sleep(100);
        DDS_STATIC_FREE(pstDomainParticipant);
    }

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DomainParticipant_get_default_topic_qos
-   功能描述: 获取域参与者默认qos
-   输    入: participant、主题qos
-   返    回: 质量码
-   全局变量:
-   注    释: 通过participant获取一个默认主题qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DomainParticipant_get_default_topic_qos
-   功能描述: 获取域参与者默认qos
-   输    入: participant、主题qos
-   返    回: 质量码
-   全局变量:
-   注    释: 通过participant获取一个默认主题qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Participant_bind_network
-   功能描述: 域参与者绑定网卡
-   输    入: 参与者qos
-   返    回: 无
-   全局变量:
-   注    释: 绑定网卡Ip
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2020\03\17       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_Participant_bind_network(DDS_DomainParticipantQos* pstDomPartQos, const CHAR* pcIpAddr)
{
	strncpy(pstDomPartQos->property.ipAddr, pcIpAddr, strlen(pcIpAddr));
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DomainParticipant_create_topic
-   功能描述: 创建主题
-   输    入: participant、主题名、主题类型、主题qos
-   返    回: 主题信息的指针
-   全局变量: 
-   注    释: 通过participant创建主题，若该主题存在则返回空指针，不存在则新建主题并添加到participant下的主题链表中
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
            /* 若发现远端已创建该主题，类型一致，直接返回该主题 */
            if (Guid_Is_Equal(&pstTopic->tpGuid, &GUID_UNKNOWN) && !strcmp(pstTopic->topicType.value, pcTopicType))
            {
                /* 生成该主题本地GUID，上面通过GUID来判断是本地还是远端主题 */
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

    /* 申请内存创建主题 */
    pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
    if (NULL == pstTopic)
    {
        PrintLog(COMMLOG_ERROR, "DomainParticipant_create_topic: Failed to create topic, Insufficient memory space.\n");
        return NULL;
    }

    /* 初始化主题 */
    DDS_Topic_Init(pstTopic);

    /* 保存父节点指针  */
    pstTopic->pstParticipant = pstParticipant;

    if (NULL != pstTopicQos)
    {
        /* 插入主题qos */
        pstTopic->stTopicQos = *pstTopicQos;
    }
    else
    {
        /* 默认主题 */
        pstTopic->stTopicQos = DDS_TOPIC_QOS_DEFAULT;
    }

	/* 生成主题GUID */
	GenGuid(&pstTopic->tpGuid, &pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_TOPIC);
	
	/* 插入主题名称 */
	pstTopic->topicName.length = (UINT32)(strlen(pcTopicName) + 1);
	strcpy_s(pstTopic->topicName.value, pstTopic->topicName.length, pcTopicName);

    /* 插入主题类型 */
    pstTopic->topicType.length = (UINT32)(strlen(pcTopicType) + 1);
    strcpy_s(pstTopic->topicType.value, pstTopic->topicType.length, pcTopicType);

    /* 该主题插入到Participant下的主题链表 */
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
-   函 数 名: DDS_DomainParticipant_get_topic_num
-   功能描述: 获取主题个数
-   输    入: participant
-   返    回: 主题个数
-   全局变量:
-   注    释: 获取主题个数
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DomainParticipant_delete_topic
-   功能描述: 删除指定参与者下的主题
-   输    入: participant、主题
-   返    回: 质量码
-   全局变量:
-   注    释: 删除指定参与者下的主题
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DomainParticipant_delete_topic_by_name
-   功能描述: 通过主题名称删除主题
-   输    入: participant、主题名
-   返    回: 质量码
-   全局变量:
-   注    释: 通过主题名称删除主题
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DomainParticipant_get_default_topic_qos
-   功能描述: 获取主题默认qos
-   输    入: participant、主题qos
-   返    回: 质量码
-   全局变量:
-   注    释: 通过participant获取一个默认主题qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DomainParticipant_get_default_topic_qos
-   功能描述: 获取主题默认qos
-   输    入: participant、主题qos
-   返    回: 质量码
-   全局变量:
-   注    释: 通过participant获取一个默认主题qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: GetBuiltinWriterFromParticipantByEntityId
-   功能描述: 查找内建写入器
-   输    入: Participant、EntityId
-   返    回:
-   全局变量:
-   注    释: 从Participant中通过EntityId查找相应内建写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: GetBuiltinReaderFromParticipantByEntityId
-   功能描述: 查找内建阅读器
-   输    入: Participant、EntityId
-   返    回:
-   全局变量:
-   注    释: 从Participant中通过EntityId查找相应内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: GetDiscoveredWriterFromParticipantByEntityId
-   功能描述: 查找写入器
-   输    入: Participant、Guid
-   返    回:
-   全局变量:
-   注    释: 从Participant中通过Guid查找相应远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredWriter* GetDiscoveredWriterFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DiscoveredWriter* pstDiscWriter = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* 遍历每一个主题 */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            /* 遍历主题下的每个远端写入器 */
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
-   函 数 名: GetDiscoveredReaderFromParticipantByEntityId
-   功能描述: 查找远端器
-   输    入: Participant、Guid
-   返    回:
-   全局变量:
-   注    释: 从Participant通过Guid查找相应远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DiscoveredReader* pstDiscReader = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* 遍历每一个主题 */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            /* 遍历主题下的每个远端阅读器 */
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
-   函 数 名: GetLocalWriterFromParticipantByEntityId
-   功能描述: 查找本地写入器
-   输    入: Participant、EntityId
-   返    回:
-   全局变量:
-   注    释: 从Participant通过EntityId查找相应写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DataWriter* GetLocalWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DataWriter* pstLocalWriter = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* 遍历每一个主题 */
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
-   函 数 名: GetLocalWriterFromParticipantByEntityId
-   功能描述: 查找阅读器
-   输    入: Participant、EntityId
-   返    回:
-   全局变量:
-   注    释: 从Participant中通过EntityId查找相应写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DataReader* GetLocalReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId)
{
    DDS_Topic* pstTopic = NULL;
    DDS_DataReader* pstLocalReader = NULL;

    if (NULL != pstDomainParticipant)
    {
        /* 遍历每一个主题 */
        pstTopic = pstDomainParticipant->pstTopic;
        while (NULL != pstTopic)
        {
            pstLocalReader = pstTopic->pstLocalReader;
            while (NULL != pstLocalReader)
            {
                /* 遍历主题下的每个本地阅读器 */
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
-   函 数 名: HandleAckNackMsg
-   功能描述: 处理Ack报文
-   输    入: ack消息、内存块、Participant
-   返    回:
-   全局变量:
-   注    释: 解析处理Ack报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\9       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleBuiltinAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant)
{
    /* 查找该EntityId所在的本地写入器 */
    BuiltinDataWriter* pstBuiltinDataWriter = GetBuiltinWriterFromParticipantByEntityId(pstParticipant, &pstAckNackMsg->writerId);
    if (NULL == pstBuiltinDataWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinAckNackMsg: Not find local data writer by entityId.\n");
        return FALSE;
    }

    /* 查找同一主题下远端阅读器*/
    BuiltinDiscReader* pstBuiltinDiscReader = GetBuiltinDiscReaderFromBuiltinDataWriterByPrefix(pstBuiltinDataWriter, &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscReader)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinAckNackMsg: Not find remote data reader by entityId.\n");
        return FALSE;
    }

    /* 每收到一次ack报文，心跳积累数清空，不清空的话，每发送一次心跳积累一次，到一定数目会判定没有ack回复，自动断开该远端阅读器（删除） */
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
	
	
    /* 过期ack报文，直接跳过 */
    //if (pstAckNackMsg->count <= pstBuiltinDiscReader->stAckNack.count)
    //{
    //    return TRUE;
    //}
    
    /* 更新ack报文序号 */
    pstBuiltinDiscReader->stAckNack = *pstAckNackMsg;

    /* 丢失分片重传 */
    RecoverBuiltinWriterMissSeqNum(pstBuiltinDataWriter, pstBuiltinDiscReader, pstAckNackMsg);

    /* 心跳设置，ack为最后一次对此不再发心跳 */
    if(!pstSubMsgHeader->flags[1])
    {
        JointHeartbeatMsgSendAssignBuiltinReader(&pstBuiltinDataWriter->pstParticipant->stRTPSMsgHeader,pstBuiltinDataWriter, pstBuiltinDiscReader);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleAckNackMsg
-   功能描述: 处理Ack报文
-   输    入: ack消息、内存块、Participant
-   返    回:
-   全局变量:
-   注    释: 解析处理Ack报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\9       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscRdGuid;

    /* 获取远端写入器guid */
    stDiscRdGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscRdGuid.entityId = pstAckNackMsg->readerId;

    /* 查找该EntityId所在的本地写入器 */
    DDS_DataWriter* pstDataWriter = GetLocalWriterFromParticipantByEntityId(pstParticipant, &pstAckNackMsg->writerId);
    if (NULL == pstDataWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Not find local data writer by entityId.\n");
        return FALSE;
    }

    /* 本地写入器为无状态，则直接返回，不处理该ack报文 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleAckNackMsg: Local reader is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* 查找同一主题下远端阅读器*/
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

    /*  远端阅读器为无状态，则直接返回，不处理该ack报文 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleAckNackMsg: Remote writer is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* 每收到一次ack报文，心跳积累数清空，不清空的话，每发送一次心跳积累一次，到一定数目会判定没有ack回复，自动断开该远端阅读器（删除） */
    pstLocalWriter->uiSendHeartBeatNum = 0;

    /* 过期ack报文，直接跳过 */
    //if (pstAckNackMsg->count <= pstLocalWriter->stAckNack.count)
    //{
    //    return TRUE;
    //}

    /* 更新ack报文序号 */
    pstLocalWriter->stAckNack.count = pstAckNackMsg->count;

    /* 更新每个远端阅读器期望的下个seqNum，以便本地缓存清除 */
    pstLocalWriter->stAckNack.readerSNState.base = pstAckNackMsg->readerSNState.base;

	/* 过期缓存清除 */
	if (pstDataWriter->stDataWriterQos.durability.kind != TRANSIENT_DURABILITY_QOS)
	{
		//DelDataWriterHistoryCache(pstDataWriter);
	}

    /* 丢失分片重传 */
    RecoverWriterMissSeqNum(pstDataWriter, pstDiscReader, pstAckNackMsg);

    /* ack报文有数据缺失，重传后立马补发心跳，以便数据快速重传 */
    if (!pstSubMsgHeader->flags[1])
    {
        JointHeartbeatMsgSendAssignReader(pstDataWriter, pstDiscReader);
    }

    return TRUE;
}

/*--------------------------------------------------------------------------------
-   函 数 名: HandleBuiltinHeartbeatMsg
-   功能描述: 内建心跳处理函数
-   输    入: 子报文消息头、心跳数据、Participant
-   返    回: 布尔
-   全局变量:
-   注    释: 内建心跳处理.是否需要数据重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleBuiltinHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant)
{
    /* 无效心跳，不予处理 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstHeartbeatMsg->firstSN))
    {
        return TRUE;
    }
    /* 查找本地内建阅读器 */
    BuiltinDataReader* pstBuiltinDataReader = GetBuiltinReaderFromParticipantByEntityId(pstParticipant, &pstHeartbeatMsg->writerId);
    if (NULL == pstBuiltinDataReader)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinHeartbeatMsg: Cannot discovered local dataWriter.\n");
        return FALSE;
    }

    /* 查找本地内建阅读器对应的远端内建写入器 */
    BuiltinDiscWriter* pstBuiltinDiscWriter = GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(pstBuiltinDataReader, &pstRTPSMsgHeader->guidPrefix);
    if (NULL == pstBuiltinDiscWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleBuiltinHeartbeatMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    /* 心跳过期，不予处理 */
    if (pstHeartbeatMsg->count <= pstBuiltinDiscWriter->stHeartbeat.count)
    {
        return TRUE;
    }
    else
    {
        /* 心跳计数同步 */
        pstBuiltinDiscWriter->stHeartbeat.count = pstHeartbeatMsg->count;
    }

    /* 查找缺失内建数据包 */
    GetBuiltinReaderMissSeqNum(pstBuiltinDataReader, pstBuiltinDiscWriter, pstHeartbeatMsg);

    /* 序列化ack报文发送 */
    JointAckNackMsgSendAssignBuiltinWriter(pstBuiltinDataReader, pstBuiltinDiscWriter);

	//Liveliness计数清零
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
-   函 数 名: HandleHeartbeatMsg
-   功能描述: 心跳处理函数
-   输    入: 子报文消息头、心跳数据、Participant
-   返    回: 布尔
-   全局变量:
-   注    释: 心跳处理.是否需要数据重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;
    /* 无效心跳，不予处理 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstHeartbeatMsg->firstSN))
    {
        return TRUE;
    }

    /* 获取远端写入器guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstHeartbeatMsg->writerId;

    /* 查找该用户数据对应的远端写入器 */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    /* 心跳过期，不予处理 */
    if (pstHeartbeatMsg->count <= pstDiscWriter->stHeartbeat.count)
    {
        return TRUE;
    }
    else
    {
        pstDiscWriter->stHeartbeat.count = pstHeartbeatMsg->count;
    }

    /* 没有指定阅读器Id，即关联为同一主题下的阅读器 */
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
            /* 指定了阅读器id，需要校验是否一致 */
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

    /* 本地阅读器为无状态，则直接返回，不处理该心跳报文 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Local reader is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /*  远端写入器为无状态，则直接返回，不处理该心跳报文 */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_ERROR, "HandleHeartbeatMsg: Remote writer is stateless, ignore heartbeat.\n");
        return FALSE;
    }

    /* 期望值比最小的seqNum还小，可能是因为网络中断, 发送方缓存进行了清理，期望值调整为最小seqNum，损失降到最低 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDiscWriter->stAckNack.readerSNState.base, pstHeartbeatMsg->firstSN))
    {
        DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN(pstDataReader, pstDiscWriter, pstHeartbeatMsg->firstSN);
        PrintLog(COMMLOG_WARN, "HandleHeartbeatMsg: Beause of network interruption, the data maybe missed.\n");
    }

    /* 数据缺失，提交缺失编号，对端获取ack报文后立马重传，ack报文丢了也没关系，等待下次心跳再校验 */
    GetDataReaderMissSeqNum(pstDataReader, pstDiscWriter, pstHeartbeatMsg->lastSN);

    /* 序列化ack报文发送 */
    JointAckNackMsgSendAssignWriter(pstDataReader, pstDiscWriter);

    return FALSE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: SendHeartbeatTasksOrder
-   功能描述: 定时心跳任务
-   输    入: pstParticipant
-   返    回: 布尔
-   全局变量:
-   注    释: 发送定时心跳任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SendHeartbeatTasksOrder(DDS_DomainParticipant* pstParticipant)
{
    UINT32 uiNum;
    DDS_DataWriter*  pstLocalWriter = NULL;

    /* 更新时间 */
    HPTB_END(pstParticipant->heartBeatMark);

    /* 计算心跳间隔(us)*/
    ULONG ulHeartbeat = HPTB_INTERVAL_MICROSEC(pstParticipant->heartBeatMark);
    if (ulHeartbeat > 1 * HEART_TIME * 1000 * 1)
	{
		/* 内建实体心跳发送 */
		int init = 1;
#ifdef QYDDS_LIVELINESS_QOS
		init = 0;
#endif
		for (uiNum = init; uiNum < 3; uiNum++)
		{
			JointHeartbeatMsgSendAllBuiltinReader(&pstParticipant->stRTPSMsgHeader, &pstParticipant->stBuiltinDataWriter[uiNum]);
		}

		/* 用户数据心跳发送 */
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
-   函 数 名: NetworkTransceiverTask
-   功能描述: 网络收发任务
-   输    入: pstParticipant
-   返    回: 布尔
-   全局变量:
-   注    释: 网络收发任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
            /* SPDP 发送任务 */
            HPTB_END(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);
            microSecInterval = HPTB_INTERVAL_MICROSEC(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);

            if (microSecInterval > g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].miliSecPeriod * 1000)
            {
                /* 网络字节序初始化，保存发布报文 */
                INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
                JointSPDPDataMsg(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], &stMemBlock);
                SendMulticastMsg(&stMemBlock, &pstParticipant->socketList.spdp_data_sender, pstParticipant->rtpsParticipantProxy.metatrafficMulticastLocator);

                //重新开始计时
                HPTB_BEGIN(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);
            }
        }

        /* SPDP 接收任务 */
        if (g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.spdp_data_receiver, buffer);

            if (length > 0)
            {
                ParseAllNetworkOrder(buffer, length, pstParticipant);
            }
        }

        /* SEDP 接收任务 */
        if (g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.sedp_data_receiver, buffer);

			if (length > 0)
			{
                ParseAllNetworkOrder(buffer, length, pstParticipant);
			}
		}

        /* USER 接收任务 */
        if (g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].active)
        {
            length = RTPSReceiveMsg(&pstParticipant->socketList.user_data_receiver, buffer);

			if (length > 0)
			{
                ParseAllNetworkOrder(buffer, length, pstParticipant);
			}
		}

        /* 心跳任务 */
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
-   函 数 名: SPDPSendTask
-   功能描述: 参与者发现数据发送任务
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 参与者发现数据发送任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID SPDPSendTask(DDS_DomainParticipant* pstParticipant)
{
    MemoryBlock stMemBlock;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];
    /* 网络字节序初始化，保存发布报文 */
    while (TRUE)
    {
        INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);
        JointSPDPDataMsg(&pstParticipant->stRTPSMsgHeader, pstParticipant->stBuiltinDataWriter[0].stHistoryCache.pstHead, &pstParticipant->stBuiltinDataWriter[0], &stMemBlock);
        SendMulticastMsg(&stMemBlock, &pstParticipant->socketList.spdp_data_sender, pstParticipant->rtpsParticipantProxy.metatrafficMulticastLocator);
        Sleep(1000);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: SEDPReceiveTask
-   功能描述: 参与者发现数据接收任务
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 参与者发现数据接收任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: SEDPReceiveTask
-   功能描述: 内建实体数据接收任务
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 内建实体数据接收任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: USERReceiveTask
-   功能描述: 用户数据接收任务
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 用户数据接收任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: HEARTSendTask
-   功能描述: 心跳发送任务
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 心跳发送任务
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
        /* 内建实体心跳发送 */
		for (uiNum = init; uiNum < 3; uiNum++)
        {
            JointHeartbeatMsgSendAllBuiltinReader(&pstParticipant->stRTPSMsgHeader, &pstParticipant->stBuiltinDataWriter[uiNum]);
        }

        /* 用户数据心跳发送 */
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
-   函 数 名: NetworkSendReceiveTask
-   功能描述: 多任务报文收发
-   输    入: 参与者
-   返    回:
-   全局变量:
-   注    释: 多任务报文收发
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\08       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID NetworkSendReceiveTask(DDS_DomainParticipant* pstParticipant)
{
    /* SPDP发送报文 */
    pstParticipant->hThread[0] = CreateThread(NULL, 0, (VOID*)&SPDPSendTask, (VOID*)pstParticipant, 0, NULL);
    /* SPDP接收报文 */
    pstParticipant->hThread[1] = CreateThread(NULL, 0, (VOID*)&SPDPReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* SEDP接收报文 */
    pstParticipant->hThread[2] = CreateThread(NULL, 0, (VOID*)&SEDPReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* USER接收报文 */
    pstParticipant->hThread[3] = CreateThread(NULL, 0, (VOID*)&USERReceiveTask, (VOID*)pstParticipant, 0, NULL);
    /* 心跳发送报文 */
    pstParticipant->hThread[4] = CreateThread(NULL, 0, (VOID*)&HEARTSendTask, (VOID*)pstParticipant, 0, NULL);
#ifdef QYDDS_LIVELINESS_QOS
	/*远端心跳检测报文*/
	pstParticipant->hThread[5] = CreateThread(NULL, 0, (VOID*)&DiscHEARTCheckTask, (VOID*)pstParticipant, 0, NULL);
#endif
}

/*---------------------------------------------------------------------------------
-   函 数 名: DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix
-   功能描述: 删除远端内建参与者
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端内建参与者
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DeleteALLDiscoveredFromByGuidPrefix
-   功能描述: 删除远端所有连接信息
-   输    入: 参与者、pstPrefix
-   返    回:
-   全局变量:
-   注    释: 删除远端所有连接信息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteALLDiscoveredFromByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t stPrefix)
{
    UINT32 uiBuiltinNun = 0;

    for (uiBuiltinNun = 0; uiBuiltinNun < 3; uiBuiltinNun++)
    {
        /* 远端内建写入器清除 */
        DeleteBuiltinDiscReaderFromBuiltinWriterByGuidPrefix(&pstParticipant->stBuiltinDataWriter[uiBuiltinNun], &stPrefix);

        /* 远端内建阅读器清除 */
        DeleteBuiltinDiscWriterFromBuiltinReaderByGuidPrefix(&pstParticipant->stBuiltinDataReader[uiBuiltinNun], &stPrefix);
    }

    DDS_Topic* pstTopic = pstParticipant->pstTopic;

    while (NULL != pstTopic)
    {
        /* 远端写入器清除 */
		DDS_MUTEX_LOCK(pstParticipant->m_discWtMutex);
		DeleteDiscWriterFromTopicByGuidPrefix(pstTopic, &stPrefix);
		DDS_MUTEX_UNLOCK(pstParticipant->m_discWtMutex);

        /* 远端阅读器清除 */
        DDS_MUTEX_LOCK(pstParticipant->m_discRdMutex);
        DeleteDiscReaderFromTopicByGuidPrefix(pstTopic, &stPrefix);
        DDS_MUTEX_UNLOCK(pstParticipant->m_discRdMutex);

        pstTopic = pstTopic->pNext;
    }

    /* 清除内建远端参与者 */
    DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix(pstParticipant, &stPrefix);
}

/*---------------------------------------------------------------------------------
-   函 数 名: UserMsgSendMonitor
-   功能描述: 删除远端所有连接信息
-   输    入: 参与者、pstPrefix
-   返    回:
-   全局变量:
-   注    释: 删除远端所有连接信息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
        /*  超过3秒清除监控节点 */
        if (time(NULL) - pstMonitor->deadtime > 3)
        {
            if (NULL == pstPreMonitor)
            {
                pstParticipant->pstMonitor = pstMonitor->pNext;
                /* 析构监控节点 */
                UninitMonitor(pstMonitor);
                DDS_STATIC_FREE(pstMonitor);
                pstMonitor = pstParticipant->pstMonitor;
            }
            else
            {
                pstPreMonitor->pNext = pstMonitor->pNext;
                /* 析构监控节点 */
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
                    /* 订阅消息标志位 */
                    pstMemBlock->base[6] = type;
                    /* 发送到对应的每个远端阅读器 */
                    RTPSSendMsg(pstMemBlock, &pstParticipant->socketList.user_data_sender, pstMonitor->unicastLocator);
                }

                pstTopicMon = pstTopicMon->pNext;
            }

			pstPreMonitor = pstMonitor;
            pstMonitor = pstMonitor->pNext;
        }
    }

}