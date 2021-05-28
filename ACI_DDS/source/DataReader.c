#include "../Include/GlobalDefine.h"
/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DataReader_Init
-   功能描述: 阅读器初始化
-   输    入: 阅读器指针
-   返    回:
-   全局变量:
-   注    释: 阅读器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DataReader_Uninit
-   功能描述: 写入器初始化
-   输    入: 写入器指针
-   返    回:
-   全局变量:
-   注    释: 写入器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_DataReader_delete
-   功能描述: 阅读器删除
-   输    入: 阅读器指针
-   返    回:
-   全局变量:
-   注    释: 阅读器删除
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: JointAckNackMsgSendAssignWriter
-   功能描述: 给远端writer回复ack报文
-   输    入: 本地写入器指针
-   返    回:
-   全局变量:
-   注    释: 给远端writer回复ack报文
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 网络字节序初始化 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER(pstDiscWriter->pstTopic->pstParticipant->stRTPSMsgHeader, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = pstDiscWriter->guid.prefix;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* 设置ACK报文头 */
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

    /* 设置ACK报文消息体 */
    pstDiscWriter->stAckNack.readerId = pstDataReader->guid.entityId;
    pstDiscWriter->stAckNack.writerId = pstDiscWriter->guid.entityId;
    pstDiscWriter->stAckNack.count++;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_ACKNACK(pstDiscWriter->stAckNack, stMemBlock);
    

    /* 子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstDiscWriter->stAckNack.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* 发送ack报文 */
    RTPSSendMsg(&stMemBlock, &pstDataReader->pstTopic->pstParticipant->socketList.user_data_sender, pstDiscWriter->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetMissSeqNum
-   功能描述: 获取丢失片段
-   输    入: 本地阅读器，远端写入器，心跳最大seqNum
-   返    回:
-   全局变量:
-   注    释: 获取丢失片段
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID GetDataReaderMissSeqNum(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, SequenceNumber_t lastSN)
{
    /* HistoryCache中seqNum是离散递增排列，expectSeqNum到lastSN是离散连续递增排列，根据这一特性，归并比较一次即可找出所有缺失片段，大数据递归、遍历乃编码大忌，骚年 切记切记 */
    HistoryData* pstHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    UINT32 uiNumbit = 0;

    /* 集合清空，每一位都表示当前序号是否缺失 */
    memset(pstDiscWriter->stAckNack.readerSNState.bitMap, 0, SEQUENCE_BIT_NUMBER);

    SequenceNumber_t tempSN = pstDiscWriter->stAckNack.readerSNState.base;

    /* 最大值比期望值小，说明未丢失 */
    if (ISLESSTHEN_SEQUENCENUMBER(lastSN, tempSN))
    {
        pstDiscWriter->stAckNack.readerSNState.numBits = 0;
        return;
    }

    while (!ISEQUAL_SEQUENCENUMBER(tempSN, Add_SequenceNumber(lastSN, SEQUENCENUMBER_START)))
    {
        if (NULL != pstHistoryData)
        {
            /* 缓存里有这条数据，跳过 */
            if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, tempSN))
            {
                pstHistoryData = pstHistoryData->pNext;
            }
            /* 缓存里也没有这条数据，记录缺失 */
            else
            {
                /* 每一位记录是否有缺失，从base开始，往后记录连续256条是否有缺失, 位运算替代传统的除法与取模运算 */
                pstDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));;
            }
        }
        else
        {
            /* 缓存遍历完，剩下全是缺失，记录缺失 */
            pstDiscWriter->stAckNack.readerSNState.bitMap[uiNumbit >> 0x05] |= (0x80000000 >> (uiNumbit & 0x1f));;
        }
        pstDiscWriter->stAckNack.readerSNState.numBits = uiNumbit + 1;

        tempSN = Add_SequenceNumber(tempSN, SEQUENCENUMBER_START);

        /* 计数器，超过256，即不在添加缺失，等待下次心跳处理 */
        uiNumbit++;
        if (256 == uiNumbit)
        {
            return;
        }
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: CorrectAllHistoryCache
-   功能描述: reader总缓存达到depth上限后，清理所有writer的最旧缓存
-   输    入: 本地阅读器
-   返    回:
-   全局变量:
-   注    释: 
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\12\08       1.0        Frank       Created
----------------------------------------------------------------------------------*/
VOID CorrectOldHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter)
{
    HistoryData* pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    while (NULL != pstTempHistoryData)
    {
        /*总数量减少到了depth上限以下，结束提交*/
        if (pstDiscWriter->stHistoryCache.uiHistoryDataNum <= pstDataReader->stDataReaderQos.history.depth)
        {
            return ;
        }
        else
        {
            /* 缓存数据剔除 */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* 提交到用户 */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* 每提交一次，用户数据个数自增 */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* 缓存数据自减 */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

            /*更新期望*/
            pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstTempHistoryData->seqNum, SEQUENCENUMBER_START);
        }
        pstTempHistoryData = pstTempHistoryData->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: CorrectDiscoveredWriterHistoryCache
-   功能描述: 矫正远端写入器缓存与提交
-   输    入: 本地阅读器、远端写入器
-   返    回:
-   全局变量:
-   注    释: 矫正远端写入器缓存与提交
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID CorrectDiscoveredWriterHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter)
{
    /* 远端写入器缓存为离散递增，根据这一特性快速提交数据以及更新期望 */
    HistoryData* pstTempHistoryData = pstDiscWriter->stHistoryCache.pstHead;
    while (NULL != pstTempHistoryData)
    {
        /* 缓存数据小于期望值，无论顺序全部提交 */
        if (ISLESSTHEN_SEQUENCENUMBER(pstTempHistoryData->seqNum, pstDiscWriter->stAckNack.readerSNState.base))
        {
            /* 清除时注意尾节点 */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* 缓存数据剔除 */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* 提交到用户 */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* 每提交一次，用户数据个数自增 */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* 缓存数据自减 */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;
        }
        /* 缓存数据等于期望值，离散连续递增数据提交 */
        else if (ISEQUAL_SEQUENCENUMBER(pstTempHistoryData->seqNum, pstDiscWriter->stAckNack.readerSNState.base))
        {
            /* 清除时注意尾节点 */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* 缓存数据剔除 */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* 提交到用户 */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* 每提交一次，用户数据个数自增 */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* 缓存数据自减 */
            pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

            /* 更新下次期望的seqNum */
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
        /* 缓存数据大于期望值*/
        else
        {
			//如果是KEEP_ALL，将期望值更新为当前seqNum
			if (pstDiscWriter->stDataWriterQos.history.kind == KEEP_ALL_HISTORY_QOS && pstDiscWriter->stDataWriterQos.durability.kind == TRANSIENT_DURABILITY_QOS)
			{
				/* 更新下次期望的seqNum */
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
                    /* 缓存数据剔除 */
                    pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

                    /* 提交到用户 */
                    DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

                    /* 每提交一次，用户数据个数自增 */
                    pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

                    /* 缓存数据自减 */
                    pstDiscWriter->stHistoryCache.uiHistoryDataNum--;

					pstDiscWriter->stAckNack.readerSNState.base = pstTempHistoryData->seqNum;
                    //pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
				}
			}
            /*如果是KEEP_LAST，则等待发端重传，直接return*/
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
        /*比Gap报文中最大丢失数据SeqNum小的，全部提交*/
        if (ISLESSTHEN_SEQUENCENUMBER(pstTempHistoryData->seqNum, gapEnd))
        {
            /* 清除时注意尾节点 */
            if (pstTempHistoryData == pstDiscWriter->stHistoryCache.pstTail)
            {
                pstDiscWriter->stHistoryCache.pstTail = NULL;
            }

            /* 缓存数据剔除 */
            pstDiscWriter->stHistoryCache.pstHead = pstTempHistoryData->pNext;

            /* 提交到用户 */
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstTempHistoryData);

            /* 每提交一次，用户数据个数自增 */
            pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;

            /* 缓存数据自减 */
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
-   函 数 名: DeilverStatuslessUserMsg
-   功能描述: 提交无状态阅读器数据
-   输    入: 阅读器、远端写入器、用户缓存数据
-   返    回:
-   全局变量:
-   注    释: 提交无状态阅读器数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
        /* 提交到用户 */
        DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstHistoryData);
    }

    if (pstHistoryData->uiFragTotal == 0 || (pstHistoryData->uiFragTotal == pstHistoryData->uiFragNum))
    {
        /* 每提交一次，用户数据个数自增 */
        pstDataReader->pstTopic->stUserData.uiHistoryDataNum++;
        /* 更新下次期望的seqNum */
        pstDiscWriter->stAckNack.readerSNState.base = Add_SequenceNumber(pstDiscWriter->stAckNack.readerSNState.base, SEQUENCENUMBER_START);
    }
    while (NULL != pstDataReader)
    {
        /* 回调函数，处理用户数据 */
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
-   函 数 名: DeilverStatusFulUserMsg
-   功能描述: 提交有状态阅读器数据
-   输    入: 阅读器、远端写入器、用户缓存数据
-   返    回:
-   全局变量:
-   注    释: 提交有状态阅读器数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /*DeadLine数据直接提交*/
    if (strcmp(pstHistoryData->data, "DeadLine MSG.") == 0)
    {
        /* 提交到用户 */
        if (NULL != pstDataReader)
        {
            DOUBLE_INSERT_TAIL(pstDataReader->pstTopic->stUserData.pstHead, pstDataReader->pstTopic->stUserData.pstTail, pstHistoryData);
        }
        while (NULL != pstDataReader)
        {

            /* 回调函数，处理用户数据 */
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

    /* 缓存里有数据直接删除，没有添加 */
    if (!InsertHistoryCacheOrder(&pstDiscWriter->stHistoryCache, pstHistoryData))
    {
        DDS_STATIC_FREE(pstHistoryData->data);
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_INFO, "HandleUSERWriterDataMsg:User data was not inserted, Because user data already exists.\n");
    }
    
    /*超出history上限，清理最旧缓存*/
    if ((pstDataReader->stDataReaderQos.history.kind == KEEP_LAST_HISTORY_QOS) && (pstDataReader->stDataReaderQos.history.depth != 0)
        && (pstDiscWriter->stHistoryCache.uiHistoryDataNum > pstDataReader->stDataReaderQos.history.depth))
    {
        CorrectOldHistoryCache(pstDataReader, pstDiscWriter);
    }
    else
    {
        /* 缓存上送 */
        CorrectDiscoveredWriterHistoryCache(pstDataReader, pstDiscWriter);
    }

    

    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {

            /* 回调函数，处理用户数据 */
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
-   函 数 名: DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN
-   功能描述: 提交有状态阅读器数据，当期望seqNum比心跳firstSN还小
-   输    入: 阅读器、远端写入器、用户缓存数据
-   返    回:
-   全局变量:
-   注    释: 提交有状态阅读器数据，当期望seqNum比心跳firstSN还小
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 调整期望值大小，为心跳最小值 */
    pstDiscWriter->stAckNack.readerSNState.base = firstSN;

    /* 矫正缓存*/
    CorrectDiscoveredWriterHistoryCache(pstDataReader, pstDiscWriter);
    if (NULL == pstDataReader)
    {
        return;
    }
    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {
            /* 回调函数，处理用户数据 */
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
-   函 数 名: DDS_DataReader_take
-   功能描述: 获取用户数据
-   输    入: 阅读器、DataSeq
-   返    回: DDS_RETCODE_OK、DDS_RETCODE_NO_DATA
-   全局变量:
-   注    释: 获取用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
    /* 缓存数据个数赋值 */
    if (strcmp(tmpHistoryData->data, "DeadLine MSG.") == 0)
    {
        pstDataSeq->length = 1;
    }
    else
    {
        pstDataSeq->length = pstDataReader->pstTopic->stUserData.uiHistoryDataNum;
    }
    /* 缓存链表首地址 */
    pstDataSeq->pstHead = tmpHistoryData;

    return DDS_RETCODE_OK;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DataReader_take
-   功能描述: 获取用户数据
-   输    入: 阅读器、DataSeq
-   返    回: DDS_RETCODE_OK、DDS_RETCODE_NO_DATA
-   全局变量:
-   注    释: 获取用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

    /* 缓存数据个数赋值 */
    pstDataSeq->length = pstDataReader->pstTopic->stUserData.uiHistoryDataNum;

    /* 缓存链表首地址 */
    pstDataSeq->pstHead = pstDataReader->pstTopic->stUserData.pstHead;

    return pstDataSeq;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_StringSeq_length
-   功能描述: 返回用户数据
-   输    入: DataSeq、数据编号
-   返    回: 用户数据指针
-   全局变量:
-   注    释: 返回用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_StringSeq_get
-   功能描述: 返回用户数据
-   输    入: DataSeq、数据编号
-   返    回: 用户数据指针
-   全局变量:
-   注    释: 返回用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_StringSeq_get
-   功能描述: 返回用户数据
-   输    入: DataSeq、数据编号
-   返    回: 用户数据指针
-   全局变量:
-   注    释: 返回用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_StringSeq_get
-   功能描述: 返回用户数据
-   输    入: DataSeq、数据编号
-   返    回: 用户数据指针
-   全局变量:
-   注    释: 返回用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_StringSeq_get
-   功能描述: 返回用户数据
-   输    入: DataSeq、数据编号
-   返    回: 用户数据指针
-   全局变量:
-   注    释: 返回用户数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: DDS_StringDataReader_return_loan
-   功能描述: 用户数据资源释放
-   输    入: 本地阅读器
-   返    回: DDS_RETCODE_OK、DDS_RETCODE_NO_DATA
-   全局变量:
-   注    释: 用户数据资源释放, 获取用户数据后必须调用释放，否则出错
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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

        /* 释放缓存器中的数据区 */
        DDS_STATIC_FREE(pstDelHistoryData->data);

        /* 释放缓存器 */
        DDS_STATIC_FREE(pstDelHistoryData);
    }

    /* 缓存初始化 */
    InitHistoryCache(&pstDataReader->pstTopic->stUserData);

    return DDS_RETCODE_OK;
}


