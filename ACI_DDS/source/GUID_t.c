#include "../Include/GlobalDefine.h"

/**
 * GuidPrefix_t唯一标识一个DDS域中的Participant
 */
const UINT8 GUIDPREFIX_UNKNOWN_VALUE[12] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// RTPS保留GuidPrefix_t类型值
const GuidPrefix_t GUIDPREFIX_UNKNOWN = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};


/**
 * 预定义的Entity_t值,RTPS协议需要用到的内置Entity
 */
const EntityId_t ENTITYID_INVALID = { 0xff, 0xff, 0xff, 0xff };   /* 无效entityID */
const EntityId_t ENTITYID_UNKNOWN = {0x00, 0x00, 0x00, 0x00};    // RTPS保留EntityId_t类型值
const EntityId_t ENTITYID_PARTICIPANT = {0x00, 0x00, 0x01, 0xc1};    // Participant
const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_WRITER = {0x00, 0x00, 0x02, 0xc2};    // SEDPBuildinTopicWriter
const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_READER = {0x00, 0x00, 0x02, 0xc7};    // SEDPbuiltinTopicReader
const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER = {0x00, 0x00, 0x03, 0xc2};    // SEDPbuiltinPublicationsWriter
const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER = {0x00, 0x00, 0x03, 0xc7};    // SEDPbuiltinPublicationsReader
const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER = {0x00, 0x00, 0x04, 0xc2};    // SEDPbuiltinSubscriptionsWriter
const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER = {0x00, 0x00, 0x04, 0xc7};    // SEDPbuiltinSubscriptionsReader
const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER = {0x00, 0x01, 0x00, 0xc2};    // SPDPbuiltinParticipantWriter
const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER = {0x00, 0x01, 0x00, 0xc7};    // SPDPbuiltinSdpParticipantReader
const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER = {0x00, 0x02, 0x00, 0xc2};    // BuiltinParticipantMessageWriter
const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER = {0x00, 0x02, 0x00, 0xc7};    // BuiltinParticipantMessageReader

/**
 * 保留的GUID_t值
 */
const GUID_t GUID_UNKNOWN = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
{0x00, 0x00, 0x00, 0x00}};    // RTPS保留GUID_t类型





BOOL IsBuiltinEndpoint(const EntityId_t entityid)
{
    return (entityid.entityKind & 0xc0) == 0xc0;
}

BOOL GuidPrefix_Is_Equal(const GuidPrefix_t * leftGuidPrefix, const GuidPrefix_t * rightGuidPrefix)
{
    int i;
    for (i = 0; i < 12; i++)
    {
        if (leftGuidPrefix->value[i] != rightGuidPrefix->value[i])
            return FALSE;
    }
    return TRUE;
}

BOOL EntityId_Is_Equal(const EntityId_t* lefEntityId, const EntityId_t* rightEntityId)
{
    /* 考虑到结构体对齐问题 不能直接判断内存是否相等 */
    int i;

    if (lefEntityId->entityKind != rightEntityId->entityKind)
        return FALSE;
    for (i = 0; i < 3; i++)
    {
        if (lefEntityId->entityKey[i] != rightEntityId->entityKey[i])
            return FALSE;
    }
    return TRUE;
}

BOOL Guid_Is_Equal(const GUID_t * leftGuid, const GUID_t * rightGuid)
{
	/* 考虑到结构体对齐问题 不能直接判断内存是否相等 */
	return (EntityId_Is_Equal(&leftGuid->entityId, &rightGuid->entityId) && GuidPrefix_Is_Equal(&leftGuid->prefix,&rightGuid->prefix));
}