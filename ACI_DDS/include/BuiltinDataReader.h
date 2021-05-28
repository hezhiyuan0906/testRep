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
*  @file     BuiltinDataReader.h
*  @brief
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/20
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/20 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_BUILTINDATAREADER_H
#define HIPERD_BUILTINDATAREADER_H

struct _DomainParticipant;
/****************************************************************************************************************************************/
//结构体定义
/****************************************************************************************************************************************/

/* 内建远端写入器 */
typedef struct _BuiltinDiscWriter
{
    GUID_t                        guid;
    AckNack                       stAckNack;   /* 本地阅读器回复远端写入器Acknack报文 */
    Heartbeat                     stHeartbeat; /* 远端写入器发送本地阅读器心跳 */
    Locator_t*                    pstUnicastLocator;
    //Locator_t*                    pstMulticastLocator;
    HistoryCache                  stHistoryCache; /* 记录远端多个发布或订阅，该内建写入器的历史数据，只记录seqNum，不保存实际数据，方便寻找丢失分片 */
    struct _BuiltinDiscWriter* pNext;

}BuiltinDiscWriter ;

/* 内建本地阅读器 */
typedef struct _BuiltinDataReader
{
    GUID_t                 guid; /* 本reader的Guid */
    SequenceNumber_t       seqNum; /* 记录SequenceNumber_t,每插入一个新的数据自加1 */
    BuiltinDiscWriter*     pstBuiltinDataWriter; /* 内建远端写入器 */
    struct _DomainParticipant* pstParticipant;

}BuiltinDataReader;

/****************************************************************************************************************************************/
//函数声明
/****************************************************************************************************************************************/

/* 内建阅读器初始化 */
extern VOID InitBuiltinDataReader(BuiltinDataReader* pstBuiltinDataReader);

/* 内建远程写入器析构 */
extern VOID UnInitBuiltinDiscWriter(BuiltinDiscWriter* pstBuiltinDiscWriter);

/* 内建阅读器析构 */
extern VOID UnInitBuiltinDataReader(BuiltinDataReader* pstBuiltinDataReader);

/* 给远端内建writer回复ack报文 */
extern VOID JointAckNackMsgSendAssignBuiltinWriter(BuiltinDataReader* pstBuiltinDataReader, BuiltinDiscWriter* pstBuiltinDiscWriter);

/* 获取丢失内建片段 */
extern VOID GetBuiltinReaderMissSeqNum(BuiltinDataReader* pstBuiltinDataReader, BuiltinDiscWriter* pstBuiltinDiscWriter, Heartbeat* pstHeartbeatMsg);

/* 内建阅读器初插入远端写入器 */
extern BOOL BuiltinDataReaderInsertDiscDataWriter(BuiltinDataReader* pstBuiltinDataReader, GUID_t* pstGuid, Locator_t* pstUnicastLocator);

/* 查找远端内建写入器 */
extern BuiltinDiscWriter* GetBuiltinDiscWriterFromBuiltinDataReaderByPrefix(BuiltinDataReader* pstBuiltinDataReader, GuidPrefix_t* pstPrefix);

/* 删除远端内建写入器 */
extern VOID DeleteBuiltinDiscWriterFromBuiltinReaderByGuidPrefix(BuiltinDataReader* pstBuiltinDataReader, GuidPrefix_t*  pstPrefix);

#endif
