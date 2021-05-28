
#include "../Include/GlobalDefine.h"

#ifdef RAPID_IO_CHANNEL

HANDLE g_hRioDev;
DWORD  g_dwLocalPort;
NodeMapSheet g_sNodeMapSheetWrite;
NodeMapSheet g_sNodeMapSheetRead;
DDS_MUTEX g_rioSendLock[MAX_RIO_DEV_NUM];
int g_rioIsSending[MAX_RIO_DEV_NUM];

/* RapidIO配置数据 */
int RIO_DEV_NUM = 0;/* 打开的设备号，0和1*/
/* 设备位置 */
int RIO_DEVICE_LOCATION = 1;
/* 本地DestID，在RapidIO网络中的设备识别号 */
DWORD RIO_HOST_ID = 1;

/* 任务队列*/
RioDataNode g_rioDataNodeList[MAX_TASKLIST_SIZE];
DDS_MUTEX g_rioDataListLock;
unsigned int g_rioDataListHead = 0;
unsigned int g_rioDataListTail = 0;
unsigned int g_rioDataListSize = 0;
/* 端口位置到设备ID的映射 */
DWORD g_mapLocationToDestID[MAX_RIO_DEV_NUM] = { 0x1e, 0x1d, 0x1c, 0x1b,
                                                0x1a, 0x19, 0x18, 0x00,
                                                0x00, 0x12, 0x13, 0x14,
                                                0xe, 0x15, 0x16, 0x17 };

/* 设备数组，由配置文件配置 */
DWORD RIO_NODE_ARRAY[MAX_RIO_DEV_NUM] = { 0 };
/* 计算设备数量，由配置文件计算出来 */
int NUM_OF_RIO_NODE = 0;

UINT64 g_SendSeqNum = 1;
UINT64 g_RecvSeqNum[16] = { 0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0};
int g_RecvProcessID[16] = { 0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0 }; 
int g_LocalProcessID = 0;
typedef struct _RioMsgHeader
{
    UINT64 s_llDataSeqNum;
    int s_iProcessID;
    int s_iDataLen;
}RioMsgHeader;
int DestIDToLocation(DWORD destid)
{
    for (int i = 0; i < MAX_RIO_DEV_NUM;i++)
    {
        if (destid == g_mapLocationToDestID[i])
        {
            return i + 1;
        }
    }
    return 0;
}
DWORD LocationToDestID(int location)
{
    return g_mapLocationToDestID[location - 1];
}

/************************************************************************/
/*                    偏移计算                                            */
/************************************************************************/
//int NodeMapInitail(NodeMapSheet *nodeSheet, DWORD *destIdArr,  int baseAddr)
//{
//    printf("初始化地址映射:destID baseAddr\n");
//    for (int i = 0; i < MAX_RIO_DEV_NUM; i++)
//    {
//        nodeSheet->m_nodeDestList[i].destId = destIdArr[i];
//        nodeSheet->m_nodeDestList[i].baseAddr = baseAddr + i * DMA_WIN_BUF_SIZE;
//        nodeSheet->m_nodeDestList[i].addrOffset = 0;
//        printf("       %d %x\n", nodeSheet->m_nodeDestList[i].destId, nodeSheet->m_nodeDestList[i].baseAddr);
//    }
//
//    return 1;
//}

