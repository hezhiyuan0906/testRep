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
  ���ļ�����������HiperD�Ĵ���ܹ�����Ҫ�����������ݣ�
  1��������������Ч�ı���ѡ��
  2�����г����е�ͷ�ļ�������ͷ�ļ��ڼܹ��еĹ���
********************************************************************/
#pragma warning(disable:4996)
#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_

//�ڴ�й©����
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

//#define USE_STATIC_MALLOC     //��̬�ڴ�
//#define USE_STATIC_MALLOC_T   //�û���̬�ڴ�

//#define RAPID_IO_CHANNEL
//#define DDS_MONITOR
#define _DDS_DLLEXPORT
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//����ƽ̨����汾���������ͬtopicName��δ���topic��ÿ��ɾ������topicNum��ֱ��Ϊ0������ɾ��
//#define EVO_DDS

/*QoS��غ�*/
/*ĿǰQoS�շ����ù�ϵ��
RELIABILITY����������ΪBEST_EFFORT��Ϊ���ɿ���������ΪRELIABLE��Ϊ�ɿ�������ΪRELIABLE����ΪBEST_EFFORT��ƥ���޷�����
DURABILITY��ֻ��Ҫ��������TRANSIENT����Ϊ�־û�����Ҫ�շ�ͬʱ����RELIABLE������Ч
LIVELINESS��д��Participant�У�Ŀǰֻ֧��Participant�Ĵ����
TIME_BASED_FILTER��ֻ���ն�����
DEADLINE��ֻ�з�����������
HISTORY��ֻ�з�����������*/
//#define QYDDS_DURABILITY_QOS
//#define QYDDS_TIME_BASED_FILTER_QOS
#define QYDDS_LIVELINESS_QOS
//#define QYDDS_DEADLINE_QOS
#define QYDDS_HISTORY_QOS

/********************************************************************
    �ڴ˴�����ƽ̨�Ͳ���ϵͳѡ��
********************************************************************/

/********************************************************************
 �ڴ˴�������Ҫ������ϵͳͷ�ļ�
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
    X86ƽ̨WIN32��������Ӧ�����windows
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
    ����ϵͳƽ̨�Ļ�������
********************************************************************/


//����ϵͳ�ں˹��ߣ����绥�������ź�����
#if defined(_WIN32)
    #define STACALL __stdcall
    //��������Դ
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
    //�ź�����Դ
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
    //�߳���Դ
    typedef  unsigned int DDS_THREAD_ID;
    typedef HANDLE    DDS_THREAD_HANDLE;
            
#endif

//����ϵͳ��ص���������
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
 �����Ǹ���Ŀ��ص�����ͷ�ļ����书������
********************************************************************/

//����ӳ�䣬����ƽ̨���Ͳ���
#include "CommonTypeDefine.h"

/* ProcessUnity.h */
#include "ProcessUnity.h"

/* MemoryBlock�ṹ����ض��弰���� */
#include "MemoryBlock.h"

//OMG��IDL��CDR�������ػ���
#include "CDRTool.h"

#include "ConfFile.h"

//RTPS�淶��Duration_t��Ӧ���������ͼ���ش������
#include "Duration_t.h"

//RTPS�淶��Guid_t��Ӧ���������ͼ���ش������
#include "GUID_t.h"

#include "ReturnCode.h"

//�߾���ʱ����ص����ݽṹ�ʹ������
#include "TimeTool.h"

/* ��Ƭ���ݺ� */
#include "FragmentNumber.h"

//64���ص����к�
#include "SequenceNumber_t.h"

/* SequenceNumber���Ͻṹ�嶨�� */
#include "SequenceNumberSet.h"

/* ��ʷ�������������Ͷ��弰��ز��� */
#include "HistoryCache.h"

//�����˶���ɹ̶���󳤶ȵ��ַ������߽ṹ
#include "BoundedString.h"

//RTPS�淶��BuiltinTopicKey_t��Ӧ���������ͼ���ش������
#include "BuiltinTopicKey_t.h"

//DCPS�淶��UserDataQosPolicy��Ӧ���������ͼ���ش������
#include "UserDataQosPolicy.h"

