#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: InitBuiltinDataReader
-   功能描述: 内建阅读器初始化
-   输    入: 内建写入器
-   返    回:
-   全局变量:
-   注    释: 内建阅读器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/

VOID InitBuiltinDataReader(BuiltinDataReader* pstBuiltinDataReader)
{
    if (NULL != pstBuiltinDataReader)
    {
        pstBuiltinDataReader->pstBuiltinDataWriter = NULL;
        pstBuiltinDataReader->seqNum = SEQUENCENUMBER_ZERO;
        pstBuiltinDataReader->pstParticipant = NULL;
		pstBuiltinDataReader->guid = GUID_UNKNOWN;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: InitBuiltinDataReader
-   功能描述: 内建阅读器初始化
-   输    入: 内建写入器
-   返    回:
-   全局变量:
-   注    释: 内建阅读器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UnInitBuiltinDiscWriter(BuiltinDiscWriter* pstBuiltinDiscWriter)
{
    if (NULL != pstBuiltinDiscWriter)
    {
        UninitHistoryCache(&pstBuiltinDiscWriter->stHistoryCache);

        //Locator_t* pstTempNode = NULL;
        ///* 析构单播地址 */
        //LIST_DELETE_ALL(pstBuiltinDiscWriter->pstUnicastLocator, pstTempNode);

        ///* 析构组播地址 */
        //LIST_DELETE_ALL(pstBuiltinDiscWriter->pstMulticastLocator, pstTempNode);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: UnInitBuiltinDataReader
-   功能描述: 内建阅读器初始化
-   输    入: 内建写入器
-   返    回:
-   全局变量:
-   注    释: 内建阅读器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UnInitBuiltinDataReader(BuiltinDataReader* pstBuiltinDataReader)
{
    if (NULL != pstBuiltinDataReader)
    {
        BuiltinDiscWriter* pstBuiltinDiscWriter = pstBuiltinDataReader->pstBuiltinDataWriter;
        while (NULL != pstBuiltinDiscWriter)
        {
            pstBuiltinDataReader->pstBuiltinDataWriter = pstBuiltinDiscWriter->pNext;
            UnInitBuiltinDiscWriter(pstBuiltinDiscWriter);
            DDS_STATIC_FREE(pstBuiltinDiscWriter);
            pstBuiltinDiscWriter = pstBuiltinDataReader->pstBuiltinDataWriter;
        }
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointAckNackMsgSendAssignBuiltinWriter
-   功能描述: 给远端内建writer回复ack报文
-   输    入: 本地内建阅读器、远端内建写入器
-   返    回:
-   全局变量:
-   注    释: 给远端内建写入器回复ack报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointAckNackMsgSendAssignBuiltinWriter(BuiltinDataReader* pstBuiltinDataReader, BuiltinDiscWriter* pstBuiltinDiscWriter)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoDestination   stInfoDest;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* 网络字节序初始化 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstBuiltinDataReader->pstParticipant->stRTPSMsgHeader, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   
    stSubMsgHeader.flags[2] = FALSE;   
    stSubMsgHeader.flags[3] = FALSE;   
    stInfoDest.guidPrefix = pstBuiltinDiscWriter->guid.prefix;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    stSubMsgHeader.submessageId = ACKNACK;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    if (pstBuiltinDiscWriter->stAckNack.readerSNState.numBits)
    {
        stSubMsgHeader.flags[1] = FALSE;
    }
    else
    {
        stSubMsgHeader.flags[1] = TRUE;
    }

    pstBuiltinDiscWriter->stAckNack.readerId = pstBuiltinDataReader->guid.entityId;
    pstBuiltinDiscWriter->stAckNack.writerId = pstBuiltinDiscWriter->guid.entityId;
    pstBuiltinDiscWriter->stAckNack.count++;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_ACKNACK(pstBuiltinDiscWriter->stAckNack, stMemBlock);

    /* 子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstBuiltinDiscWriter->stAckNack.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* 发送ack报文 */
    RTPSSendMsg(&stMemBlock, &pstBuiltinDataReader->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscWriter->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetBuiltinReaderMissSeqNum
-   功能描述: 获取丢失内建数据包
-   输    入: 本地内建阅读器、远端内建写入器、心跳报文
-   返    回:
-   全局变量:
-   注    释: 获取丢失内建数据包
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/

VOID GetBuiltinReaderMissSeqNum(BuiltinDataReader* pstBuiltinDataReader, BuiltinDiscWriter* pstBuiltinDiscWriter, Heartbeat* pstHeartbeatMsg)
{
    /* HistoryCache中seqNum是离散递增排列，expectSeqNum到lastSN是离散连续递增排列，根据这一特性，归并比较一次即可找出所有缺失片段 */
    HistoryData* pstDelHistoryData = NULL;
    const UINT32  UINT32_BIT = 32;
    UINT32 uiNumbit = 0;

    pstBuiltinDiscWriter->stAckNack.readerSNState.numBits = 0;

    /* 最大seqNUm小于期望值，则数据未缺失 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstBuiltinDiscWriter->stAckNack.readerSNState.base))
    {
        return;
    }

    /* 比最小的还小，调整期望值 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstBuiltinDiscWriter->stAckNack.readerSNState.base, pstHeartbeatMsg->firstSN))
    {
        pstBuiltinDiscWriter->stAckNack.readerSNState.base = pstHeartbeatMsg->firstSN;
    }

    /* 远端写入器缓存为离散递增，根据这一特性快速提交数据以及更新期望 */
    HistoryData* pstHistoryData = pstBuiltinDiscWriter->stHistoryCache.pstHead;
    while (NULL != pstHistoryData)
    {
        /* 缓存数据小于期望值，删除缓存 */
        if (ISLESSTHEN_SEQUENCENUMBER(pstHistoryData->seqNum, pstBuiltinDiscWriter->stAckNack.readerSNState.base))
        {
            pstBuiltinDiscWriter->stHistoryCache.pstHead = pstHistoryData->pNext;

            /* 清除时注意尾节点 */
            if (pstHistoryData == pstBuiltinDiscWriter->stHistoryCache.pstTail)
            {
                pstBuiltinDiscWriter->stHistoryCache.pstTail = NULL;
            }
            pstDelHistoryData = pstHistoryData;
        }
        /* 缓存数据等于期望值，删除缓存并更新下次期望的seqNum */
        else if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, pstBuiltinDiscWriter->stAckNack.readerSNState.base))
        {
            pstBuiltinDiscWriter->stHistoryCache.pstHead = pstHistoryData->pNext;

            /* 清除时注意尾节点 */
            if (pstHistoryData == pstBuiltinDiscWriter->stHistoryCache.pstTail)
            {
                pstBuiltinDiscWriter->stHistoryCache.pstTail = NULL;
            }
            pstDelHistoryData = pstHistoryData;
            pstBuiltinDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstBuiltinDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
        }
        /* 缓存数据大于期望值，离散不连续递增数据直接跳出 */
        else
        {
            break;
        }
        pstHistoryData = pstHistoryData->pNext;

        /* 跳到下个节点后释放上个节点内存 */
        DDS_STATIC_FREE(pstDelHistoryData);
    }

    /* 最大seqNUm小于期望值，则数据未缺失 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstHeartbeatMsg->lastSN, pstBuiltinDiscWriter->stAckNack.readerSNState.base))
    {
        return;
    }

    /* 集合清空，每一位都表示当前序号是否缺失 */
    memset(pstBuiltinDiscWriter->stAckNack.readerSNState.bitMap, 0, SEQUENCE_BIT_NUMBER);

    pstHistoryData = pstBuiltinDiscWriter->stHistoryCache.pstHead;

    SequenceNumber_t tempSN = pstBuiltinDiscWriter->stAckNack.readerSNState.base;
    while (!ISEQUAL_SEQUENCENUMBER(tempSN, Add_SequenceNumber(pstHeartbeatMsg->lastSN, SEQUENCENUMBER_START)))
    {
        if (NULL != pstHistoryData)
        {
            /* 缓存里有这条数据，标记未缺失 */
            if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, tempSN))
            {
                pstHistoryData = pstHistoryData->pNext;
            }
            /* 缓存里也没有这条数据，记录缺失 */
            else
            {
                /* 每一位记录是否有缺失，从base开始，往后记录连续256条是否有缺失 */
                pstBuiltinDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));
            }
        }
        else
        {
            /* 缓存遍历完，剩下全是缺失，记录缺失 */
            pstBuiltinDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));
        }

        pstBuiltinDiscWriter->stAckNack.readerSNState.numBits = uiNumbit + 1;

        tempSN = Add_SequenceNumber(tempSN, SEQUENCENUMBER_START);

        /* 计数器，超过256，即不再添加缺失，等待下次心跳处理 */
        uiNumbit++;
        if (256 == uiNumbit)
        {
            return;
        }
    }

}

