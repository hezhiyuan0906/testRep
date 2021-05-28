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
//结构体定义
/****************************************************************************************************************************************/

/*主题信息*/
typedef struct _Topic
{
    /* 主题guid */
    GUID_t                 tpGuid;              
    /* 主题名 */
    BoundedString256       topicName;
    /* 主题类型 */
    BoundedString256       topicType;
    /* 主题类型是否为key */
    BOOL                   hasKey;                   
    /* 主题策略 */
    DDS_TopicQos           stTopicQos;
    /* 远端写入器 */
    DDS_DiscoveredWriter*  pstDiscWriter;
    /* 远端读取器 */
    DDS_DiscoveredReader*  pstDiscReader;
    /* 本地写入器 */
    DDS_DataWriter*        pstLocalWriter;
    /* 本地读取器 */
    DDS_DataReader*        pstLocalReader;
    /* 提交后的用户数据 */
    HistoryCache           stUserData;
    /* 监控节点 */
    Locator_t*             pstMonLocator;
    /* 父节点Participant */
    struct _DomainParticipant* pstParticipant;

#ifdef EVO_DDS
    INT topicNum;
#endif
    /* 下个节点指针 */
    struct _Topic*      pNext;

}DDS_Topic;

/****************************************************************************************************************************************/
//函数声明
/****************************************************************************************************************************************/

/* 主题信息初始化 */
extern void DDS_Topic_Init(DDS_Topic* pstTopic);

/* 主题信息析构 */
extern void DDS_Topic_Uninit(DDS_Topic* pstTopic);

/* 获取默认写入器qos */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datawriter_qos(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos);

/* 获取默认写入器qos c# */
extern DDS_DLL DDS_DataWriterQos* DDS_Topic_get_default_datawriter_qos_cs(DDS_Topic* pstTopic);

/* 获取默认阅读器qos */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_get_default_datareader_qos(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos);

/* 获取默认阅读器qos c#*/
extern DDS_DLL DDS_DataReaderQos* DDS_Topic_get_default_datareader_qos_cs(DDS_Topic* pstTopic);

/* 写入器设置可靠性 */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_datawriter_qos_set_reliability(DDS_DataWriterQos* pstWriterQos, ReliabilityQosPolicyKind kind);

/* 阅读器设置可靠性 */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_datareader_qos_set_reliability(DDS_DataReaderQos* pstReaderQos, ReliabilityQosPolicyKind kind);

/* 主题创建写入器 */
extern DDS_DLL DDS_DataWriter* DDS_Topic_create_datawriter(DDS_Topic* pstTopic, DDS_DataWriterQos* pstWriterQos, CALLBACKTIME callBackTime);

/* 主题创建写入器 */
extern DDS_DLL UINT32 DDS_Topic_get_datawriter_num(DDS_Topic* pstTopic);

/* 主题创阅读器 */
extern DDS_DLL DDS_DataReader* DDS_Topic_create_datareader(DDS_Topic* pstTopic, DDS_DataReaderQos* pstReaderQos, CALLBACKFUN callBackFun);

/* 主题删除写入器 */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datawriter(DDS_Topic* pstTopic);

/* 主题创阅读器 */
extern DDS_DLL UINT32 DDS_Topic_get_datareader_num(DDS_Topic* pstTopic);

/* 主题删除写入器 */
extern DDS_DLL DDS_ReturnCode_t DDS_DataWriter_delete_from_topic(DDS_DataWriter*  pstDataWriter);

/* 主题删除阅读器 */
extern DDS_DLL DDS_ReturnCode_t DDS_Topic_delete_datareader(DDS_Topic* pstTopic);

/* 删除阅读器 */
extern DDS_DLL DDS_ReturnCode_t DDS_DataReader_delete_from_topic(DDS_DataReader*  pstDataReader);

/* 从主题中通过Guid查找相应远端写入器 */
extern DDS_DiscoveredWriter* GetDiscoveredWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

/* 从主题中通过Guid查找相应远端阅读器 */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

/* 从主题中通过Guid查找相应远端阅读器 */
extern DDS_DiscoveredReader* GetDiscoveredReaderFromTopicByPrefix(DDS_Topic* pstTopic, GuidPrefix_t* pstPrefix);

/* 主题中删除指定GuidPrefix远端写入器 */
extern VOID DeleteDiscWriterFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix);

/* 主题中删除指定GuidPrefix远端阅读器 */
extern VOID DeleteDiscReaderFromTopicByGuidPrefix(DDS_Topic* pstTopic, GuidPrefix_t*  pstPrefix);

/* 主题中删除指定GUID远端写入器 */
extern VOID DeleteDiscWriterFromTopicByGuid(DDS_Topic* pstTopic, GUID_t*  pstGuid);

/* 主题中删除指定GUID远端兄弟阅读器 */
extern VOID DeleteTwinDiscReaderFromTopicByGuid(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid);

/* 主题中删除指定GUID远端阅读器 */
extern VOID DeleteDiscReaderFromTopicByGuid(DDS_Topic* pstTopic, GUID_t* pstGuid);

#endif
