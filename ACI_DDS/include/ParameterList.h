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
*  @file     ParameterList.h
*  @brief    ParameterList
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

/*
ParameterList.h
该文件包含了参数结构体Parameter的定义，Parameter用来表示RTPS协议中的参数
以及参数列表结构体ParameterList及其相关操作机制
*/

#ifndef HIPERD_PARAMETER_LIST_H
#define HIPERD_PARAMETER_LIST_H

typedef EX_INT16 ParameterId_t;

typedef EX_INT16 ParamType;
/**
* ParameterId_t的保留值
*/
#define PID_PAD                                 0x0000    /* N/A                       */
#define PID_SENTINEL                            0x0001    /* N/A                       */
#define PID_USER_DATA                           0x002c    /* UserDataQosPolicy         */
#define PID_TOPIC_NAME                          0x0005    /* string<256>               */
#define PID_TYPE_NAME                           0x0007    /* string<256>               */
#define PID_GROUP_DATA                          0x002d    /* GroupDataQosPolicy        */
#define PID_TOPIC_DATA                          0x002e    /* TopicDataQosPolicy        */
#define PID_DURABILITY                          0x001d    /* DurabilityQosPolicy       */
#define PID_DURABILITY_SERVICE                  0x001e    /* DurabilityServiceQosPolicy*/
#define PID_DEADLINE                            0x0023    /* DeadlineQosPolicy         */
#define PID_LATENCY_BUDGET                      0x0027    /* LatencyBudgetQosPolicy    */
#define PID_LIVELINESS                          0x001b    /* LivelinessQosPolicy       */
#define PID_RELIABILITY                         0x001A    /* ReliabilityQosPolicy      */
#define PID_LIFESPAN                            0x002b    /* LifespanQosPolicy         */
#define PID_DESTINATION_ORDER                   0x0025    /* DestinationOrderQosPolicy */
#define PID_HISTORY                             0x0040    /* HistoryQosPolicy          */
#define PID_RESOURCE_LIMITS                     0x0041    /* ResourceLimitsQosPolicy   */
#define PID_OWNERSHIP                           0x001f    /* OwnershipQosPolicy        */
#define PID_OWNERSHIP_STRENGTH                  0x0006    /* OwnershipStrengthQosPolicy*/
#define PID_PRESENTATION                        0x0021    /* PresentationQosPolicy     */
#define PID_PARTITION                           0x0029    /* PartitionQosPolicy        */
#define PID_TIME_BASED_FILTER                   0x0004    /* TimeBasedFilterQosPolicy  */
#define PID_TRANSPORT_PRIORITY                  0x0049    /* TransportPriorityQoSPolicy*/
#define PID_PROTOCOL_VERSION                    0x0015    /* ProtocolVersion_t         */
#define PID_VENDORID                            0x0016    /* VendorId_t                */
#define PID_UNICAST_LOCATOR                     0x002f    /* Locator_t                 */
#define PID_MULTICAST_LOCATOR                   0x0030    /* Locator_t                 */
#define PID_MULTICAST_IPADDRESS                 0x0011    /* IPv4Address_t             */
#define PID_DEFAULT_UNICAST_LOCATOR             0x0031    /* Locator_t                 */
#define PID_DEFAULT_MULTICAST_LOCATOR           0x0048    /* Locator_t                 */
#define PID_METATRAFFIC_UNICAST_LOCATOR         0x0032    /* Locator_t                 */
#define PID_METATRAFFIC_MULTICAST_LOCATOR       0x0033    /* Locator_t                 */
#define PID_DEFAULT_UNICAST_IPADDRESS           0x000c    /* IPv4Address_t             */
#define PID_DEFAULT_UNICAST_PORT                0x000e    /* Port_t                    */
#define PID_METATRAFFIC_UNICAST_IPADDRESS       0x0045    /* IPv4Address_t             */
#define PID_METATRAFFIC_UNICAST_PORT            0x000d    /* Port_t                    */
#define PID_METATRAFFIC_MULTICAST_IPADDRESS     0x000b    /* IPv4Address_t             */
#define PID_METATRAFFIC_MULTICAST_PORT          0x0046    /* Port_t                    */
#define PID_EXPECTS_INLINE_QOS                  0x0043    /* boolean                   */
#define PID_PARTICIPANT_MANUAL_LIVELINESS_COUNT 0x0034    /* Count_t                   */
#define PID_PARTICIPANT_BUILTIN_ENDPOINTS       0x0044    /* unsigned_long             */
#define PID_PARTICIPANT_LEASE_DURATION          0x0002    /* Duration_t                */
#define PID_CONTENT_FILTER_PROPERTY             0x0035    /* ContentFilterProperty_t   */
#define PID_PARTICIPANT_GUID                    0x0050    /* GUID_t                    */
#define PID_PARTICIPANT_ENTITYID                0x0051    /* EntityId_t                */
#define PID_GROUP_GUID                          0x0052    /* GUID_t                    */
#define PID_GROUP_ENTITYID                      0x0053    /* EntityId_t                */
#define PID_BUILTIN_ENDPOINT_SET                0x0058    /* BuiltinEndpointSet_t      */
#define PID_PROPERTY_LIST                       0x0059    /* sequence<Property_t>      */
#define PID_TYPE_MAX_SIZE_SERIALIZED            0x0060    /* long                      */
#define PID_ENTITY_NAME                         0x0062    /* EntityName_t              */
#define PID_KEY_HASH                            0x0070    /* KeyHash_t                 */
#define PID_STATUS_INFO                         0x0071    /* StatusInfo_t              */
#define PID_CONTENT_FILTER_INFO                 0x0055    /* ContentFilterInfo_t       */
#define PID_COHERENT_SET                        0x0056    /* SequenceNumber_t          */
#define PID_DIRECTED_WRITE                      0x0057    /* sequence<GUID_t>          */
#define PID_ORIGINAL_WRITER_INFO                0x0061    /* OriginalWriterInfo_t      */
/* 协议中没有，但是SEDP中用到，rti以及opendds中均有定义 */
#define PID_ENDPOINT_GUID                       0x005a  
#define PID_MONITOR_DOMAIN                      0x800e  
#define PID_DOMAINID                            0x800f    /* domain_id                 */

