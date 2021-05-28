#include "../Include/GlobalDefine.h"

RTPSDiscovery g_rtps_discovery_info = {7400 , 250 , 2 , 0 , 10 , 1 , 11 , "239.255.0.1" , 0};

BOOL InitializeDiscoveryInfo(const char* ipAddr , unsigned int rootParticipantId)
{
    g_rtps_discovery_info.rootParticipantId = rootParticipantId;
    strcpy_s(g_rtps_discovery_info.unicastAddr , 16 , ipAddr);

    return TRUE;
}

