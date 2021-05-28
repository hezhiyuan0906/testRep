#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_Topic_Init
-   功能描述: 初始主题
-   输    入: 主题指针
-   返    回:
-   全局变量: 
-   注    释: 初始主题
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_Uninit
-   功能描述: 析构主题
-   输    入: 主题指针
-   返    回:
-   全局变量:
-   注    释: 析构主题
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
void DDS_Topic_Uninit(DDS_Topic* pstTopic)
{
    if (NULL != pstTopic)
    {
        UninitHistoryCache(&pstTopic->stUserData);

        /* 析构本地写入器 */
        DDS_DataWriter*  pstDataWriter = pstTopic->pstLocalWriter;
        while (NULL != pstDataWriter)
        {
            pstTopic->pstLocalWriter = pstDataWriter->pNext;

            DDS_DataWriter_Uninit(pstDataWriter);
            DDS_STATIC_FREE(pstDataWriter);

            pstDataWriter = pstTopic->pstLocalWriter;
        }

        /* 析构本地阅读器 */
        DDS_DataReader*  pstDataReader = pstTopic->pstLocalReader;
        while (NULL != pstDataReader)
        {
            pstTopic->pstLocalReader = pstDataReader->pNext;

            DDS_DataReader_Uninit(pstDataReader);
            DDS_STATIC_FREE(pstDataReader);

            pstDataReader = pstTopic->pstLocalReader;
        }

        /* 析构远端写入器 */
        DDS_DiscoveredWriter*  pstDiscWriter = pstTopic->pstDiscWriter;
        while (NULL != pstDiscWriter)
        {
            pstTopic->pstDiscWriter = pstDiscWriter->pNext;

            DDS_DiscoveredWriter_Uninit(pstDiscWriter);
            DDS_STATIC_FREE(pstDiscWriter);

            pstDiscWriter = pstTopic->pstDiscWriter;
        }

        /* 析构远端阅读器 */
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
-   函 数 名: DDS_Topic_get_default_datawriter_qos
-   功能描述: 获取默认写入器qos
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 获取默认写入器qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_get_default_datawriter_qos
-   功能描述: 获取默认写入器qos
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 获取默认写入器qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_get_default_datawriter_qos
-   功能描述: 获取默认写入器qos
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 获取默认阅读器qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_datawriter_qos_set_reliability
-   功能描述: 写入器设置可靠性
-   输    入: 写入器、可靠性
-   返    回: 返回值
-   全局变量:
-   注    释: 写入器设置可靠性qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_datareader_qos_set_reliability
-   功能描述: 阅读器设置可靠性
-   输    入: 阅读器、可靠性
-   返    回: 返回值
-   全局变量:
-   注    释: 阅读器设置可靠性qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_get_default_datawriter_qos
-   功能描述: 获取默认写入器qos
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 获取默认阅读器qos
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_create_datawriter
-   功能描述: 创建本地写入器
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 创建本地写入器并发送自身信息到订阅端
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 申请静态内存创建HistoryData链表节点 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        DDS_STATIC_FREE(pstDataWriter);
        return NULL;
    }

    /* 申请静态内存创建历史缓存数据 */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* 失败释放前面申请的内存 */
        DDS_STATIC_FREE(pstHistoryData);
        DDS_STATIC_FREE(pstDataWriter);
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* 网络字节序初始化，保存发布报文 */
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
        /* 插入本地写入器qos */
        pstDataWriter->stDataWriterQos = *pstWriterQos;
    }
    else
    {
        /* 插入本地写入器默认qos */
        pstDataWriter->stDataWriterQos = DDS_DATAWRITER_QOS_DEFAULT;
    }

    /* 初始化本地写入器 */
    DDS_DataWriter_Init(pstDataWriter);

    /* 保存父节点,方便访问上层数据 */
    pstDataWriter->pstTopic = pstTopic;

    /* 生成时间戳函数回调 */
    pstDataWriter->writerCallBack = callBackTime;

	/* 生成写入器GUID */
	GenGuid(&pstDataWriter->guid, &pstTopic->pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_WRITER_NO_KEY);

    /* 每创建一个本地写入器，内建seqNum自加1*/
    pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

    /* 设置是否取消发布 */
    pstHistoryData->bCancel = FALSE;

    /* 缓存记录seqNum编号 */
    pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum;

    /* 缓存记录实体GUID */
    pstHistoryData->stGuid = pstDataWriter->guid;

    /* 序列化写入器消息体 */
    DataWriterConvertNetworkByteOrder(pstTopic, pstDataWriter, &stMemBlock);

    /* 记录序列化报文长度 */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* 插入到内建发布写入器缓存 */
    InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

    /* 组装发布报文 */
    JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[1], NULL);

    /* 远端reader添加本地writer */
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
-   函 数 名: DDS_Topic_get_datawriter_num
-   功能描述: 获取主题下本地写入器个数
-   输    入: 主题指针
-   返    回: 写入器个数
-   全局变量:
-   注    释: 获取主题下本地写入器个数
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_create_datareader
-   功能描述: 创建本地阅读器
-   输    入: 主题指针、阅读器qos指针、回调函数
-   返    回: 写入器指针
-   全局变量:
-   注    释: 创建本地写入器并发送自身信息到订阅端
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 申请静态内存创建HistoryData链表节点 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        DDS_STATIC_FREE(pstDataReader);
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
        return NULL;
    }

    /* 申请静态内存创建历史缓存数据 */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC(NETWORK_BYTE_MAX_SIZE);
    if (NULL == pstHistoryData->data)
    {
        /* 失败释放前面申请的内存 */
        DDS_STATIC_FREE(pstDataReader);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData buff, Insufficient memory space.\n");
        return NULL;
    }

    /* 网络字节序初始化，保存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, pstHistoryData->data, NETWORK_BYTE_MAX_SIZE);

    if (NULL != pstReaderQos)
    {
        /* 插入本地阅读器qos */
        pstDataReader->stDataReaderQos = *pstReaderQos;
    }
    else
    {
        /* 插入默认阅读器qos */
        pstDataReader->stDataReaderQos = DDS_DATAREADER_QOS_DEFAULT;
    }

    if (pstDataReader->stDataReaderQos.history.depth <= 0)
    {
        DDS_STATIC_FREE(pstDataReader);
        DDS_STATIC_FREE(pstHistoryData);
        printf("ReaderQoS error: history.depth must be larger than zero! \n");
        return NULL;
    }
    /* 插入回调函数 */
    pstDataReader->recvCallBack = callBackFun;

    /* 初始化本地阅读器 */
    DDS_DataReader_Init(pstDataReader);

    /* 保存父节点 */
    pstDataReader->pstTopic = pstTopic;

	/* 生成写入器GUID */
	GenGuid(&pstDataReader->guid, &pstTopic->pstParticipant->stRTPSMsgHeader.guidPrefix, ENTITYKIND_READER_NO_KEY);

    /* 每创建一个本地阅读器，内建seqNum自加1*/
    pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

    /* 设置是否取消订阅 */
    pstHistoryData->bCancel = FALSE;

    /* 缓存记录seqNum编号 */
    pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum;

    /* 缓存记录实体GUID */
    pstHistoryData->stGuid = pstDataReader->guid;

    /* 序列化阅读器消息体 */
    DataReaderConvertNetworkByteOrder(pstTopic, pstDataReader, &stMemBlock);

    /* 记录序列化报文长度 */
    pstHistoryData->uiDataLen = stMemBlock.writeIndex;

    /* 插入到内建发布写入器缓存 */
    InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

    /* 组装订阅报文 */
    JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[2], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataReader was created successfully.\n");

    LIST_INSERT_TAIL(pstTopic->pstLocalReader, pstDataReader, pstTempDataReader);
    return pstDataReader;
}


