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
*  @file     DiscoveredReader.h
*  @brief    DiscoveredReader
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

#ifndef HIPERD_DISCOVEREDREADER_H
#define HIPERD_DISCOVEREDREADER_H

struct _Topic;

typedef struct _LocalWriter
{
    GUID_t                      guid;
    AckNack                     stAckNack;      /* 远端阅读器回复本地写入器Acknack报文 */
    UINT32                      uiSendHeartBeatNum;    /* 最后回复ack截至后发送心跳个数，超过规定值，失去连接，删除该远端阅读器 */
    struct _LocalWriter*        pNext;

}DDS_LocalWriter;

typedef struct _DiscoveredReader
{
    BOOL                        bVaild;           /* 对端内建是否也已建立连接，通过对端回复的ack报文确认 */
    GUID_t                      guid;
    Locator_t*                  pstUnicastLocator;
    DDS_DataReaderQos           stDataReaderQos;
    DDS_LocalWriter*            pstLocalWriter;

    struct _Topic*              pstTopic;

    struct _DiscoveredReader*   pstTwinDiscReader;

    struct _DiscoveredReader*   pNext;

}DDS_DiscoveredReader;

/* 初始化远端阅读器 */
extern VOID DDS_DiscoveredReader_Init(DDS_DiscoveredReader * pstDiscReader);

/* 去始化远端阅读器 */
extern VOID DDS_DiscoveredReader_Uninit(DDS_DiscoveredReader* pstDiscReader);

/* 添加本地写入器 */
extern BOOL DiscoveredReaderAddLocalWriter(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid);

/* 删除本地写入器 */
extern BOOL DiscoveredReaderDelLocalWriter(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid);

/* 兄弟写入器置顶 */
extern VOID TwinDiscoveredReaderChangePosition(DDS_DiscoveredReader* pstDiscReader);

/* 获取本地写入器 */
extern DDS_LocalWriter*  GetDiscoveredReaderLocalWriterByGuid(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid);

#endif