int NodeSendMapInitail(NodeMapSheet *nodeSheet)
{
    printf("初始化发送地址映射:destID baseAddr\n");
    for (int i = 0; i < MAX_RIO_DEV_NUM; i++)
    {
        nodeSheet->m_nodeDestList[i].baseAddr = i * INBOOD_DMA_SIZE + (RIO_DEVICE_LOCATION - 1) * DMA_OUTB_BUF_SIZE;
        nodeSheet->m_nodeDestList[i].addrOffset = 0;
        printf("    %d ----- %#x\n",i,  nodeSheet->m_nodeDestList[i].baseAddr);
    }

    return 1;
}
int NodeRecvMapInitail(NodeMapSheet *nodeSheet)
{
    printf("初始化接收地址映射:destID baseAddr\n");
    for (int i = 0; i < MAX_RIO_DEV_NUM; i++)
    {
        nodeSheet->m_nodeDestList[i].baseAddr = /*(RIO_DEVICE_LOCATION - 1) * INBOOD_DMA_SIZE + */i * DMA_OUTB_BUF_SIZE;
        nodeSheet->m_nodeDestList[i].addrOffset = 0;
        printf("    %d ----- %#x\n", i, nodeSheet->m_nodeDestList[i].baseAddr);
    }

    return 1;
}
//int GetTargetOffsetAddr(NodeMapSheet *nodeSheet, const int location, const int dataLen, unsigned int *dmaAddr)
//{
//    int rst = 0;
//    if (nodeSheet->m_nodeDestList[location - 1].addrOffset + dataLen > DMA_WIN_BUF_SIZE)
//    {
//        nodeSheet->m_nodeDestList[location - 1].addrOffset = 0;/* 从头开始 */
//        rst = 2;
//    }
//    else
//    {
//        rst = 1;
//    }
//    *dmaAddr = nodeSheet->m_nodeDestList[location - 1].baseAddr + nodeSheet->m_nodeDestList[location - 1].addrOffset;
//    nodeSheet->m_nodeDestList[location - 1].addrOffset += dataLen;
//    return rst;
//}

int GetTargetBaseAddr(NodeMapSheet *nodeSheet, const int location, unsigned int *dmaAddr)
{
    int rst = 0;
    *dmaAddr = nodeSheet->m_nodeDestList[location - 1].baseAddr;
    rst = 1;
    return rst;
}

