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
*  @file     GlobalDefine.h
*  @brief    GlobalDefine
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
  该文件定义了整个HiperD的代码架构，主要包括如下内容：
  1）对整个工程有效的编译选项
  2）罗列出所有的头文件并给出头文件在架构中的功能
********************************************************************/
#pragma warning(disable:4996)
#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_

//内存泄漏测试
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

//#define USE_STATIC_MALLOC     //静态内存
//#define USE_STATIC_MALLOC_T   //用户静态内存

//#define RAPID_IO_CHANNEL
//#define DDS_MONITOR
#define _DDS_DLLEXPORT
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//软无平台特殊版本，可针对相同topicName多次创建topic，每次删除会检查topicNum，直到为0才真正删除
//#define EVO_DDS

/*QoS相关宏*/
/*目前QoS收发配置关系：
RELIABILITY：当收设置为BEST_EFFORT则为不可靠，都设置为RELIABLE则为可靠，若收为RELIABLE而发为BEST_EFFORT则不匹配无法接收
DURABILITY：只需要发端设置TRANSIENT即视为持久化，需要收发同时配置RELIABLE才能生效
LIVELINESS：写在Participant中，目前只支持Participant的存活性
TIME_BASED_FILTER：只在收端配置
DEADLINE：只有发端配置有用
HISTORY：只有发端配置有用*/
//#define QYDDS_DURABILITY_QOS
//#define QYDDS_TIME_BASED_FILTER_QOS
#define QYDDS_LIVELINESS_QOS
//#define QYDDS_DEADLINE_QOS
#define QYDDS_HISTORY_QOS

/********************************************************************
    在此处设置平台和操作系统选项
********************************************************************/

/********************************************************************
 在此处给出需要的所有系统头文件
********************************************************************/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <io.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <direct.h>
#include <process.h>
#include <time.h>
#include <shlwapi.h>
#include <assert.h>
#include <Iphlpapi.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <fcntl.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
#else 

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#endif


/********************************************************************
    X86平台WIN32环境，对应常规的windows
********************************************************************/
#if defined(_WIN32) || defined(_WIN64)
    #if defined(_DDS_DLLEXPORT)
        #define DDS_DLL __declspec(dllexport)
    #elif defined(_HIPERD_DLLIMPORT)
        #define HIPERD_DLL __declspec(dllimport)
    #else
        #define HIPERD_DLL
    #endif
#endif


/********************************************************************
    其它系统平台的环境配置
********************************************************************/


//定义系统内核工具，例如互斥锁、信号量等
#if defined(_WIN32)
    #define STACALL __stdcall
    //互斥锁资源
    #define DDS_MUTEX HANDLE
    #define DDS_SEMA HANDLE
    #define DDS_MUTEX_LOCK(mutex) WaitForSingleObject(mutex, INFINITE)
    #define DDS_MUTEX_UNLOCK(mutex) ReleaseMutex(mutex)
    #define DDS_MUTEX_INIT(mutex) mutex = CreateMutex(0, FALSE, 0)
    #define DDS_MUTEX_DESTROY(mutex) \
            {\
            CloseHandle(mutex);\
            mutex = NULL;\
        }
    //信号量资源
    #define DDS_SEMA_WAIT_TIME(sema,delay) WaitForSingleObject(sema, delay)
    #define DDS_SEMA_WAIT(sema) WaitForSingleObject(sema, INFINITE)
    #define DDS_SEMA_POST(sema) ReleaseSemaphore(sema, 1, NULL)
    #define DDS_SEMA_TRYWAIT(sema) WaitForSingleObject(sema, 0)
    #define DDS_SEMA_DESTROY(sema) \
            {\
            CloseHandle(sema);\
            sema = NULL;\
        }
    #define DDS_SEMA_INIT(sema, initCount, maxCount) sema = CreateSemaphore(NULL, initCount, maxCount, NULL)
    //线程资源
    typedef  unsigned int DDS_THREAD_ID;
    typedef HANDLE    DDS_THREAD_HANDLE;
            
#endif

//定义系统相关的数据类型
#ifndef VOID
typedef  void VOID;
typedef  char BOOL;
#endif
typedef  float FLOAT32;
typedef  double DOUBLE64;
typedef  unsigned long ULONG;
typedef  unsigned char UCHAR;
typedef  char CHAR;
typedef  char NETBYTE;
typedef  short SHORT;
typedef  unsigned short USHORT;
typedef  int INT;
typedef  unsigned int UINT32;
typedef  long long INT64;
typedef  unsigned long long UINT64;
typedef  float FLOAT;
typedef  double DOUBLE;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

/********************************************************************
 以下是该项目相关的所有头文件及其功能描述
********************************************************************/

//类型映射，屏蔽平台类型差异
#include "CommonTypeDefine.h"

/* ProcessUnity.h */
#include "ProcessUnity.h"

/* MemoryBlock结构体相关定义及操作 */
#include "MemoryBlock.h"

