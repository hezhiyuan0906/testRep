/*********************************************************************************
*  OpenST Basic tool library
*  Copyright (C) 2019 QiYuan
*
*
*  This program is security software
*  without permission you cannot use or distribute the software
*  otherwise, legal liability will be investigated
*
*  If you want to use the software, please contact the authorities to obtain a license
*  < http://www.iamqy.com.cn >.
*
*  @file     DataReader.h
*  @brief    DataReader
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/08
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/08 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_DATAREADER_H
#define HIPERD_DATAREADER_H

struct _Topic;
struct _DataReader;

typedef void(STACALL *CALLBACKFUN)(struct _DataReader* pstDataReader);

typedef struct _DataSeq
{
    UINT32 length;
    HistoryData* pstHead;

}DataSeq;

typedef UINT32 DataLen;

typedef struct _InfoSeq
{
    UINT32 length;
}InfoSeq;

typedef struct _DataReader
{
    GUID_t              guid; /* 本reader的Guid */
    CALLBACKFUN         recvCallBack; /* 对应的recvListener,用于通知反馈  */
    DDS_DataReaderQos   stDataReaderQos; /* 写入器qos*/
    struct _Topic*      pstTopic; /* 父节点主题 */
    UINT64              params[4]; /* 用户自定义数据 */
    Time_t				lastRecvTime;		//用于TIME_BASED_FLITER判断接收时间间隔
    UINT64              totHistoryNum;
    struct _DataReader* pNext;

}DDS_DataReader;

/* 写入器初始化 */
extern VOID DDS_DataReader_Init(DDS_DataReader* pstDataReader);

/* 写入器析构 */
extern VOID DDS_DataReader_Uninit(DDS_DataReader* pstDataReader);

/* 删除阅读器 */
extern VOID DDS_DataReader_delete(DDS_DataReader* pstDataReader);

/* 给远端writer回复ack报文 */
extern VOID JointAckNackMsgSendAssignWriter(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter);

/* 获取丢失片段 */
extern VOID GetDataReaderMissSeqNum(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, SequenceNumber_t lastSN);

/* 矫正远端写入器缓存与提交 */
extern VOID CorrectDiscoveredWriterHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter);

/*达到reader的History.depth上限后，提交最旧报文清理缓存*/
extern VOID CorrectOldHistoryCache(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter);

/*处理远端写入器Gap报文*/
extern VOID CorrectDiscoveredWriterGapMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter,Gap* pstGapMsg);

/* 提交无状态阅读器数据 */
extern VOID DeilverStatuslessUserMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, HistoryData* pstHistoryData);

/* 提交有状态阅读器数据 */
extern VOID DeilverStatusFulUserMsg(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, HistoryData* pstHistoryData);

/* 提交有状态阅读器数据，当期望seqNum比心跳firstSN还小 */
extern VOID DeilverStatusFulUserMsgWhenExpectSeqNumUnderFirstSN(DDS_DataReader* pstDataReader, DDS_DiscoveredWriter* pstDiscWriter, SequenceNumber_t firstSN);

/* 获取用户数据 */
extern DDS_DLL DDS_ReturnCode_t DDS_StringDataReader_take(DDS_DataReader* pstDataReader, DataSeq* pstDataSeq);

/* 获取用户数据 */
extern DDS_DLL DataSeq*  DDS_StringDataReader_take_cs(DDS_DataReader* pstDataReader);

/* 获取用户消息长度 */
extern DDS_DLL UINT32 DDS_StringSeq_length(DataSeq* pstDataSeq);

/* 返回用户数据 */
extern DDS_DLL CHAR* DDS_StringSeq_get(DataSeq* pstDataSeq, UINT32 num, DataLen* pDataLen);

/* 返回用户数据时间戳 */
extern DDS_DLL BOOL DDS_TimeSeq_get(DataSeq* pstDataSeq, UINT32 num, Time_t* pstTimeInfo);

/* 返回用户数据长度 */
extern DDS_DLL UINT32 DDS_StringSeq_len_cs(DataSeq* pstDataSeq, UINT32 num);

/* 返回用户数据 */
extern DDS_DLL VOID DDS_StringSeq_get_cs(DataSeq* pstDataSeq, UINT32 num,  CHAR* msg);

/* 用户数据资源释放, 获取用户数据后必须调用释放，否则出错 */
extern DDS_DLL DDS_ReturnCode_t DDS_StringDataReader_return_loan(DDS_DataReader* pstDataReader, DataSeq* pstDataSeq);

#endif