/*---------------------------------------------------------------------------------
-   函 数 名: DDS_Topic_get_datareader_num
-   功能描述: 获取主题下本地阅读器个数
-   输    入: 主题指针
-   返    回: 阅读器个数
-   全局变量:
-   注    释: 获取主题下本地阅读器个数
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_Topic_delete_datawriter
-   功能描述: 主题删除写入器
-   输    入: 主题指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 主题删除写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datawriter(DDS_Topic* pstTopic)
{
    /* 析构本地写入器 */
    DDS_DataWriter*  pstDataWriter = pstTopic->pstLocalWriter;
    while (NULL != pstDataWriter)
    {
        /* 申请静态内存创建HistoryData链表节点 */
        HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
        if (NULL == pstHistoryData)
        {
            PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
            return DDS_RETCODE_ERROR;
        }

        /* 每创建一个本地写入器，内建seqNum自加1*/
        pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

        /* 附加数据设置 */
        pstHistoryData->data = NULL;

        /* 设置是否取消订阅 */
        pstHistoryData->bCancel = TRUE;

        /* 缓存记录seqNum编号 */
        pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[1].seqNum;

        /* 缓存记录实体GUID */
        pstHistoryData->stGuid = pstDataWriter->guid;

        /* 插入到内建发布写入器缓存 */
        InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

        /* 组装订阅报文 */
        JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[1], NULL);

        PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

        /* 析构写入器 */
        
        DDS_DataWriter_Uninit(pstDataWriter);
        

        pstDataWriter = pstDataWriter->pNext;
    }

    /* 节点删除 */
    LIST_DELETE_ALL(pstTopic->pstLocalWriter, pstDataWriter);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_Topic_delete_datawriter
