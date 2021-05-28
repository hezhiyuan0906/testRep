#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: HandleSEDPDiscoveredWriterDataMsg
-   功能描述: 处理SEDP发现写入器报文
-   输    入: 子报文消息头、Data、内存块、Participant
-   返    回:
-   全局变量: 
-   注    释: 处理SEDP发现写入器报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 内建发布者writer */
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

    /* 初始化历史数据 */
    InitHistoryData(pstHistoryData);

    /* 记录seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* 序列在线qos */
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
        if (stInlineQos.disposed)
        {
            DDS_Topic* pstTopic = pstParticipant->pstTopic;
            while (NULL != pstTopic)
            {
                /* 远端写入器清除 */
                DDS_MUTEX_LOCK(pstParticipant->m_discWtMutex);
                DeleteDiscWriterFromTopicByGuid(pstTopic, &stInlineQos.guid);
                DDS_MUTEX_UNLOCK(pstParticipant->m_discWtMutex);

                pstTopic = pstTopic->pNext;
            }
        }
    }

    /* 处理发布报文 */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        /* 创建主题申请内存 */
        DDS_Topic* pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
        if (NULL == pstTopic)
        {
            DDS_STATIC_FREE(pstHistoryData);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create remote topic, Insufficient memory space.\n");
            return FALSE;
        }

        /* 初始化主题 */
        DDS_Topic_Init(pstTopic);

        /* 创建远端写入器申请内存 */
        DDS_DiscoveredWriter*  pstDiscWriter = (DDS_DiscoveredWriter*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredWriter));
        if (NULL == pstDiscWriter)
        {
            DDS_STATIC_FREE(pstHistoryData);
            DDS_STATIC_FREE(pstTopic);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredWriter: Failed to create remote DiscoveredWriter, Insufficient memory space.\n");
            return FALSE;
        }

        /* 初始化写入器 */
        DDS_DiscoveredWriter_Init(pstDiscWriter);

        /* 序列化data present */
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));
        NetworkByteOrderConvertDiscDataWriter(pstMemBlock, pstTopic, pstDiscWriter);

        /* Participant地址挂到对应的阅读器上 */
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

        /* 校验本地是否存在该主题 */
        DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
        while (NULL != pstTempTopic)
        {
            /* 主题本地是否已存在 */
            if (!strcmp(pstTopic->topicName.value, pstTempTopic->topicName.value) && !strcmp(pstTopic->topicType.value, pstTempTopic->topicType.value))
            {
                /* 若本地主题已存在该远端写入器，则释放内存直接放回 */
                pstTempDiscWriter = GetDiscoveredWriterFromTopicByGuid(pstTempTopic, &pstDiscWriter->guid);
                if (NULL != pstTempDiscWriter)
                {
                    PrintLog(COMMLOG_INFO, "SEDPDiscoveredWriter: The discovered writer is exist | topic name: %s.\n", pstTopic->topicName.value);
                    DDS_STATIC_FREE(pstTopic);
                    DDS_STATIC_FREE(pstDiscWriter);
                    DDS_STATIC_FREE(pstHistoryData);
                    return TRUE;
                }
                /* 释放新建的主题 */
                DDS_STATIC_FREE(pstTopic);

                /* 插入内建发布者reader,失败则清理 */
                if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
                {
                    DDS_STATIC_FREE(pstHistoryData);
                }

                /* 插入父节点，方便访问上层数据结构 */
                pstDiscWriter->pstTopic = pstTempTopic;

                /* 远端写入器不存在，需插入 */
                LIST_INSERT_HEAD(pstTempTopic->pstDiscWriter, pstDiscWriter);

                return TRUE;
            }
            pstTempTopic = pstTempTopic->pNext;
        }

        /* 插入父节点，方便访问上层数据结构 */
        pstDiscWriter->pstTopic = pstTopic;

        /* 远端写入器插入到新主题下*/
        LIST_INSERT_HEAD(pstTopic->pstDiscWriter, pstDiscWriter);

        /* 插入父节点，方便访问上层数据结构 */
        pstTopic->pstParticipant = pstParticipant;

        /* 该新主题插入到Participant下的主题链表 */
        LIST_INSERT_HEAD(pstParticipant->pstTopic, pstTopic);
    }

    /* 插入内建发布者reader,失败则清理 */
    if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: HandleSEDPDiscoveredReaderDataMsg