//�洢�ַ�������ֵ�Ե��б�ṹ
#include "PropertyList.h"

//��ʾRTPSЭ���еĲ����Լ������б�ṹ��ParameterList������ز�������
#include "ParameterList.h"

//RTPS�淶��BuiltinEndpointSet_t��Ӧ���������ͼ���ش������
#include "BuiltinEndpointSet_t.h"

//�淶��Locator_t��Ӧ���������ͼ���ش������
#include "Locator_t.h"

//RTPS�淶��ProtocolVersion_t��Ӧ���������ͼ���ش������
#include "ProtocolVersion_t.h"

//RTPS�淶��VendorId_t��Ӧ���������ͼ���ش������
#include "VendorId_t.h"

/* QosPolicy.h */
#include "QosPolicy.h"

/* QosService.h */
#include "QosService.h"

//#include "Configure.h"

//RTPS�淶��������Ϣ��Header�ṹ
#include "RTPSMessageHeader.h"

#include "RTPSDiscovery.h"

//RTPS����Ϣͷ����������Ϣ���ͱ�־������
#include "SubMessageHeader.h"

//RTPS��Discoveryģ��������SPDP��ParticipantData�ṹ
#include "ParticipantData.h"

//Զ�̵�Participant����SPDP��SEDP�����ı�Ҫ��Ϣ�����������ַ�Ͷ˿�
#include "ParticipantProxy.h"

#include "SerializedData.h"

/* ������Ϣͷ���ݽṹ������ز���������� */
#include "Data.h"

/* ��������GUID */
#include "GuidGen.h"

/* ����ģʽ���繤���࣬��װ���õ�������� */
#include "NetWorkUnity.h"

/* ��װ���ַ����йصĲ��� */
#include "StringUnity.h"

/* AckNack����Ϣ�ṹ�弰����ر������� */
#include "AckNack.h"

/* Heartbeat����Ϣ�ṹ�弰����ر������� */
#include "Heartbeat.h"

/* ���ཻ��SequenceNumber_t���伯�� */
//#include "DisjointSequence.h"

#include "MajorTask.h"

/* �洢ĳ����·�ķ�Ƭ�յ�����Ϣ */
//#include "RTPSFragInfo.h"

/* DataFrag�ӱ��� */
#include "DataFrag.h"

/* InfoTimestamp�ӱ��� */
#include "InfoTimestamp.h"

/* InfoDestination �ӱ��� */
#include "InfoDestination.h"

/* InfoReply �ӱ��� */
#include "InfoReply.h"

/* gap�ӱ���*/
#include "Gap.h"

/*��־ģ��*/
#include "CommLog.h"

/*�������*/
#include "ListOper.h"

/* ��̬�ڴ���� */
#include "MemoryAlloc.h"

/* Զ��д���� */
#include "DiscoveredWriter.h"

/* Զ�˶�ȡ�� */
#include "DiscoveredReader.h"

/* ����д���� */
#include "DataWriter.h"

/* ���ݶ�ȡ��*/
#include "DataReader.h"

/* ���� */
#include "Topic.h"

/* ���ֲ����� */
#include "DiscoveredParticipant.h"

/* ����д���� */
#include "BuiltinDataWriter.h"

/* ���ݶ�ȡ��*/
#include "BuiltinDataReader.h"

/* ��������� */
#include "Monitor.h"

/* ������ */
#include "DomainParticipant.h"

#ifdef RAPID_IO_CHANNEL
#include "RapidIO/tsi721api.h"
#include "RapidIO/tsi721ioctl.h"
#include "RapidIO/RioUtility.h"
#endif

/* �����ʼ�� */
#include "NetWorkStructure.h"

/* �����ֽ������ */
#include "NetWorkOrder.h"

/* SPDP���Ĵ���ģ�� */
#include "Spdp.h"

/* SEDP����ģ�� */
#include "Sedp.h"

/* �û��շ����� */
#include "User.h"

/* �������SPDP���ֱ���֮������б��� */
 #include "RTPSReceiveStrategy.h"

#include "Face/TypeAbstraction_TS.h"

#endif