-   功能描述: 主题删除写入器
-   输    入: 主题指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 主题删除写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DataWriter_delete_from_topic(DDS_DataWriter*  pstDataWriter)
{
    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* 申请静态内存创建HistoryData链表节点 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataWriter: Failed to create HistoryData, Insufficient memory space.\n");
        return DDS_RETCODE_ERROR;
    }

    /* 每创建一个本地写入器，内建seqNum自加1*/
    pstParticipant->stBuiltinDataWriter[1].seqNum = Add_SequenceNumber(pstParticipant->stBuiltinDataWriter[1].seqNum, SEQUENCENUMBER_START);

    /* 附加数据设置 */
    pstHistoryData->data = NULL;

    /* 设置是否取消订阅 */
    pstHistoryData->bCancel = TRUE;

    /* 缓存记录seqNum编号 */
    pstHistoryData->seqNum = pstParticipant->stBuiltinDataWriter[1].seqNum;

    /* 缓存记录实体GUID */
    pstHistoryData->stGuid = pstDataWriter->guid;

    /* 插入到内建发布写入器缓存 */
    InsertHistoryCacheTail(&pstParticipant->stBuiltinDataWriter[1].stHistoryCache, pstHistoryData);

    /* 组装订阅报文 */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstParticipant->stBuiltinDataWriter[1], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

    /* 析构写入器 */
    DDS_DataWriter_delete(pstDataWriter);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_Topic_delete_datawriter