//OMG对IDL的CDR编解码相关机制
#include "CDRTool.h"

#include "ConfFile.h"

//RTPS规范中Duration_t对应的数据类型及相关处理机制
#include "Duration_t.h"

//RTPS规范中Guid_t对应的数据类型及相关处理机制
#include "GUID_t.h"

#include "ReturnCode.h"

//高精度时间相关的数据结构和处理机制
#include "TimeTool.h"

/* 分片数据号 */
#include "FragmentNumber.h"

//64比特的序列号
#include "SequenceNumber_t.h"

/* SequenceNumber集合结构体定义 */
#include "SequenceNumberSet.h"

/* 历史数据相数据类型定义及相关操作 */
#include "HistoryCache.h"

//定义了多个干固定最大长度的字符串工具结构
#include "BoundedString.h"

//RTPS规范中BuiltinTopicKey_t对应的数据类型及相关处理机制
#include "BuiltinTopicKey_t.h"

//DCPS规范中UserDataQosPolicy对应的数据类型及相关处理机制
#include "UserDataQosPolicy.h"

//存储字符串化名值对的列表结构
#include "PropertyList.h"

//表示RTPS协议中的参数以及参数列表结构体ParameterList及其相关操作机制
#include "ParameterList.h"

//RTPS规范中BuiltinEndpointSet_t对应的数据类型及相关处理机制
#include "BuiltinEndpointSet_t.h"

//规范中Locator_t对应的数据类型及相关处理机制
#include "Locator_t.h"

//RTPS规范中ProtocolVersion_t对应的数据类型及相关处理机制
#include "ProtocolVersion_t.h"

//RTPS规范中VendorId_t对应的数据类型及相关处理机制
#include "VendorId_t.h"

/* QosPolicy.h */
#include "QosPolicy.h"

/* QosService.h */
#include "QosService.h"

//#include "Configure.h"

//RTPS规范中所有消息的Header结构
#include "RTPSMessageHeader.h"

#include "RTPSDiscovery.h"

//RTPS子消息头，标明子消息类型标志及长度
#include "SubMessageHeader.h"

//RTPS的Discovery模块中用于SPDP的ParticipantData结构
#include "ParticipantData.h"

//远程的Participant进行SPDP和SEDP交互的必要信息，例如网络地址和端口
#include "ParticipantProxy.h"

#include "SerializedData.h"

/* 报文消息头数据结构及其相关操作处理机制 */
#include "Data.h"

/* 用于生成GUID */
#include "GuidGen.h"

/* 单例模式网络工具类，封装常用的网络操作 */
#include "NetWorkUnity.h"

/* 封装与字符串有关的操作 */
#include "StringUnity.h"

/* AckNack子消息结构体及其相关编解码操作 */
#include "AckNack.h"

/* Heartbeat子消息结构体及其相关编解码操作 */
#include "Heartbeat.h"

/* 不相交的SequenceNumber_t区间集合 */
//#include "DisjointSequence.h"

#include "MajorTask.h"

/* 存储某条链路的分片收到的信息 */
//#include "RTPSFragInfo.h"

/* DataFrag子报文 */
#include "DataFrag.h"

/* InfoTimestamp子报文 */
#include "InfoTimestamp.h"

/* InfoDestination 子报文 */
#include "InfoDestination.h"

/* InfoReply 子报文 */
#include "InfoReply.h"

/* gap子报文*/
#include "Gap.h"

/*日志模块*/
#include "CommLog.h"

/*链表操作*/
#include "ListOper.h"

/* 静态内存管理 */
#include "MemoryAlloc.h"

/* 远端写入器 */
#include "DiscoveredWriter.h"

/* 远端读取器 */
#include "DiscoveredReader.h"

/* 数据写入器 */
#include "DataWriter.h"

/* 数据读取器*/
#include "DataReader.h"

/* 主题 */
#include "Topic.h"

/* 发现参与者 */
#include "DiscoveredParticipant.h"

/* 数据写入器 */
#include "BuiltinDataWriter.h"

/* 数据读取器*/
#include "BuiltinDataReader.h"

/* 监控配置项 */
#include "Monitor.h"

/* 参与者 */
#include "DomainParticipant.h"

#ifdef RAPID_IO_CHANNEL
#include "RapidIO/tsi721api.h"
#include "RapidIO/tsi721ioctl.h"
#include "RapidIO/RioUtility.h"
#endif

/* 网络初始化 */
#include "NetWorkStructure.h"

/* 网络字节序解析 */
#include "NetWorkOrder.h"

/* SPDP报文处理模块 */
#include "Spdp.h"

/* SEDP处理模块 */
#include "Sedp.h"

/* 用户收发数据 */
#include "User.h"

/* 处理除了SPDP发现报文之外的所有报文 */
 #include "RTPSReceiveStrategy.h"

#include "Face/TypeAbstraction_TS.h"

#endif



