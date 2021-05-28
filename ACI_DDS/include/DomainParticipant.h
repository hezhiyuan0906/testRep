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
*  @file     DomainParticipant.h
*  @brief    Participant
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
#ifndef HIPERD_PARTICIPANT_H
#define HIPERD_PARTICIPANT_H

#define NETWORK_BYTE_MAX_SIZE (1024*62)

//#define DomainId unsigned int

extern BOOL g_RapdIOValid;
/****************************************************************************************************************************************/
//结构体定义
/****************************************************************************************************************************************/

typedef struct _SocketList
{
    SocketInfo spdp_data_sender;
    SocketInfo spdp_data_receiver;
    SocketInfo sedp_data_sender;
    SocketInfo sedp_data_receiver;
    SocketInfo user_data_sender;
    SocketInfo user_data_receiver;

}SocketList;

typedef struct _DomainParticipant
{
    RTPSMessageHeader             stRTPSMsgHeader;
    ParticipantData               dcpsParticipantData;
    ParticipantProxy              rtpsParticipantProxy;
	Duration_t                    livelinessDuration;				//自身存活性心跳时间，0为未设置存活性
	Duration_t					  discMinDuration;				//所有远端participant中最小的心跳间隔
    HighPrecisionTimeBlock        heartBeatMark;
    HighPrecisionTimeBlock        timeTest;
    DDS_DiscoveredParticipant*    pstDiscParticipant;
    SocketList                    socketList;
	DDS_DomainParticipantQos	  pstParticipantQos;

    DDS_Topic*                    pstTopic;               /* 创建的主题信息 */
    BuiltinDataWriter             stBuiltinDataWriter[3]; /* 1、参与者；2发布者；3订阅者 */
    BuiltinDataReader             stBuiltinDataReader[3]; /* 1、参与者；2发布者；3订阅者 */

    DDS_Monitor*                  pstMonitor;
    BOOL                          bLoopBack;

	int							  sleepTime;			//livelinessQoS下检测心跳线程休眠时间
	int 						  livelinessNum;		//远端配置了存活性的participant现存数量

	int							  processId;		//进程ID
	EX_UINT32						  ipAddr;			//IP地址
    EX_UINT32                       netMask;        //子网掩码

    HANDLE hThread[6];      //收发任务线程
    BOOL threadSignal;      //信号量
	DDS_MUTEX sleepMutex;	
    DDS_MUTEX threadMutex;  //线程加锁
    DDS_MUTEX m_discWtMutex;    //线程加锁
    DDS_MUTEX m_discRdMutex;    //线程加锁
    DDS_MUTEX monMutex;     //监控线程加锁

}DDS_DomainParticipant;

/****************************************************************************************************************************************/
//函数声明
/****************************************************************************************************************************************/

/* 资源初始化 */
extern DDS_DLL VOID DDS_ParticipantFactory_init();

/* 初始化participant成员 */
extern VOID InitDomainParticipant(DDS_DomainParticipant* pstDomainParticipant);

/* 通过participant工厂创建participant */
extern DDS_DLL DDS_DomainParticipant* DDS_DomainParticipantFactory_create_participant(USHORT domainId, DDS_DomainParticipantQos* pstDomPartQos);

/* 不忽略本地回环 */
extern DDS_DLL VOID DDS_DomainParticipant_not_ignore_participant(DDS_DomainParticipant* pstDomainParticipant);

/* 忽略本地回环 */
extern DDS_DLL VOID DDS_DomainParticipant_ignore_participant(DDS_DomainParticipant* pstDomainParticipant);

/* 通过participant工厂删除participant */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_delete_participant(DDS_DomainParticipant* pstDomainParticipan);

/* 通过participant获取一个默认主题qos */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_get_defaul_participant_qos(DDS_DomainParticipantQos* pstDomPartQos);

/* 通过participant获取一个默认主题qos */
extern DDS_DLL DDS_DomainParticipantQos* DDS_DomainParticipantFactory_get_defaul_participant_qos_cs();

/* 绑定网卡Ip */
extern DDS_DLL VOID DDS_Participant_bind_network(DDS_DomainParticipantQos* pstDomPartQos, const CHAR* pcIpAddr);

/* 通过participant创建主题 */
extern DDS_DLL DDS_Topic*  DDS_DomainParticipant_create_topic(DDS_DomainParticipant* pstParticipant, const char* pcTopicName, const char* pcTopicType, DDS_TopicQos* pstTopicQos);

/* 通过participant和topic_name查询主题 */
extern DDS_DLL DDS_Topic*  DDS_DomainParticipant_get_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType);

/* 获取主题个数 */
extern DDS_DLL UINT32  DDS_DomainParticipant_get_topic_num(DDS_DomainParticipant* pstParticipant);

/* 通过participant删除主题 */
extern DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic(DDS_DomainParticipant* pstParticipant, DDS_Topic* pstTopic);

/* 通过topicName删除主题 */
extern DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType);

/* 通过participant获取一个默认主题qos */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipant_get_default_topic_qos(DDS_DomainParticipant* pstParticipant, DDS_TopicQos* pstTopicQos);

/* 通过participant获取一个默认主题qos */
extern DDS_DLL DDS_TopicQos* DDS_DomainParticipant_get_default_topic_qos_cs(DDS_DomainParticipant* pstParticipant);

/* 通过EntityId查找相应内建写入器 */
extern BuiltinDataWriter* GetBuiltinWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* 通过EntityId查找相应内建阅读器 */
extern BuiltinDataReader* GetBuiltinReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* 通过EntityId查找相应写入器 */
extern DDS_DataWriter* GetLocalWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* 通过EntityId查找相应写入器 */
extern DDS_DataReader* GetLocalReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* 通过Guid查找相应远端写入器 */
extern DDS_DiscoveredWriter* GetDiscoveredWriterFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid);

/* 通过Guid查找相应远端阅读器 */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid);

/* 处理内建Ack报文 */
extern BOOL HandleBuiltinAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant);

/* 处理Ack报文 */
extern BOOL HandleAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant);

/* 内建心跳处理.是否需要数据重传 */
extern BOOL HandleBuiltinHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant);

/* 心跳处理.是否需要数据重传 */
extern BOOL HandleHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant);

/* 发送定时心跳任务 */
extern VOID SendHeartbeatTasksOrder(DDS_DomainParticipant* pstParticipant);

/* 多任务网络收发任务 */
extern VOID NetworkSendReceiveTask(DDS_DomainParticipant* pstParticipant);

/* 单任务网络收发任务 */
extern DDS_DLL VOID NetworkTransceiverTask(DDS_DomainParticipant* pstParticipant);

/* 主题中删除指定远端内建参与者 */
extern VOID DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t*  pstPrefix);

/* 删除远端所有连接信息 */
extern VOID DeleteALLDiscoveredFromByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t stPrefix);

/* 向监控节点发送用户消息 */
extern VOID UserMsgSendMonitor(DDS_DomainParticipant* pstParticipant, GUID_t* tpGuid, MemoryBlock* pstMemBlock, UINT8 type);
 
#endif