/*---------------------------------------------------------------------------------
-   函 数 名: BuiltinDataReaderInsertDiscDataWriter
-   功能描述: 内建阅读器初插入远端写入器
-   输    入: 内建阅读器、远端guid、地址
-   返    回:
-   全局变量:
-   注    释: 内建阅读器初插入远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL BuiltinDataReaderInsertDiscDataWriter(BuiltinDataReader* pstBuiltinDataReader, GUID_t* pstGuid, Locator_t* pstUnicastLocator)
{
    UINT32 uiHeartNum = 0;
    if (NULL != pstBuiltinDataReader)
    {
        /* 申请静态内存创建内建远端阅读器 */
        BuiltinDiscWriter*  pstBuiltinDiscWriter = (BuiltinDiscWriter*)DDS_STATIC_MALLOC(sizeof(BuiltinDiscWriter));
        if (NULL == pstBuiltinDiscWriter)
        {
            PrintLog(COMMLOG_ERROR, "BuiltinDiscReader: Failed to create BuiltinDiscWriter, Insufficient memory space.\n");
            return FALSE;
        }

        pstBuiltinDiscWriter->guid = *pstGuid;
        pstBuiltinDiscWriter->stHeartbeat.count = 0;
        pstBuiltinDiscWriter->stAckNack.count = 0;
        pstBuiltinDiscWriter->stAckNack.readerSNState.base = SEQUENCENUMBER_ZERO;

        /*  numBits大于零，促使flags[1]为false，强制对端发送心跳报文 */
        pstBuiltinDiscWriter->stAckNack.readerSNState.numBits = 1;
        pstBuiltinDiscWriter->pstUnicastLocator = pstUnicastLocator;

        /* 历史缓存初始化 */
        InitHistoryCache(&pstBuiltinDiscWriter->stHistoryCache);

        /* 链表头插 */
        LIST_INSERT_HEAD(pstBuiltinDataReader->pstBuiltinDataWriter, pstBuiltinDiscWriter);

        /* 回复ack让对方强制发送心跳报文以校验缺失 */
        memset(pstBuiltinDiscWriter->stAckNack.readerSNState.bitMap, 0, 4);

       JointAckNackMsgSendAssignBuiltinWriter(pstBuiltinDataReader, pstBuiltinDiscWriter);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix
