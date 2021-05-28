#include "../Include/GlobalDefine.h"

const EX_UINT32 ENTITY_KEY_MASK = 0x00ffffff;
const EX_UINT32 ENTITY_KIND_MASK = 0xff000000;
const EX_UINT32 AUTO_ENTITYID = 0x00ffffff;

/* entity类型 */
const EX_UINT8 ENTITYKIND_TOPIC             = 0x0a;
const EX_UINT8 ENTITYKIND_WRITER_NO_KEY     = 0x03;
const EX_UINT8 ENTITYKIND_READER_NO_KEY     = 0x04;
const EX_UINT8 ENTITYKIND_WRITER_WITH_KEY   = 0x02;
const EX_UINT8 ENTITYKIND_READER_WITH_KEY   = 0x07;

const EX_UINT32 GEN_INVALID = 0xffffffff;

static UINT32 internal_id = 1;

static UINT16 kind_id = 0;

/* 生成前缀 */
RETSTATUS GenPrefix(DDS_DomainParticipant*  pstDomainParticipant, AUTO_GEN_PREFIX_KIND gen_kind)
{
    EX_UINT32 t_nRtpsHostId, t_nRtpsAppId, t_nRtpsInstanceId, t_nInternalid;
    t_nRtpsHostId = 0;
    t_nRtpsAppId = 0;
    t_nRtpsInstanceId = 0;
    t_nInternalid = 0;

    int listSizeRT = 0;
    t_nInternalid = gen_internal_id();
    if (AUTO_GUID_FROM_IP == gen_kind)
    {
        // 获取本机IP
        BoundedString256 ipList;
        listSizeRT = getHostIPv4AddrList(&ipList);
        if (listSizeRT <= 0)
        {
            /* TODO 获取本机ip失败 */;
            return STA_ERROR;
        }
        /* 默认使用0号网卡 */
        t_nRtpsHostId = getLongFromIPAddr(ipList.value);
        // 获取本进程ID
        t_nRtpsAppId = pstDomainParticipant->processId;

		pstDomainParticipant->ipAddr = htonl(t_nRtpsHostId);
        getNetMask(pstDomainParticipant, ipList.value);
        // 计数器的值
        t_nRtpsInstanceId = t_nInternalid;
    }
    // 根据参数构造前缀
    unsigned char prefix[12];
    getStrFromLong(
        t_nRtpsHostId,
        &prefix[0], &prefix[1], &prefix[2], &prefix[3]);
    getStrFromLong(
        t_nRtpsAppId,
        &prefix[4], &prefix[5], &prefix[6], &prefix[7]);
    getStrFromLong(
        t_nRtpsInstanceId,
        &prefix[8], &prefix[9], &prefix[10], &prefix[11]);

	memcpy(&pstDomainParticipant->stRTPSMsgHeader.guidPrefix.value, prefix, SIZE_OF_GUIDPREFIX);
    return STA_OK;
}

/* 生成GUID */
RETSTATUS GenGuid(GUID_t * guid_t, const GuidPrefix_t*  guidPrefix, UINT8 kind)
{
    kind_id++;
    guid_t->prefix = *guidPrefix;
    guid_t->entityId.entityKind = kind;
    memset(guid_t->entityId.entityKey, 0, 3);
    memcpy(guid_t->entityId.entityKey, &kind_id, sizeof(UINT16));

    return STA_OK;
}

EX_UINT32 gen_internal_id()
{
    return internal_id++;
}