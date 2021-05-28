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
*  @file     Monitor.h
*  @brief    Monitor
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

#ifndef _DDS_MONITOR_H_
#define _DDS_MONITOR_H_

/****************************************************************************************************************************************/
//结构体定义
/****************************************************************************************************************************************/

typedef struct _DDS_TopicMon
{
    GUID_t                    tpGuid;
    BOOL                      status;
    struct _DDS_TopicMon*     pNext;

}DDS_TopicMon;

typedef struct _DDS_Monitor
{
    RTPSMessageHeader         stRTPSMsgHeader;
    BoundedString256          userData;
    Locator_t*                unicastLocator;
    DDS_TopicMon*             pstTopicMon;
    time_t                    deadtime;
    struct _DDS_Monitor*      pNext;

}DDS_Monitor;


extern BOOL NetworkByteOrderConvertMonitor(MemoryBlock*  pstMemBlock, DDS_Monitor * pstMonitor);

extern BOOL NetworkByteOrderConvertTopic(MemoryBlock*  pstMemBlock, DDS_TopicMon * pstTopicMon);

extern VOID InitMonitor(DDS_Monitor* pstMonitor);

extern VOID UninitMonitor(DDS_Monitor* pstMonitor);

#endif