-   功能描述: 查找远端内建写入器
-   输    入: 内建写入器、GuidPrefix_t
-   返    回:
-   全局变量:
-   注    释: 查找远端内建写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BuiltinDiscWriter* GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(BuiltinDataReader* pstBuiltinDataReader, GuidPrefix_t* pstPrefix)
{
    BuiltinDiscWriter* pstBuiltinDiscWriter = pstBuiltinDataReader->pstBuiltinDataWriter;

    while (NULL != pstBuiltinDiscWriter)
    {
        if (GuidPrefix_Is_Equal(&pstBuiltinDiscWriter->guid.prefix, pstPrefix))
        {
            return pstBuiltinDiscWriter;
        }
        pstBuiltinDiscWriter = pstBuiltinDiscWriter->pNext;
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DeleteBuiltinDiscWriterFromBuiltinReaderByGuidPrefix
-   功能描述: 删除远端内建写入器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 删除远端内建写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteBuiltinDiscWriterFromBuiltinReaderByGuidPrefix(BuiltinDataReader* pstBuiltinDataReader, GuidPrefix_t*  pstPrefix)
{
    BuiltinDiscWriter*  pstPrevBuiltinDataWriter = NULL;
    BuiltinDiscWriter*  pstBuiltinDataWriter = pstBuiltinDataReader->pstBuiltinDataWriter;

    while (NULL != pstBuiltinDataWriter)
    {
        if (GuidPrefix_Is_Equal(&pstBuiltinDataWriter->guid.prefix, pstPrefix))
        {
            if (!pstPrevBuiltinDataWriter)
            {
                pstBuiltinDataWriter = pstBuiltinDataWriter->pNext;
                DDS_STATIC_FREE(pstBuiltinDataReader->pstBuiltinDataWriter);
                pstBuiltinDataReader->pstBuiltinDataWriter = pstBuiltinDataWriter;
            }
            else
            {
                pstPrevBuiltinDataWriter->pNext = pstBuiltinDataWriter->pNext;
                DDS_STATIC_FREE(pstBuiltinDataWriter);
                pstBuiltinDataWriter = pstPrevBuiltinDataWriter->pNext;
            }
        }
        else
        {
            pstPrevBuiltinDataWriter = pstBuiltinDataWriter;
            pstBuiltinDataWriter = pstBuiltinDataWriter->pNext;
        }
    }
}