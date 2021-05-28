#include "../Include/GlobalDefine.h"

const int ProtocalId_RTPS = 0x53505452;

/*
* 保留的ProtocolId_t值
*/
const ProtocolId_t PROTOCOL_RTPS_LITTLE_ENDIAN = 0x53505452;    // 小端RTPS协议
const ProtocolId_t PROTOCOL_RTPX_LITTLE_ENDIAN = 0x58505452;    // 小端RTPX协议
const ProtocolId_t PROTOCOL_RTPS_BIG_ENDIAN = 0x52545053;    // 大端RTPS协议
const ProtocolId_t PROTOCOL_RTPX_BIG_ENDIAN = 0x52505458;    // 大端RTPX协议

VOID InitRTPSMessageHeader(RTPSMessageHeader* pstRTPSMsgHeader)
{
    pstRTPSMsgHeader->protocolId      = PROTOCOL_RTPS_LITTLE_ENDIAN;
    pstRTPSMsgHeader->vendorId        = VENDORID_EVO;
    pstRTPSMsgHeader->protocolVersion = PROTOCOLVERSION;
    //pstRTPSMsgHeader->guidPrefix      = g_rtps_discovery_info.guidPrefix;
}