/************************************************************************/
/*                    数据队列                                            */
/************************************************************************/
void RioTaskListInit()
{
    g_rioDataListHead = 0;
    g_rioDataListTail = 0;
    g_rioDataListSize = 0;
    for (int i = 0; i < MAX_TASKLIST_SIZE; i++)
    {
        g_rioDataNodeList[i].s_uDataSize = 0;
        g_rioDataNodeList[i].s_pDataBuf = NULL;
    }
    DDS_MUTEX_INIT(g_rioDataListLock);
}
void RioTaskListUninit()
{
    for (int i = 0; i < MAX_TASKLIST_SIZE; i++)
    {
        g_rioDataNodeList[i].s_uDataSize = 0;
        if (g_rioDataNodeList[i].s_pDataBuf != NULL)
        {
            DDS_STATIC_FREE(g_rioDataNodeList[i].s_pDataBuf);
            g_rioDataNodeList[i].s_pDataBuf = NULL;
        }
    }
    DDS_MUTEX_DESTROY(g_rioDataListLock);
}
void InsertRioData(char * f_buf, unsigned int f_dataLen)
{
    DDS_MUTEX_LOCK(g_rioDataListLock);
    if (g_rioDataListSize == MAX_TASKLIST_SIZE)
    {
        //if (g_rioDataListTail == MAX_TASKLIST_SIZE)
        //{
        //    g_rioDataListTail = 0;/* 重头开始插入 */
        //}
        //g_rioDataNodeList[g_rioDataListTail].s_pDataBuf = f_buf;
        //g_rioDataNodeList[g_rioDataListTail].s_uDataSize = f_dataLen;
        ///* 插入完之后，偏移*/
        //g_rioDataListTail++;
        DDS_MUTEX_UNLOCK(g_rioDataListLock);
        printf("队列满，插入失败\n");
        return;
    }
    if (g_rioDataListTail == MAX_TASKLIST_SIZE)
    {
        g_rioDataListTail = 0;/* 重头开始插入 */
    }
    g_rioDataNodeList[g_rioDataListTail].s_pDataBuf = f_buf;
    g_rioDataNodeList[g_rioDataListTail].s_uDataSize = f_dataLen;
    /* 插入完之后，偏移*/
    g_rioDataListSize++;
    g_rioDataListTail++;
    DDS_MUTEX_UNLOCK(g_rioDataListLock);
}
/************************************************************************/
/*        Tsi721RapidIO设置                                             */
/************************************************************************/
int OpenDeveice()
{
    APP_THREAD_PARAM m_param;
    DWORD dwRegVal, dwPortInfo, dwErr;
    memset(&m_param, 0, sizeof(APP_THREAD_PARAM));

    if (TSI721DeviceOpen(&g_hRioDev, RIO_DEV_NUM, &m_param))
    {
        /* 设置本机ID */
        if (ERROR_SUCCESS == TSI721SetLocalHostId(g_hRioDev, RIO_HOST_ID))
        {
            printf_s("SRIO Host ID = 0x%x\n", RIO_HOST_ID);
        }
        else /* 判定是否设置成功 */
        {
            printf("Set Host ID fail.\n");
            return 0;
        }
        dwErr = TSI721RegisterRead(g_hRioDev, RIO_PORT_N_ERR_STAT_CSR, 1, &dwRegVal);

        if (dwErr != ERROR_SUCCESS) {
            printf_s("Read port status failed, err = 0x%x\n", dwErr);

            return 0;
        }

        if(dwRegVal & 0x00010100) {
            printf_s("Port is in error stopped state, status = 0x%08x\n", dwRegVal);
            printf_s("Please reset the board to run this test ...\n");
            return 0;
        }

        if(dwRegVal & 0x02)
            printf_s("Port status OK, SRIO Host ID = 0x%x\n", RIO_HOST_ID);
        else {
            printf_s("Port link status is not OK, status = 0x%08x\n", dwRegVal);
            return 0;
        }
#define RIO_SWITCH_DEVICE_CTL1     0xF2000C
        // reset port when received reset control symbol
        dwErr = TSI721SrioMaintRead(g_hRioDev, 0, 0, RIO_SWITCH_DEVICE_CTL1, &dwRegVal);
        dwErr = TSI721SrioMaintWrite(g_hRioDev, 0, 0, RIO_SWITCH_DEVICE_CTL1, dwRegVal | 0x00000001);

        //
        // Check attached switch device
        //

        // Read device ID register
        dwErr = TSI721SrioMaintRead(g_hRioDev, 0, 0, RIO_DEV_ID_CAR, &dwRegVal);
        if (dwErr != ERROR_SUCCESS) {
            printf_s("(%d) Failed to read switch device ID, err = 0x%x\n", __LINE__, dwErr);
            // Check if SRIO port link is OK
            dwErr = TSI721RegisterRead(g_hRioDev, RIO_PORT_N_ERR_STAT_CSR, 1, &dwRegVal);
            if (dwErr != ERROR_SUCCESS) {
                printf_s("(%d) Read port status failed, err = 0x%x\n", __LINE__, dwErr);
            }
            else {
                printf_s("SRIO Port status = 0x%08x\n", dwRegVal);
            }
            return 0;
        }

        dwErr = TSI721SrioMaintRead(g_hRioDev, 0, 0, RIO_SWITCH_PORT_INF_CAR, &dwPortInfo);

        if (dwErr != ERROR_SUCCESS) {
            printf_s("(%d) Failed to read switch port info CAR, err = 0x%x\n", __LINE__, dwErr);
            return 0;
        }
        g_dwLocalPort = dwPortInfo & 0xff;/* 保存本机对应端口 */
        printf_s("Tsi721 attached to port %d of switch 0x%08x (%d ports)\n",
            dwPortInfo & 0xff, dwRegVal, (dwPortInfo >> 8) & 0xff);
        return 1;
    }
    return 0;
}

