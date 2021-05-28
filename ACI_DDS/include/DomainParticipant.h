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
//�ṹ�嶨��
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
	Duration_t                    livelinessDuration;				//������������ʱ�䣬0Ϊδ���ô����
	Duration_t					  discMinDuration;				//����Զ��participant����С���������
    HighPrecisionTimeBlock        heartBeatMark;
    HighPrecisionTimeBlock        timeTest;
    DDS_DiscoveredParticipant*    pstDiscParticipant;
    SocketList                    socketList;
	DDS_DomainParticipantQos	  pstParticipantQos;

    DDS_Topic*                    pstTopic;               /* ������������Ϣ */
    BuiltinDataWriter             stBuiltinDataWriter[3]; /* 1�������ߣ�2�����ߣ�3������ */
    BuiltinDataReader             stBuiltinDataReader[3]; /* 1�������ߣ�2�����ߣ�3������ */

    DDS_Monitor*                  pstMonitor;
    BOOL                          bLoopBack;

	int							  sleepTime;			//livelinessQoS�¼�������߳�����ʱ��
	int 						  livelinessNum;		//Զ�������˴���Ե�participant�ִ�����

	int							  processId;		//����ID
	EX_UINT32						  ipAddr;			//IP��ַ
    EX_UINT32                       netMask;        //��������

    HANDLE hThread[6];      //�շ������߳�
    BOOL threadSignal;      //�ź���
	DDS_MUTEX sleepMutex;	
    DDS_MUTEX threadMutex;  //�̼߳���
    DDS_MUTEX m_discWtMutex;    //�̼߳���
    DDS_MUTEX m_discRdMutex;    //�̼߳���
    DDS_MUTEX monMutex;     //����̼߳���

}DDS_DomainParticipant;

/****************************************************************************************************************************************/
//��������
/****************************************************************************************************************************************/

/* ��Դ��ʼ�� */
extern DDS_DLL VOID DDS_ParticipantFactory_init();

/* ��ʼ��participant��Ա */
extern VOID InitDomainParticipant(DDS_DomainParticipant* pstDomainParticipant);

/* ͨ��participant��������participant */
extern DDS_DLL DDS_DomainParticipant* DDS_DomainParticipantFactory_create_participant(USHORT domainId, DDS_DomainParticipantQos* pstDomPartQos);

/* �����Ա��ػػ� */
extern DDS_DLL VOID DDS_DomainParticipant_not_ignore_participant(DDS_DomainParticipant* pstDomainParticipant);

/* ���Ա��ػػ� */
extern DDS_DLL VOID DDS_DomainParticipant_ignore_participant(DDS_DomainParticipant* pstDomainParticipant);

/* ͨ��participant����ɾ��participant */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_delete_participant(DDS_DomainParticipant* pstDomainParticipan);

/* ͨ��participant��ȡһ��Ĭ������qos */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipantFactory_get_defaul_participant_qos(DDS_DomainParticipantQos* pstDomPartQos);

/* ͨ��participant��ȡһ��Ĭ������qos */
extern DDS_DLL DDS_DomainParticipantQos* DDS_DomainParticipantFactory_get_defaul_participant_qos_cs();

/* ������Ip */
extern DDS_DLL VOID DDS_Participant_bind_network(DDS_DomainParticipantQos* pstDomPartQos, const CHAR* pcIpAddr);

/* ͨ��participant�������� */
extern DDS_DLL DDS_Topic*  DDS_DomainParticipant_create_topic(DDS_DomainParticipant* pstParticipant, const char* pcTopicName, const char* pcTopicType, DDS_TopicQos* pstTopicQos);

/* ͨ��participant��topic_name��ѯ���� */
extern DDS_DLL DDS_Topic*  DDS_DomainParticipant_get_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType);

/* ��ȡ������� */
extern DDS_DLL UINT32  DDS_DomainParticipant_get_topic_num(DDS_DomainParticipant* pstParticipant);

/* ͨ��participantɾ������ */
extern DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic(DDS_DomainParticipant* pstParticipant, DDS_Topic* pstTopic);

/* ͨ��topicNameɾ������ */
extern DDS_DLL DDS_ReturnCode_t  DDS_DomainParticipant_delete_topic_by_name(DDS_DomainParticipant* pstParticipant, const CHAR* pcTopicName, const CHAR* pcTopicType);

/* ͨ��participant��ȡһ��Ĭ������qos */
extern DDS_DLL DDS_ReturnCode_t DDS_DomainParticipant_get_default_topic_qos(DDS_DomainParticipant* pstParticipant, DDS_TopicQos* pstTopicQos);

/* ͨ��participant��ȡһ��Ĭ������qos */
extern DDS_DLL DDS_TopicQos* DDS_DomainParticipant_get_default_topic_qos_cs(DDS_DomainParticipant* pstParticipant);

/* ͨ��EntityId������Ӧ�ڽ�д���� */
extern BuiltinDataWriter* GetBuiltinWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* ͨ��EntityId������Ӧ�ڽ��Ķ��� */
extern BuiltinDataReader* GetBuiltinReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* ͨ��EntityId������Ӧд���� */
extern DDS_DataWriter* GetLocalWriterFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* ͨ��EntityId������Ӧд���� */
extern DDS_DataReader* GetLocalReaderFromParticipantByEntityId(DDS_DomainParticipant* pstDomainParticipant, EntityId_t* pstEntityId);

/* ͨ��Guid������ӦԶ��д���� */
extern DDS_DiscoveredWriter* GetDiscoveredWriterFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid);

/* ͨ��Guid������ӦԶ���Ķ��� */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromParticipantByGuid(DDS_DomainParticipant* pstDomainParticipant, GUID_t* pstGuid);

/* �����ڽ�Ack���� */
extern BOOL HandleBuiltinAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant);

/* ����Ack���� */
extern BOOL HandleAckNackMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader* pstSubMsgHeader, AckNack* pstAckNackMsg, DDS_DomainParticipant* pstParticipant);

/* �ڽ���������.�Ƿ���Ҫ�����ش� */
extern BOOL HandleBuiltinHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant);

/* ��������.�Ƿ���Ҫ�����ش� */
extern BOOL HandleHeartbeatMsg(RTPSMessageHeader* pstRTPSMsgHeader, Heartbeat* pstHeartbeatMsg, DDS_DomainParticipant* pstParticipant);

/* ���Ͷ�ʱ�������� */
extern VOID SendHeartbeatTasksOrder(DDS_DomainParticipant* pstParticipant);

/* �����������շ����� */
extern VOID NetworkSendReceiveTask(DDS_DomainParticipant* pstParticipant);

/* �����������շ����� */
extern DDS_DLL VOID NetworkTransceiverTask(DDS_DomainParticipant* pstParticipant);

/* ������ɾ��ָ��Զ���ڽ������� */
extern VOID DeleteBuiltinDiscParticipantFromParticipantByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t*  pstPrefix);

/* ɾ��Զ������������Ϣ */
extern VOID DeleteALLDiscoveredFromByGuidPrefix(DDS_DomainParticipant* pstParticipant, GuidPrefix_t stPrefix);

/* ���ؽڵ㷢���û���Ϣ */
extern VOID UserMsgSendMonitor(DDS_DomainParticipant* pstParticipant, GUID_t* tpGuid, MemoryBlock* pstMemBlock, UINT8 type);
 
#endif