/* 自定义监控协议 */

#define TOPIC_GUID                             0x0100    /* topic guid                 */
#define WRITER_GUID                            0x0101    /* writer guid                 */
#define READER_GUID                            0x0102    /* reader guid                 */
#define DISC_WRITER_GUID                       0x0103    /* discovery writer guid       */
#define DISC_READER_GUID                       0x0104    /* discovery reader guid        */
#define PID_TOPIC_GUID                         0x0105    /* discovery reader guid        */
#define PID_STATUS                             0x0106    /* discovery reader guid        */

/*IP地址和进程ID信息*/
#define PID_IPADDR								0x0200
#define PID_PROCESSID							0x0201
#define PID_NETMASK							    0x0202

/* Parameter最大长度 */
#define MAX_PARA_LEN 512
#define PARAM_HEAD_SIZE 4

typedef struct _Parameter
{
    /* 唯一标识参数类型 */
    ParameterId_t parameterId;
    /* 该参数值的长度 */
    EX_INT16 length;
    // 长度为length的具体内容
    EX_UINT8 value[MAX_PARA_LEN];
}Parameter;


#define ASSIGNMENT_PARAMETER(target_para, src_para)                 \
    do                                                              \
    {                                                               \
        target_para.parameterId = src_para.parameterId;             \
        target_para.length = src_para.length;                       \
        memcpy(target_para.value, src_para.value, src_para.length); \
    } while (0);

/*
Parameter 编解码工具宏
*/



#define SERIALIZE_PARAMETER(para, memBlock)                 \
    do                                                      \
    {                                                       \
        PUT_SHORT(memBlock, para.parameterId)               \
        PUT_SHORT(memBlock, para.length)                    \
        PUT_BYTE_ARRAY(memBlock, para.value, para.length)   \
    } while (0);

#define DESERIALIZE_PARAMETER(para, memBlock)                       \
    do                                                              \
    {                                                               \
        EX_UINT8 rubValue_t[MAX_PARA_LEN];                          \
        GET_SHORT(memBlock, para.parameterId)                       \
        GET_SHORT(memBlock, para.length)                            \
        if(para.length > MAX_PARA_LEN)                              \
        {                                                           \
            GET_BYTE_ARRAY(memBlock, rubValue_t, MAX_PARA_LEN)       \
        }                                                           \
        else                                                        \
        {                                                           \
            if(para.parameterId != PID_SENTINEL)                    \
                GET_BYTE_ARRAY(memBlock, para.value, para.length)   \
            else                                                    \
                para.length = 0;                                    \
        }                                                           \
    } while (0);


/* ParameterList 中最多能包含40个参数 可根据实际情况调整*/
#define MAX_PARA_NUM 40
typedef struct ParameterList
{
    /* Parameter列表 */
    Parameter stParamList[MAX_PARA_NUM];
    /* 当前列表中有多少个 Parameter */
    UINT32 uiCurPos;
}ParameterList;

BOOL containsRegister(ParameterList * curList);

BOOL containsUnregister(ParameterList * curList);

BOOL containsDispose(ParameterList * curList);

BOOL containsDisposeUnregister(ParameterList * curList);

#define INITIAL_PARAMETER_LIST(paraList) paraList.uiCurPos=0;

/*
ParameterList操作宏
*/                  
#define INSERT_PARAMETERLIST(paraList, para)                                    \
    do                                                                          \
    {                                                                           \
        if (paraList.uiCurPos < MAX_PARA_NUM)                                  \
        {                                                                       \
            ASSIGNMENT_PARAMETER(paraList.stParamList[paraList.uiCurPos], para)\
            paraList.uiCurPos += 1;                                            \
        }                                                                       \
    } while (0);


/* 
ParameterList 编解码工具宏
*/



#define SERIALIZE_PARAMETERLIST(paraList, memBlock)                                                     \
    do                                                                                                  \
    {                                                                                                   \
        int  paraListIndex_local;                                                                       \
        if (paraList.uiCurPos > 0 &&                                                                   \
            paraList.stParamList[paraList.uiCurPos - 1].parameterId != PID_SENTINEL)                   \
        {                                                                                               \
            /* TODO handle error : ParameterList not end with PID_SENTINEL */                           \
        }                                                                                               \
        for (paraListIndex_local = 0; paraListIndex_local < paraList.uiCurPos; paraListIndex_local++)  \
        {                                                                                               \
            SERIALIZE_PARAMETER(paraList.stParamList[paraListIndex_local], memBlock)                    \
        }                                                                                               \
    } while (0);


#define DESERIALIZE_PARAMETERLIST(paraList, memBlock)                                       \
    do                                                                                      \
    {                                                                                       \
        paraList.uiCurPos = 0;                                                             \
        while (1)                                                                           \
        {                                                                                   \
            DESERIALIZE_PARAMETER(paraList.stParamList[paraList.uiCurPos], memBlock)       \
            paraList.uiCurPos += 1;                                                        \
            if (paraList.stParamList[paraList.uiCurPos - 1].parameterId == PID_SENTINEL)   \
                break;                                                                      \
        }                                                                                   \
    } while (0);


#endif