int ConfigWin(DWORD addr, DWORD size, int id)
{
    int rst = 0;
    R2P_WINCFG r2pWinCfg;
    DWORD  dwErr;

    r2pWinCfg.BAddrHi = 0;   // Upper part of base address of R2P window
    r2pWinCfg.BAddrLo = addr;   // Lower part of base address of R2P window
    r2pWinCfg.BAddrEx = 0;   // Bits [65:64] of 66-bit base address
    r2pWinCfg.Size = size;  // Size of window in bytes (32KB - 16GB)
    dwErr = TSI721CfgR2pWin(g_hRioDev, id, &r2pWinCfg);
    if (dwErr == ERROR_SUCCESS)
    {
        printf("窗口 %d %#x %#x 配置成功 \n", id, addr, size);
        return 1;
    }
    else
    {
        printf("窗口 %d %#x %#x 配置失败 \n", id, addr, size);
        switch (dwErr)
        {
        case 0x5aa:    //  0x5aa can not be recovered from TSI721FreeR2pWin
            printf_s("(%d) Failed to initialize IB_WIN_%d, err = 0x%x\n",
                __LINE__, id, dwErr);
            break;
        case 0x54f:        // 0x54f can be recovered from TSI721FreeR2pWin
            printf_s("(%d) Failed to initialize IB_WIN_%d, err = 0x%x, try release and reconfig\n",
                __LINE__, id, dwErr);
            dwErr = TSI721FreeR2pWin(g_hRioDev, id);
            if (dwErr != ERROR_SUCCESS)
            {
                printf_s("Failed to release IB_WIN_%d\n", id);
                break;
            }
            else
            {
                dwErr = TSI721CfgR2pWin(g_hRioDev, id, &r2pWinCfg);
                if (dwErr != ERROR_SUCCESS) {
                    printf_s("(%d) second time: Failed to initialize IB_WIN_%d, err = 0x%x\n",
                        __LINE__, id, dwErr);
                    break;;
                }
                else
                {
                    printf_s("second time: Successfully config IB_WIN_%d\n", id);
                    rst = 1;
                }
            }
            break;
        default:  // if has other dwErr, research and implement here
            printf_s("(%d) Failed to initialize IB_WIN_%d, err = 0x%x\n", __LINE__,
                id, dwErr);
        }
        return rst;
    }
}

void ClearDoorBell()
{
    DWORD num = 0;
    IB_DB_ENTRY ibDbBuf;
    DWORD dbSize = sizeof(ibDbBuf);
    if (ERROR_SUCCESS == TSI721SrioDoorbellCheck(g_hRioDev, &num))
    {
        while (num--)
        {
            if (ERROR_SUCCESS == TSI721SrioDoorbellGet(g_hRioDev, &ibDbBuf, &dbSize))
            {
            }
        }
    }
}
/************************************************************************/
/*                    发送锁                                              */
/************************************************************************/
void InitRioSendLock()
{
    for (int i = 0; i < MAX_RIO_DEV_NUM; i++)
    {
        DDS_MUTEX_INIT(g_rioSendLock[i]);
        g_rioIsSending[i] = 0;
    }
}

void RioSendLock(int location)
{
    DDS_MUTEX_LOCK(g_rioSendLock[location - 1]);
}

