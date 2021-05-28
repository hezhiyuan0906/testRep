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
*  @file     HistoryCache.h
*  @brief    HistoryCache
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

/********************************************************************
HistoryCache.h
********************************************************************/

#ifndef HIPERD_HISTORYCACHE_H
#define HIPERD_HISTORYCACHE_H

#define HISTORYCACHE_MAX 512

// 消息类型
typedef enum DCPSMESSAGEID
{
    SAMPLE_DATA                 = 0x00,    /* 普通的样本数据 */
    REGISTER_INSTANCE           = 0x01,    /* 注册实例 */
    UNREGISTER_INSTANCE         = 0x02,    /* 注销实例 */
    DISPOSE_INSTANCE            = 0x03,    /* 删除实例 */
    DISPOSE_UNREGISTER_INSTANCE = 0x04,    /* 注销并删除实例 */
    END_COHERENT_CHANGES        = 0x05, /* 相干性控制 */
    MESSAGE_INVALID             = 0x06    /* 无效的消息类型 */
}DCPSMESSAGEID;

typedef struct _FragData
{
    USHORT              usFragIndex;    /*分片编号*/
    USHORT              usDataLen;      /*用户数据长度*/
    CHAR*               data;           /*用户数据*/
}FragData;


typedef struct _HistoryData
{
    BOOL                 bCancel;   /* 是否取消发布订阅 */
    SequenceNumber_t     seqNum;    /* 消息序列号 */
    GUID_t               stGuid;    /* 实体guid */
    Time_t               timeInfo;  /* 数据时间戳 */
    UINT32               uiDataLen; /* 用户数据长度 */
    CHAR*                data;      /* 用户数据 */

    USHORT               uiFragTotal;   /*分片总数*/
    USHORT               uiFragNum;     /*当前分片数*/
    UINT32               uiFragLen;     /*分片总长度*/
    FragData*            pstFragData;   /*分片数据*/

    struct _HistoryData*  pNext;    /* 下一个节点 */

}HistoryData;

typedef struct HistoryCache
{
    HistoryData* pstHead;
    HistoryData* pstTail;
    UINT32       uiHistoryDataNum;

}HistoryCache;

#define SERIALIZE_HISTORYDATA(historyData, memBlock)                    \
    do                                                                  \
    {                                                                   \
        PUT_UNSIGNED_INT(memBlock, historyData.uiDataLen)               \
        PUT_CHAR_ARRAY(memBlock,historyData.data,historyData.uiDataLen) \
    } while (0);


#define DESERIALIZE_HISTORYDATA(serData, memBlock)                      \
    do                                                                  \
    {                                                                   \
        GET_UNSIGNED_INT(memBlock, historyData.uiDataLen)               \
        GET_CHAR_ARRAY(memBlock,historyData.data,historyData.uiDataLen) \
    } while (0);


/* 初始化历史数据 */
extern VOID InitHistoryData(HistoryData* pstHistoryData);

/* 析构历史数据 */
extern VOID UninitHistoryData(HistoryData* pstHistoryData);

/* 初始化历史缓存数据 */
extern VOID InitHistoryCache(HistoryCache* pstHistoryCache);

/* 析构历史缓存数据 */
extern VOID UninitHistoryCache(HistoryCache* pstHistoryCache);

/* 缓存数据尾插缓存器 */
extern VOID InsertHistoryCacheTail(HistoryCache* pstHistoryCache, HistoryData* pstHistoryData);

/* 按照seqNum顺序大小插入缓存器 */
extern BOOL InsertHistoryCacheOrder(HistoryCache* pstHistoryCache, HistoryData* pstHistoryData);

#endif