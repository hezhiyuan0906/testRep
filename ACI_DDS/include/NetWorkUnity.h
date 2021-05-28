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
*  @file     NetWorkUnity.h
*  @brief    NetWorkUnity
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
NetWorkUnity.h
单例模式网络工具类，封装常用的网络操作
********************************************************************/
#ifndef HIPERD_NETWORK_UNITY_H
#define HIPERD_NETWORK_UNITY_H

//#define _CRTDBG_MAP_ALLOC
//
//#if defined(_WIN32)
//
//#include <crtdbg.h>
//extern _CrtMemState g_startMemState;
//extern _CrtMemState g_endMemState;
//extern _CrtMemState g_diffMemState;
//#endif

extern CHAR g_ipAddr[32];

#define CHECK_MEMORY()\
    _CrtMemCheckpoint(&g_endMemState);\
    _CrtMemDifference(&g_diffMemState, &g_startMemState,&g_endMemState);\
    _CrtMemDumpStatistics(&g_diffMemState);\
    _CrtDumpMemoryLeaks()

void HiperDGetHostName(BoundedString256 * name);

/* 获取本机的IP地址列表 返回ip个数 */
int getHostIPv4AddrList(BoundedString256 * ipList);

/* 获取本机的Mac地址列表 */
int getHostMacAddrList(BoundedString256 * macList);

/* 点十形式IP变成long型 */
long getLongFromIPAddr(const char* ipAddr);

/********************************************************************/
/* UDP网络初始化相关函数 */
/********************************************************************/

extern BOOL InitSendSocketInfo(SocketInfo* socketInfo, const char *localAddr);

extern BOOL InitUnicastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port, Locator_t* locator);

extern BOOL InitMulticastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port, const char*multicastAddr, Locator_t* locator);

/********************************************************************/
/* 网络数据发送接收相关函数封装 */
/********************************************************************/
/* 发送数据 */
BOOL SendMulticastMsg(MemoryBlock * memBlock, SocketInfo * socketInfo, Locator_t * locator);

/* 接收数据 */
int RTPSReceiveMsg(SocketInfo * locator, char * buffer);

/* 发送数据到单个Locator_t */
BOOL RTPSSendMsg(MemoryBlock * memBlock, SocketInfo * socketInfo, Locator_t * locator);

#endif

