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
*  @file     Guid_t.h
*  @brief    Guid_t
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
Guid_t.h
RTPS规范中Guid_t对应的数据类型及相关处理机制
********************************************************************/

#ifndef HIPERD_GUID_T_H
#define HIPERD_GUID_T_H


/* GuidPrefix_t唯一标识一个DDS域中的Participant */
extern const EX_UINT8 GUIDPREFIX_UNKNOWN_VALUE[12];

/* GuidPrefix_t 结构体序列化成Parameter时占用空间的大小 需要考虑字节对齐*/
#define SIZE_OF_GUIDPREFIX 12
#define GUID_SIZE 16
typedef struct GuidPrefix_t
{
    /* GuidPrefix_t为12个八位字节的GUID前缀，唯一标识一个Participant */    
	EX_UINT8 value[SIZE_OF_GUIDPREFIX];
}GuidPrefix_t;

#define ISEQUAL_GUIDPREFIX(left, right) (memcmp(left.value, right.value, SIZE_OF_GUIDPREFIX) == 0)

/* RTPS保留GuidPrefix_t类型值 */
extern const GuidPrefix_t GUIDPREFIX_UNKNOWN;    

/* EntityId_t唯一标识某一个Participant内的Entity */
typedef struct EntityId_t
{
    // 在保证唯一性（包括预定义的值）条件下任意取值
    EX_UINT8 entityKey[3];
    /**
     * entityKind最高两比特位用用来编码Entity的种类
     *     0x00: 用户定义Entity，RTPS协议定义，但由用户实例化
     *     0x11: 内置Entity，RTPS协议定义，自动实例化
     *     0x01: 厂商特别的Entity
     * 低六位完整列表见Table9.1(P169)
     */
    EX_UINT8 entityKind;
}EntityId_t;

#define ENTITYID_IS_EQUAL(leftEntity,rightEntity)                   \
        ( (leftEntity.entityKey[0] == rightEntity.entityKey[0]) &&  \
          (leftEntity.entityKey[1] == rightEntity.entityKey[1]) &&  \
          (leftEntity.entityKey[2] == rightEntity.entityKey[2]) &&  \
          (leftEntity.entityKind == rightEntity.entityKind))
/* 预定义的Entity_t值,RTPS协议需要用到的内置Entity */
extern const EntityId_t ENTITYID_INVALID                                ; /* 无效的EntityID                  */
extern const EntityId_t ENTITYID_UNKNOWN                                ; /* RTPS保留EntityId_t类型值        */
extern const EntityId_t ENTITYID_PARTICIPANT                            ; /* Participant                     */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_WRITER              ; /* SEDPBuildinTopicWriter          */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_READER              ; /* SEDPbuiltinTopicReader          */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER       ; /* SEDPbuiltinPublicationsWriter   */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER       ; /* SEDPbuiltinPublicationsReader   */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER      ; /* SEDPbuiltinSubscriptionsWriter  */
extern const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER      ; /* SEDPbuiltinSubscriptionsReader  */
extern const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER        ; /* SPDPbuiltinParticipantWriter    */
extern const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER        ; /* SPDPbuiltinSdpParticipantReader */
extern const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER ; /* BuiltinParticipantMessageWriter */
extern const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER ; /* BuiltinParticipantMessageReader */

/* 在一个DDS域内每一个Entity都有一个GUID属性来唯一标识，由二元组<prefix，entitId>构成，类型分别为GuidPrefix_t和EntityId_t类型 */
typedef struct GUID_t
{
    GuidPrefix_t    prefix  ; /* prefix唯一标识Participant                   */
    EntityId_t      entityId; /* entityId唯一标识某一个Participant内的Entity */
}GUID_t;

/* 判断是否为内建的entity */
extern BOOL IsBuiltinEndpoint(const EntityId_t entityid);

/* 判断是否相等 为了防止if判断过长采用函数调用的形式实现 */
extern BOOL Guid_Is_Equal(const GUID_t * leftGuid, const GUID_t * rightGuid);

extern BOOL GuidPrefix_Is_Equal(const GuidPrefix_t * leftGuidPrefix, const GuidPrefix_t * rightGuidPrefix);

extern BOOL EntityId_Is_Equal(const EntityId_t* lefEntityId, const EntityId_t* rightEntityId);

/*
 以下为该结构编解码工具宏
*/
#define SERIALIZE_GUID_PREFIX(prefix , memBlock) PUT_BYTE_ARRAY(memBlock,prefix.value,SIZE_OF_GUIDPREFIX)

#define DESERIALIZE_GUID_PREFIX(prefix , memBlock)    GET_BYTE_ARRAY(memBlock,prefix.value,SIZE_OF_GUIDPREFIX)

#define SERIALIZE_ENTITYID(entityId , memBlock)         \
    do                                                  \
    {                                                   \
        PUT_BYTE_ARRAY(memBlock, entityId.entityKey, 3) \
        PUT_BYTE(memBlock, entityId.entityKind)         \
    } while (0);

#define DESERIALIZE_ENTITYID(entityId , memBlock)       \
    do                                                  \
    {                                                   \
        GET_BYTE_ARRAY(memBlock, entityId.entityKey, 3) \
        GET_BYTE(memBlock, entityId.entityKind)         \
    } while (0);

#define SERIALIZE_GUID(guid , memBlock)                 \
    do                                                  \
    {                                                   \
        SERIALIZE_GUID_PREFIX(guid.prefix , memBlock)   \
        SERIALIZE_ENTITYID(guid.entityId , memBlock)    \
    } while (0);

#define DESERIALIZE_GUID(guid , memBlock)               \
    do                                                  \
    {                                                   \
        DESERIALIZE_GUID_PREFIX(guid.prefix , memBlock) \
        DESERIALIZE_ENTITYID(guid.entityId , memBlock)  \
    } while (0);

/**
 * 保留的GUID_t值
 */
extern const GUID_t GUID_UNKNOWN;    // RTPS保留GUID_t类型

#endif