-   功能描述: 处理SEDP发现阅读器报文
-   输    入: 子报文消息头、Data、内存块、Participant
-   返    回:
-   全局变量:
-   注    释: 处理SEDP发现阅读器报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 内建发布者reader */
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

    /* 初始化历史数据 */
    InitHistoryData(pstHistoryData);

    /* 记录seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* 反序列化在线qos信息 */
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0};
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);

        if (stInlineQos.disposed)
        {
            DDS_Topic* pstTopic = pstParticipant->pstTopic;
            while (NULL != pstTopic)
            {
                /* 远端阅读器清除 */
                DDS_MUTEX_LOCK(pstParticipant->m_discRdMutex);
                DeleteDiscReaderFromTopicByGuid(pstTopic, &stInlineQos.guid);
                DDS_MUTEX_UNLOCK(pstParticipant->m_discRdMutex);

                pstTopic = pstTopic->pNext;
            }
        }
    }

    /* 处理订阅报文 */
    if (pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3])
    {
        /* 创建主题申请内存 */
        DDS_Topic* pstTopic = (DDS_Topic*)DDS_STATIC_MALLOC(sizeof(DDS_Topic));
        if (NULL == pstTopic)
        {
            DDS_STATIC_FREE(pstHistoryData);
            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredReader: Failed to create remote topic, Insufficient memory space.\n");
            return FALSE;
        }

        /* 初始化主题 */
        DDS_Topic_Init(pstTopic);

        /* 创建远端写入器申请内存 */
        DDS_DiscoveredReader*  pstDiscReader = (DDS_DiscoveredReader*)DDS_STATIC_MALLOC(sizeof(DDS_DiscoveredReader));
        if (NULL == pstDiscReader)
        {
            /* 失败需释放之前创建的主题并返回 */
            DDS_STATIC_FREE(pstHistoryData);
            DDS_STATIC_FREE(pstTopic);

            PrintLog(COMMLOG_ERROR, "SEDPDiscoveredReader: Failed to create remote DiscoveredWriter, Insufficient memory space.\n");
            return FALSE;
        }

        /* 初始化远端阅读器 */
        DDS_DiscoveredReader_Init(pstDiscReader);

        /* 反序列化data present */
        DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));
        NetworkByteOrderConvertDiscDataReader(pstMemBlock, pstTopic, pstDiscReader);
    
        /* Participant地址挂到对应的阅读器上 */
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

        /* 校验本地是否存在该主题 */
        DDS_Topic* pstTempTopic = pstParticipant->pstTopic;
        while (NULL != pstTempTopic)
        {
            /* 发现本地已创建该主题，校验主题类型是否一致*/
            if (!strcmp(pstTopic->topicName.value, pstTempTopic->topicName.value) && !strcmp(pstTopic->topicType.value, pstTempTopic->topicType.value))
            {
                /* 若本地主题已存在该远端写入器，则释放内存直接放回 */
                pstTempDiscReader = GetDiscoveredReaderFromTopicByGuid(pstTempTopic, &pstDiscReader->guid);
                if (NULL != pstTempDiscReader)
                {
                    PrintLog(COMMLOG_INFO, "SEDPDiscoveredReader: The discovered reader is exist | topic name: %s.\n", pstTopic->topicName.value);
                    DDS_STATIC_FREE(pstTopic);
                    DDS_STATIC_FREE(pstDiscReader);
                    DDS_STATIC_FREE(pstHistoryData);
                    return TRUE;
                }
                /* 释放新建的主题 */
                DDS_STATIC_FREE(pstTopic);

                /* 插入内建订阅者reader,已存在则清理 */
                if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
                {
                    DDS_STATIC_FREE(pstHistoryData);
                }

                /* 孪生远端reader插入 */
                pstTempDiscReader =  GetDiscoveredReaderFromTopicByPrefix(pstTempTopic, &pstDiscReader->guid.prefix);
                if (NULL != pstTempDiscReader)
                {
                    LIST_INSERT_TAIL(pstTempDiscReader->pstTwinDiscReader, pstDiscReader, pstTwinDiscReader);
                    return TRUE;
                }

                /* 插入父节点，方便访问上层数据结构 */
                pstDiscReader->pstTopic = pstTempTopic;

                /* 远端写入器不存在，需插入 */
                LIST_INSERT_HEAD(pstTempTopic->pstDiscReader, pstDiscReader);

                /* 远端reader添加本地writer */
                
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

        /* 插入父节点，方便访问上层数据结构 */
        pstDiscReader->pstTopic = pstTempTopic;

        /* 远端写入器插入到新主题下*/
        LIST_INSERT_HEAD(pstTopic->pstDiscReader, pstDiscReader);

        /* 插入父节点，方便访问上层数据结构 */
        pstTopic->pstParticipant = pstParticipant;

        /* 该新主题插入到Participant下的主题链表 */
        LIST_INSERT_HEAD(pstParticipant->pstTopic, pstTopic);

    }

    /* 插入内建订阅者reader, 已存在则清理 */
    if (!InsertHistoryCacheOrder(&pstBuiltinDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData);
    }

    return TRUE;
}