-   功能描述: 主题删除阅读器
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 主题删除阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datareader(DDS_Topic* pstTopic)
{
    /* 析构本地阅读器 */
    DDS_DataReader*  pstDataReader = pstTopic->pstLocalReader;

    while (NULL != pstDataReader)
    {
        /* 申请静态内存创建HistoryData链表节点 */
        HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
        if (NULL == pstHistoryData)
        {
            PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
            return DDS_RETCODE_ERROR;
        }

        /* 每创建一个本地写入器，内建seqNum自加1*/
        pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

        /* 附加数据设置 */
        pstHistoryData->data = NULL;

        /* 设置是否取消订阅 */
        pstHistoryData->bCancel = TRUE;

        /* 缓存记录seqNum编号 */
        pstHistoryData->seqNum = pstTopic->pstParticipant->stBuiltinDataWriter[2].seqNum;

        /* 缓存记录实体GUID */
        pstHistoryData->stGuid = pstDataReader->guid;

        /* 插入到内建发布写入器缓存 */
        InsertHistoryCacheTail(&pstTopic->pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

        /* 组装订阅报文 */
        JointSEDPDataMsgSendBuiltinReader(&pstTopic->pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstTopic->pstParticipant->stBuiltinDataWriter[2], NULL);

        PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

        /* 析构写入器 */
        DDS_DataReader_Uninit(pstDataReader);

        pstDataReader = pstDataReader->pNext;
    }

    /* 节点删除 */
    LIST_DELETE_ALL(pstTopic->pstLocalReader, pstDataReader);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_Topic_delete_datawriter
-   功能描述: 主题删除阅读器
-   输    入: 主题指针、写入器qos指针
-   返    回: 写入器指针
-   全局变量:
-   注    释: 主题删除阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\18       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL DDS_ReturnCode_t DDS_DataReader_delete_from_topic(DDS_DataReader*  pstDataReader)
{
    DDS_DomainParticipant* pstParticipant =  pstDataReader->pstTopic->pstParticipant;

    /* 申请静态内存创建HistoryData链表节点 */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "DataReader: Failed to create HistoryData, Insufficient memory space.\n");
        return DDS_RETCODE_ERROR;
    }

    /* 每创建一个本地写入器，内建seqNum自加1*/
    pstParticipant->stBuiltinDataWriter[2].seqNum = Add_SequenceNumber(pstParticipant->stBuiltinDataWriter[2].seqNum, SEQUENCENUMBER_START);

    /* 附加数据设置 */
    pstHistoryData->data = NULL;

    /* 设置是否取消订阅 */
    pstHistoryData->bCancel = TRUE;

    /* 缓存记录seqNum编号 */
    pstHistoryData->seqNum = pstParticipant->stBuiltinDataWriter[2].seqNum;

    /* 缓存记录实体GUID */
    pstHistoryData->stGuid = pstDataReader->guid;

    /* 插入到内建发布写入器缓存 */
    InsertHistoryCacheTail(&pstParticipant->stBuiltinDataWriter[2].stHistoryCache, pstHistoryData);

    /* 组装订阅报文 */
    JointSEDPDataMsgSendBuiltinReader(&pstParticipant->stRTPSMsgHeader, pstHistoryData, &pstParticipant->stBuiltinDataWriter[2], NULL);

    PrintLog(COMMLOG_INFO, "Topic: The dataWriter was deleted successfully.\n");

    /* 析构写入器 */
    DDS_DataReader_delete(pstDataReader);

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetDiscoveredWriterFromTopicByEntityId
-   功能描述: 查找写入器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 从主题中通过Guid查找相应远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredWriter* GetDiscoveredWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid)
{
    if (NULL != pstTopic)
    {
        /* 遍历主题下的每个远端写入器 */
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
-   函 数 名: GetDiscoveredReaderFromTopicByEntityId
-   功能描述: 查找远端器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 从主题中通过Guid查找相应远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid)
{
    if (NULL != pstTopic)
    {
        DDS_DiscoveredReader* pstTwinDiscReader = NULL;
        /* 遍历主题下的每个远端阅读器 */
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
-   函 数 名: GetDiscoveredReaderFromTopicByEntityId
-   功能描述: 查找远端器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 从主题中通过Guid查找相应远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByPrefix(DDS_Topic* pstTopic, GuidPrefix_t* pstPrefix)
{
    if (NULL != pstTopic)
    {
        /* 遍历主题下的每个远端阅读器 */
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
-   函 数 名: DeleteDiscWriterFromTopicByGuidPrefix
-   功能描述: 删除远端写入器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DeleteDiscReaderFromTopicByGuidPrefix
-   功能描述: 删除远端阅读器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DeleteDiscWriterFromTopicByGuid
-   功能描述: 删除远端写入器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DeleteDiscReaderFromTopicByGuidPrefix
-   功能描述: 删除远端阅读器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DeleteDiscReaderFromTopicByGuidPrefix
-   功能描述: 删除远端阅读器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
