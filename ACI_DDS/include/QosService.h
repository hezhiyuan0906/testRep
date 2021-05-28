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
*  @file     QosService.h
*  @brief    QosService
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
QosService.h
用来保存系统中的各种Qos及提供默认Qos支持
********************************************************************/


#ifndef HIPERD_QOSSERVICE_H
#define HIPERD_QOSSERVICE_H

typedef struct _InlineQos
{
    GUID_t guid;
    UINT32 disposed;

}InlineQos;

typedef struct _QosService
{
    /* 通用默认Qos */
    UserDataQosPolicy              defaultUserDataQosPolicy;
    TopicDataQosPolicy             defaultTopicDataQosPolicy;
    GroupDataQosPolicy             defaultGroupDataQosPolicy;
    TransportPriorityQosPolicy     defaultTransportPriorityQosPolicy;
    LifespanQosPolicy              defaultLifespanQosPolicy;
    DurabilityQosPolicy            defaultDurabilityQosPolicy;
    DurabilityServiceQosPolicy     defaultDurabilityServiceQosPolicy;
    PresentationQosPolicy          defaultPresentationQosPolicy;
    DeadlineQosPolicy              defaultDeadlineQosPolicy;
    LatencyBudgetQosPolicy         defaultLatencyBudgetQosPolicy;
    OwnershipQosPolicy             defaultOwnershipQosPolicy;
    OwnershipStrengthQosPolicy     defaultOwnershipStrengthQosPolicy;
    LivelinessQosPolicy            defaultLivelinessQosPolicy;
    TimeBasedFilterQosPolicy       defaultTimeBasedFilterQosPolicy;
    PartitionQosPolicy             defaultPartitionQosPolicy;
    ReliabilityQosPolicy           defaultReliabilityQosPolicy;
    DestinationOrderQosPolicy      defaultDestinationOrderQosPolicy;
    HistoryQosPolicy               defaultHistoryQosPolicy;
    ResourceLimitsQosPolicy        defaultResourceLimitsQosPolicy;
    EntityFactoryQosPolicy         defaultEntityFactoryQosPolicy;
    WriterDataLifecycleQosPolicy   defaultWriterDataLifecycleQosPolicy;
    ReaderDataLifecycleQosPolicy   defaultReaderDataLifecycleQosPolicy;

}QosService;

extern DDS_DLL QosService g_default_Qos;

extern DDS_DLL DDS_DomainParticipantQos       DDS_PARTICIPANT_QOS_DEFAULT;
extern DDS_DLL DDS_TopicQos                   DDS_TOPIC_QOS_DEFAULT;
extern DDS_DLL DDS_DataWriterQos              DDS_DATAWRITER_QOS_DEFAULT;
extern DDS_DLL DDS_DataReaderQos              DDS_DATAREADER_QOS_DEFAULT;

void InitialDefaultQos();

#endif