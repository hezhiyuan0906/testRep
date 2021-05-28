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
*  @file     VendorId_t.h
*  @brief    VendorId_t
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
VendorId_t.h
RTPS规范中VendorId_t对应的数据类型及相关处理机制
********************************************************************/

#ifndef HIPERD_MAJORTASK_H
#define HIPERD_MAJORTASK_H

/*
主任务的工作状态，包括各种可能的出错情况
*/
typedef enum MainTaskState
{
    MTS_IDLE = 0,
    MTS_TO_EXIT = 1,
    MTS_SEND_SPDP = 2,
    MTS_RECV_SPDP = 3,
}MainTaskState;

typedef enum SUB_TASK_ID
{
    SUB_TASK_INVALID = -1,
    SUB_TASK_TEST = 0,
    SUB_TASK_SPDP_PARTICIPANT_DATA_SEND = 1,
    SUB_TASK_SPDP_PARTICIPANT_DATA_RECV = 2,
    SUB_TASK_SEDP_DATA_SEND = 3,
    SUB_TASK_SEDP_DATA_RECV = 4,
    SUB_TASK_HEARTBEAT = 5,
    SUB_TASK_USER_DATA_RECV = 6,
    SUB_TASK_COUNT = 32,
}SUB_TASK_ID;

/*
主任务管理结构
在主任务中循环执行各种子任务
*/
typedef struct MainTaskControlBlock
{
    DDS_THREAD_ID taskID;
    DDS_THREAD_HANDLE taskHandle;
    MainTaskState taskState;
    char udpRecvBuf[MAX_UDP_MSG_SIZE];
}MainTaskControlBlock;


/*
子任务管理结构
*/
typedef struct SubTaskControlBlock
{
    //该子任务是否被执行
    BOOL active;

    //对任务执行进行管理的时标结构
    HighPrecisionTimeBlock timeBlock;

    //以毫秒为单位的周期
    unsigned long miliSecPeriod;

    //以微秒为单位的延迟上限，硬实时任务若超过延迟上限则失败
    unsigned long microSecDelayBound;

}SubTaskControlBlock;

extern SubTaskControlBlock g_sub_task_ctrl_blocks[SUB_TASK_COUNT];

extern void InitializeSubTasks();

#endif