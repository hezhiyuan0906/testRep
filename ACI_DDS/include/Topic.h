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
*  @file     Topic.h                                                      
*  @brief    topic information
*  Details.                                                                  
*                                                                            
*  @author   Hunter                                                       
*  @email    1028251654@qq.com                                          
*  @version  1.0                                              
*  @date     2019/07/24                                             
*  @license                               
*                                                                            
*--------------------------------------------------------------------------------
*  Remark         : Description                                              
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/07/24 | 1.0       | Hunter    | Create file | 
*--------------------------------------------------------------------------------
*                                                                            
*********************************************************************************/

#ifndef HIPERD_TOPIC_H
#define HIPERD_TOPIC_H

struct _DomainParticipant;
/****************************************************************************************************************************************/
//�ṹ�嶨��
/****************************************************************************************************************************************/

/*������Ϣ*/
typedef struct _Topic
{
    /* ����guid */
    GUID_t                 tpGuid;              
    /* ������ */
    BoundedString256       topicName;
    /* �������� */
    BoundedString256       topicType;
    /* ���������Ƿ�Ϊkey */
    BOOL                   hasKey;                   
    /* ������� */
    DDS_TopicQos           stTopicQos;
    /* Զ��д���� */
    DDS_DiscoveredWriter*  pstDiscWriter;
    /* Զ�˶�ȡ�� */
    DDS_DiscoveredReader*  pstDiscReader;
    /* ����д���� */
    DDS_DataWriter*        pstLocalWriter;
    /* ���ض�ȡ�� */
    DDS_DataReader*        pstLocalReader;
    /* �ύ����û����� */
    HistoryCache           stUserData;
    /* ��ؽڵ� */
    Locator_t*             pstMonLocator;
    /* ���ڵ�Participant */
    struct _DomainParticipant* pstParticipant;

#ifdef EVO_DDS
    INT topicNum;
#endif
    /* �¸��ڵ�ָ�� */
    struct _Topic*      pNext;

}DDS_Topic;

/****************************************************************************************************************************************/
//��������
/****************************************************************************************************************************************/

/* ������Ϣ��ʼ�� */
extern void DDS_Topic_Init(DDS_Topic* pstTopic);

/* ������Ϣ���� */
extern void DDS_Topic_Uninit(DDS_Topic* pstTopic);

/* ��ȡĬ��д����qos */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datawriter_qos(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos);

/* ��ȡĬ��д����qos c# */
extern DDS_DLL DDS_DataWriterQos* DDS_Topic_get_default_datawriter_qos_cs(DDS_Topic* pstTopic);

/* ��ȡĬ���Ķ���qos */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datareader_qos(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos);

/* ��ȡĬ���Ķ���qos c#*/
extern DDS_DLL DDS_DataReaderQos* DDS_Topic_get_default_datareader_qos_cs(DDS_Topic* pstTopic);

/* д�������ÿɿ��� */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_datawriter_qos_set_reliability(DDS_DataWriterQos* pstWriterQos, ReliabilityQosPolicyKind kind);

/* �Ķ������ÿɿ��� */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_datareader_qos_set_reliability(DDS_DataReaderQos* pstReaderQos, ReliabilityQosPolicyKind kind);

/* ���ⴴ��д���� */
extern DDS_DLL DDS_DataWriter* DDS_Topic_create_datawriter(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos, CALLBACKTIME callBackTime);

/* ���ⴴ��д���� */
extern DDS_DLL UINT32 DDS_Topic_get_datawriter_num(DDS_Topic* pstTopic);

/* ���ⴴ�Ķ��� */
extern DDS_DLL DDS_DataReader* DDS_Topic_create_datareader(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos, CALLBACKFUN callBackFun);

/* ����ɾ��д���� */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datawriter(DDS_Topic* pstTopic);

/* ���ⴴ�Ķ��� */
extern DDS_DLL UINT32 DDS_Topic_get_datareader_num(DDS_Topic* pstTopic);

/* ����ɾ��д���� */
extern DDS_DLL DDS_ReturnCode_t DDS_DataWriter_delete_from_topic(DDS_DataWriter*  pstDataWriter);

/* ����ɾ���Ķ��� */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datareader(DDS_Topic* pstTopic);

/* ɾ���Ķ��� */
extern DDS_DLL DDS_ReturnCode_t DDS_DataReader_delete_from_topic(DDS_DataReader*  pstDataReader);

/* ��������ͨ��Guid������ӦԶ��д���� */
extern DDS_DiscoveredWriter* GetDiscoveredWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

/* ��������ͨ��Guid������ӦԶ���Ķ��� */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

/* ��������ͨ��Guid������ӦԶ���Ķ��� */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByPrefix(DDS_Topic* pstTopic, GuidPrefix_t* pstPrefix);

/* ������ɾ��ָ��GuidPrefixԶ��д���� */
extern VOID DeleteDiscWriterFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix);

/* ������ɾ��ָ��GuidPrefixԶ���Ķ��� */
extern VOID DeleteDiscReaderFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix);

/* ������ɾ��ָ��GUIDԶ��д���� */
extern VOID DeleteDiscWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t*  pstGuid);

/* ������ɾ��ָ��GUIDԶ���ֵ��Ķ��� */
extern VOID DeleteTwinDiscReaderFromTopicByGuid(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid);

/* ������ɾ��ָ��GUIDԶ���Ķ��� */
extern VOID DeleteDiscReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

#endif