void RioSendUnLock(DWORD destID)
{
    DDS_MUTEX_UNLOCK(g_rioSendLock[destID - 1]);
}
/************************************************************************/
/*                    读取RapidIO配置                                              */
/************************************************************************/
BOOL InitRioConfig()
{
	g_LocalProcessID = getProcessId();
    if(ReadIniFile("sys.ini"))
    {
		GetIntValue("rapidio", "devNum", &RIO_DEV_NUM);
		GetIntValue("rapidio", "location", &RIO_DEVICE_LOCATION);
		GetIntListValue("rapidio", "destIdList", RIO_NODE_ARRAY, &NUM_OF_RIO_NODE);
		return TRUE;
    }
	return FALSE;
}
/************************************************************************/
/*                    接收数据线程                                        */
/************************************************************************/
VOID data_rw_thread(PVOID params)
{
    HANDLE hDev = (HANDLE)params;
    IB_DB_ENTRY ibDbBuf; /* 门铃缓存 */
    OVERLAPPED ovl;
    memset(&ovl, 0, sizeof(ovl));

    DWORD num = 0, index, dwTemDataSize;
    DWORD dbSize = sizeof(ibDbBuf);
    RioMsgHeader tmpMsgHeader;

    while (1)
    {
        /*if (ERROR_IO_PENDING == TSI721SrioIbDoorbellWait(hDev, &ibDbBuf, sizeof(ibDbBuf), &ulRetSize, &ovl))
        {
        continue;
        }*/
        if (ERROR_SUCCESS == TSI721SrioDoorbellCheck(g_hRioDev, &num))
        {
            for (index = 0; index < num; index++)
            {
                if (ERROR_SUCCESS == TSI721SrioDoorbellGet(g_hRioDev, &ibDbBuf, &dbSize))
                {
                    //printf("接收 %#x 到 %#x 门铃信息：%#x\n", ibDbBuf.db.SrcId, ibDbBuf.db.DstId, ibDbBuf.db.Info);
                    /* 源设备ID */
                    DWORD srcID = (DWORD)ibDbBuf.db.SrcId;
                    // 计算目标位置；
                    unsigned int tmpR2PAddr = 0;
                    if (ibDbBuf.db.Info == DB_TYPE_BACK) /* 数据返回门铃 */
                    {
                        g_rioIsSending[DestIDToLocation(srcID) - 1] = 0;
                    }
                    else if (ibDbBuf.db.Info == DB_TYPE_DATA)
                    {
                        GetTargetBaseAddr(&g_sNodeMapSheetRead, DestIDToLocation(srcID), &tmpR2PAddr);/* 目前发送数据需要等到回应再发，所以不需要计算地址偏移，每次都从基地址收*/
                        dwTemDataSize = sizeof(tmpMsgHeader);
                        if (ERROR_SUCCESS == TSI721IbwBufferGet(hDev, tmpR2PAddr / WIN_BUF_SIZE, tmpR2PAddr % WIN_BUF_SIZE, &tmpMsgHeader, &dwTemDataSize))
                        {
                            if (g_RecvProcessID[DestIDToLocation(srcID) - 1] == 0)
                            {
                                g_RecvProcessID[DestIDToLocation(srcID) - 1] = tmpMsgHeader.s_iProcessID;
                            }
                            else
                            {
                                if (g_RecvProcessID[DestIDToLocation(srcID) - 1] != tmpMsgHeader.s_iProcessID)
                                {
                                    g_RecvSeqNum[DestIDToLocation(srcID) - 1] = 0;
                                    g_RecvProcessID[DestIDToLocation(srcID) - 1] = tmpMsgHeader.s_iProcessID;
                                }
                            }
                            if (tmpMsgHeader.s_llDataSeqNum > g_RecvSeqNum[DestIDToLocation(srcID) - 1])/* 只有序号更大的数据才读取 */
                            {
                                char *recvBuf = DDS_STATIC_MALLOC(tmpMsgHeader.s_iDataLen);
                                if (recvBuf == NULL)
                                {
                                    printf("alloc buf 失败 : %d\n", tmpMsgHeader.s_iDataLen);
                                    break;
                                }
                                dwTemDataSize = tmpMsgHeader.s_iDataLen;
                                if (ERROR_SUCCESS == TSI721IbwBufferGet(hDev, tmpR2PAddr / WIN_BUF_SIZE, tmpR2PAddr % WIN_BUF_SIZE + sizeof(tmpMsgHeader), recvBuf, &dwTemDataSize))
                                {
                                    if (dwTemDataSize != tmpMsgHeader.s_iDataLen)
                                    {
                                        printf("数据接收不完整\n");
                                        break;
                                    }
                                    InsertRioData(recvBuf, (unsigned int)tmpMsgHeader.s_iDataLen);
                                    g_RecvSeqNum[DestIDToLocation(srcID) - 1] = tmpMsgHeader.s_llDataSeqNum;/* 更新接收序号 */
                                }
                            }
                        }
                        /* 返回门铃 */
                        TSI721SrioDoorbellSend(g_hRioDev, srcID, DB_TYPE_BACK);
                    }
                    else
                    {
                        printf("未知门铃\n");
                    }
                }
            }
        }
    }
}
/************************************************************************/
/*                    外部接口                                            */
/************************************************************************/
//对任务执行进行管理的时标结构
HighPrecisionTimeBlock timeBlock;
/* RapidIO网络初始化 */
int InitRioNetWork()
{
    HPTB_INIT(timeBlock);
	if (!InitRioConfig())
	{
		return 0;
	}
    RioTaskListInit();
    RIO_HOST_ID = g_mapLocationToDestID[RIO_DEVICE_LOCATION - 1];
    printf("-----------本机设备ID：%#x\n", RIO_HOST_ID);

    /* 初始化地址,写地址的初始地址为SRIO地址，读地址初始地址为R2P地址 */
    NodeSendMapInitail(&g_sNodeMapSheetWrite);
    NodeRecvMapInitail(&g_sNodeMapSheetRead);

    /* 打开设备 */
    if (!OpenDeveice())
    {
        printf("打开rapidio设备失败\n");
        return 0;
    }
    else
    {
        for (int i = 0; i < MAX_RIO_WIN_NUM; i++)
        {
            /* 配置接收窗口 */
            if (!ConfigWin((RIO_DEVICE_LOCATION - 1) * INBOOD_DMA_SIZE + i * WIN_BUF_SIZE, WIN_BUF_SIZE, i))/* to do 窗口的配置方式和注意事项需要确认 */
            {
                printf("配置rapidio映射窗口0失败，关闭程序重新开始配置\n");
                return 0;
            }
        }
    }
    /* 清门铃 */
    ClearDoorBell();
    /* 启动接收线程 */
    HANDLE hThread = (HANDLE)_beginthread(data_rw_thread, 0, (PVOID)g_hRioDev);
    /* 初始化发送锁 */
    InitRioSendLock();
    /* 初始化发送缓存 */
    RioSendBufInitial();
    return 1;
}
int ExitRioNetWork()
{
    /* 释放窗口，卸载设备 */
    if (g_hRioDev)
    {
        for (int index = 0; index < MAX_RIO_WIN_NUM; index++)
        {
            TSI721FreeR2pWin(g_hRioDev, index);
        }
        APP_THREAD_PARAM  appThreadParam;
        TSI721DeviceClose(g_hRioDev, &appThreadParam);
        g_hRioDev = NULL;
    }
    /* 释放任务队列 */
    RioTaskListUninit();
    RioSendBufUnInitial();
    return 1;
}
void MulticastSend(char *f_dataBuf, int f_iDataSize)
{
    for (int i = 1; i <= NUM_OF_RIO_NODE; i++)
    {
        if (i == RIO_DEVICE_LOCATION)
        {
            continue;
        }
        UnicastSend(RIO_NODE_ARRAY[i], f_dataBuf, f_iDataSize);
    }
}

