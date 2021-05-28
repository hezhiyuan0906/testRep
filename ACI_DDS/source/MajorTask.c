#include "../Include/GlobalDefine.h"

SubTaskControlBlock g_sub_task_ctrl_blocks[SUB_TASK_COUNT];

/*
对所有的子任务管理结构进行初始化。基于系统配置蓝图，系统中的所有通信关系
都是确定性的，因此可以在每一个设备的系统初始化阶段将所有的任务完成初始化
*/
void InitializeSubTasks()
{
    int sub_task_index;
    for (sub_task_index = 0; sub_task_index<SUB_TASK_COUNT; sub_task_index++)
    {
        g_sub_task_ctrl_blocks[sub_task_index].active = FALSE;
    }
    //对测试任务进行设置
    g_sub_task_ctrl_blocks[SUB_TASK_TEST].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_TEST].miliSecPeriod = 5 * 1000;
    g_sub_task_ctrl_blocks[SUB_TASK_TEST].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_TEST].timeBlock);

    //对SPDP数据发送任务进行配置
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].miliSecPeriod = 1 * 1000;
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].microSecDelayBound = 10 * 1000;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_SEND].timeBlock);

    //对SPDP数据接收任务进行设置
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].miliSecPeriod = 0;
    g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_SPDP_PARTICIPANT_DATA_RECV].timeBlock);

    /* 对SEDP数据发送任务进行设置 */
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_SEND].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_SEND].miliSecPeriod = 0;
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_SEND].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_SEND].timeBlock);

    /* 对SEDP数据接收任务进行设置 */
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].miliSecPeriod = 0;
    g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_SEDP_DATA_RECV].timeBlock);

    /* 对心跳任务进行设置 */
    g_sub_task_ctrl_blocks[SUB_TASK_HEARTBEAT].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_HEARTBEAT].miliSecPeriod = 0;
    g_sub_task_ctrl_blocks[SUB_TASK_HEARTBEAT].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_HEARTBEAT].timeBlock);

    /* 对用户数据接收任务进行设置 */
    g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].active = TRUE;
    g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].miliSecPeriod = 0;
    g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].microSecDelayBound = 0x0fffffff;
    HPTB_INIT(g_sub_task_ctrl_blocks[SUB_TASK_USER_DATA_RECV].timeBlock);

    //根据系统蓝图配置信息对所有的网络发送和接收任务进行设置
    //系统初始化阶段，虽然完成了这些子任务的初始化，但是并没有进入激活状态
    //后续随着业务应用订阅发布接口的调用，网络数据收发子任务将逐步被激活
}

