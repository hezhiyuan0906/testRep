#include "../Include/GlobalDefine.h"
 
/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredParticipant_Init
-   功能描述: 初始化SPDP发现过程
-   输    入: SPDPDiscoveredParticipant指针
-   返    回: 
-   全局变量:
-   注    释: SPDPDiscoveredParticipant成员指针赋NULL
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\04       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredParticipant_Init(DDS_DiscoveredParticipant* pstDiscoveredParticipant)
{
    if (NULL != pstDiscoveredParticipant)
    {
        /* 所有指针都需置NULL */
        pstDiscoveredParticipant->rtpsParticipantProxy.metatrafficUnicastLocator = NULL;
        pstDiscoveredParticipant->rtpsParticipantProxy.metatrafficMulticastLocator = NULL;
        pstDiscoveredParticipant->rtpsParticipantProxy.defaultUnicastLocator = NULL;
        pstDiscoveredParticipant->rtpsParticipantProxy.defaultMulticastLocator = NULL;
        pstDiscoveredParticipant->pNext = NULL;
		pstDiscoveredParticipant->leaseDuration.sec = 0;
		pstDiscoveredParticipant->leaseDuration.nanosec = 0;
		pstDiscoveredParticipant->lastHeartBeat = pstDiscoveredParticipant->leaseDuration;
		pstDiscoveredParticipant->livelinessHeartMissNum = 0;
		pstDiscoveredParticipant->IPAddr = 0;
        pstDiscoveredParticipant->netMask = 0;
		pstDiscoveredParticipant->processID = 0;
		DDS_MUTEX_INIT(pstDiscoveredParticipant->threadMutex);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredParticipant_Uninit
-   功能描述: 析构SPDP发现过程
-   输    入: 发现Participant指针
-   返    回:
-   全局变量:
-   注    释: 析构发现Participant成员指针
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\04       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredParticipant_Uninit(DDS_DiscoveredParticipant* pstDiscoveredParticipant)
{
    if (NULL != pstDiscoveredParticipant)
    {
        Locator_t* pstTempNode = NULL;
        /* 所有成员指针析构 */
        LIST_DELETE_ALL(pstDiscoveredParticipant->rtpsParticipantProxy.metatrafficUnicastLocator, pstTempNode);
        LIST_DELETE_ALL(pstDiscoveredParticipant->rtpsParticipantProxy.metatrafficMulticastLocator, pstTempNode);
        LIST_DELETE_ALL(pstDiscoveredParticipant->rtpsParticipantProxy.defaultUnicastLocator, pstTempNode);
        LIST_DELETE_ALL(pstDiscoveredParticipant->rtpsParticipantProxy.defaultMulticastLocator, pstTempNode);
    }
}