/* 初始化发送缓存 */
char * g_SendBufList[16];
void RioSendBufInitial()
{
    for (int i = 0; i < MAX_RIO_DEV_NUM;i++)
    {
        if (i != 7 && i != 8)
        {
            g_SendBufList[i] = (char *)DDS_STATIC_MALLOC(DMA_OUTB_BUF_SIZE);
        }
    }
}
void RioSendBufUnInitial()
{
    for (int i = 0; i < MAX_RIO_DEV_NUM; i++)
    {
        if (i != 7 && i != 8)
        {
            DDS_STATIC_FREE(g_SendBufList[i]);
        }
    }
}
/* RapidIO发送数据 */
int UnicastSend(int location, char *f_dataBuf, int f_iDataSize)
{
    if (location == RIO_DEVICE_LOCATION)
    {
        printf("暂时不支持往自己发数据，路由未配置。\n");
        return 0;
    }
    if (f_iDataSize > DMA_OUTB_BUF_SIZE - sizeof(g_SendSeqNum))
    {
        printf("数据包过长，最大支持 %d 字节。\n", DMA_OUTB_BUF_SIZE);
        return 0;
    }
    RioSendLock(location);
    g_rioIsSending[location - 1] = 1;

    /* 计算目标位置 */
    int tmpSrioAddr = 0;
    int iRst = GetTargetBaseAddr(&g_sNodeMapSheetWrite, location, &tmpSrioAddr);/* 目前发送数据需要等到回应再发，所以不需要计算地址偏移，每次都从基地址发*/
    if (iRst == 0)
    {
        return 0;
    }

    DWORD status;
    DWORD dwDataSize;
    DMA_REQ_CTRL dmaCtrl;
    dmaCtrl.bits.Iof = 0;
    dmaCtrl.bits.Crf = 0;
    dmaCtrl.bits.Prio = 0;
    dmaCtrl.bits.Rtype = ALL_NWRITE;// ALL_NWRITE;//LAST_NWRITE_R; // Last packet NWRITE_R, all other NWRITE or SWRITE
    dmaCtrl.bits.XAddr = 0; // bits 65:64 of SRIO address
    RioMsgHeader tmpMsgHeader;
    tmpMsgHeader.s_llDataSeqNum = g_SendSeqNum;
    tmpMsgHeader.s_iProcessID = g_LocalProcessID;
    tmpMsgHeader.s_iDataLen = f_iDataSize;
    memcpy(g_SendBufList[location - 1], &tmpMsgHeader, sizeof(tmpMsgHeader));
    memcpy(g_SendBufList[location - 1] + sizeof(tmpMsgHeader), f_dataBuf, f_iDataSize);
    dwDataSize = sizeof(tmpMsgHeader) + f_iDataSize;
    //printf("before srio write ");
    status = TSI721SrioWrite(g_hRioDev, LocationToDestID(location), 0, tmpSrioAddr, g_SendBufList[location - 1], &dwDataSize, dmaCtrl);
    //printf("after srio write \n");
    //printf("[DEBUG] : 往节点%d %#x --- %#x 发送数据 %d\n", location, LocationToDestID(location), tmpSrioAddr, dwDataSize);
    if (ERROR_SUCCESS == status)
    {
        TSI721SrioDoorbellSend(g_hRioDev, LocationToDestID(location), DB_TYPE_DATA);/* 高15位*/
        g_SendSeqNum++;/* 发送之后增加序号 */
        //等待
        ULONG microSecInterval;
        //重新开始计时
        HPTB_BEGIN(timeBlock);
        while (g_rioIsSending[location - 1] != 0)
        {
            HPTB_END(timeBlock);
            microSecInterval = HPTB_INTERVAL_MICROSEC(timeBlock);
            if (microSecInterval > 10 * 1000)
            {
                printf("等待回复超时 .\n");
                RioSendUnLock(location);
                return 0;
            }    
        }
        RioSendUnLock(location);
        return 1;
    }
    else
    {
        //printf("TSI721SrioWrite error status = %08x\n", status);
        RioSendUnLock(location);
        return 0;
    }
}


/* RapidIO接收数据 */
unsigned int GetRioData(char *f_recvBuf)
{
    DDS_MUTEX_LOCK(g_rioDataListLock);
    unsigned int rst = 0;
    if (g_rioDataListSize == 0)
    {
        DDS_MUTEX_UNLOCK(g_rioDataListLock);
        return rst;/* 没有数据 */
    }
    else
    {
        if (g_rioDataListHead == MAX_TASKLIST_SIZE)
        {
            g_rioDataListHead = 0;/* 重头开始读取 */
        }
        memcpy(f_recvBuf, g_rioDataNodeList[g_rioDataListHead].s_pDataBuf, g_rioDataNodeList[g_rioDataListHead].s_uDataSize);
        rst = g_rioDataNodeList[g_rioDataListHead].s_uDataSize;
        /* 取完数据之后，初始化*/
        if (NULL != g_rioDataNodeList[g_rioDataListHead].s_pDataBuf)
        {
            DDS_STATIC_FREE(g_rioDataNodeList[g_rioDataListHead].s_pDataBuf);
            g_rioDataNodeList[g_rioDataListHead].s_pDataBuf = NULL;
        }
        g_rioDataNodeList[g_rioDataListHead].s_uDataSize = 0;
        g_rioDataListHead++;
        g_rioDataListSize--;
        DDS_MUTEX_UNLOCK(g_rioDataListLock);
        return rst;
    }
}

#endif