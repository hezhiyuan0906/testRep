#include "../Include/GlobalDefine.h"

HANDLE g_hDev = NULL;

BOOL InitNetWorkStructure(DDS_DomainParticipant*  pstDomainParticipant, USHORT domainId)
{
    BoundedString256 ipList;

    Locator_t* SPDPMulticastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
    if (NULL == SPDPMulticastLocator)
    {
        return FALSE;
    }

    Locator_t* SEDPUnicastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
    if (NULL == SEDPUnicastLocator)
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
        return FALSE;
    }

    Locator_t* USERUnicastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
    if (NULL == USERUnicastLocator)
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
        return FALSE;
    }

    /* ��ȡ���������ַ */
    if (0 == getHostIPv4AddrList(&ipList))
    {
        DDS_STATIC_FREE(SPDPMulticastLocator);
        DDS_STATIC_FREE(SEDPUnicastLocator);
        DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* ��ʼ�����ر�����ȫ����Ϣ*/
    int participantId = pstDomainParticipant->processId;
    participantId >>= 2;
    InitializeDiscoveryInfo(ipList.value, participantId);

    /* ����SPDP�鲥�˿ںţ���RTPS v2.1 ��9.8 */
    UINT32 Multicastport = g_rtps_discovery_info.pb + (g_rtps_discovery_info.dg * domainId) + g_rtps_discovery_info.d0;

    /* ����SEDP�����˿� */
    UINT32 UnicastPort = g_rtps_discovery_info.pb + (g_rtps_discovery_info.dg * domainId) + g_rtps_discovery_info.rootParticipantId * g_rtps_discovery_info.pg  + g_rtps_discovery_info.d1;
 

	GenPrefix(pstDomainParticipant, AUTO_GUID_FROM_IP);

    /* SPDP���Ͷ˳�ʼ�� */
    if (!InitSendSocketInfo(&pstDomainParticipant->socketList.spdp_data_sender, ipList.value))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* SPDP���ն˳�ʼ�� */
    if (!InitMulticastReceiveSocketInfo(&pstDomainParticipant->socketList.spdp_data_receiver, ipList.value, Multicastport, g_rtps_discovery_info.groupAddr, SPDPMulticastLocator))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* SEDP���Ͷ˳�ʼ�� */
    if (!InitSendSocketInfo(&pstDomainParticipant->socketList.sedp_data_sender, ipList.value))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* SEDP���ն˳�ʼ�� */
    if (!InitUnicastReceiveSocketInfo(&pstDomainParticipant->socketList.sedp_data_receiver, ipList.value, UnicastPort, SEDPUnicastLocator))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* USER���Ͷ˳�ʼ�� */
    if (!InitSendSocketInfo(&pstDomainParticipant->socketList.user_data_sender, ipList.value))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    /* USER���ն˳�ʼ�� */
    if (!InitUnicastReceiveSocketInfo(&pstDomainParticipant->socketList.user_data_receiver, ipList.value, UnicastPort, USERUnicastLocator))
    {
		DDS_STATIC_FREE(SPDPMulticastLocator);
		DDS_STATIC_FREE(SEDPUnicastLocator);
		DDS_STATIC_FREE(USERUnicastLocator);
        return FALSE;
    }

    LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.metatrafficMulticastLocator, SPDPMulticastLocator);

    LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.metatrafficUnicastLocator, SEDPUnicastLocator);

    LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.defaultUnicastLocator, USERUnicastLocator);

#ifdef RAPID_IO_CHANNEL

    if (g_RapdIOValid)
    {
        /* ���û�ȡ�����豸id */
        if (!GetStrValue("rapidio", "location", ipList.value))
        {
            printf("localId not exist!\n");
            return TRUE;
        }

        Locator_t* SPDPMulticastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
        if (NULL == SPDPMulticastLocator)
        {
            return TRUE;
        }

        Locator_t* SEDPUnicastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
        if (NULL == SEDPUnicastLocator)
        {
            return TRUE;
        }

        Locator_t* USERUnicastLocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
        if (NULL == USERUnicastLocator)
        {
            return TRUE;
        }

        strncpy(SPDPMulticastLocator->address, ipList.value, strlen(ipList.value));
        SPDPMulticastLocator->port = 0;

		strncpy(SEDPUnicastLocator->address, ipList.value, strlen(ipList.value));
        SEDPUnicastLocator->port = 0;

        strncpy(USERUnicastLocator->address, ipList.value, strlen(ipList.value));
        USERUnicastLocator->port = 0;

        LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.metatrafficMulticastLocator, SPDPMulticastLocator);

        LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.metatrafficUnicastLocator, SEDPUnicastLocator);

        LIST_INSERT_HEAD(pstDomainParticipant->rtpsParticipantProxy.defaultUnicastLocator, USERUnicastLocator);
    }
#endif

    return TRUE;

}

BOOL getNetMask(DDS_DomainParticipant* pstDomainParticipant, char* ipAddr)
{
    char* szMark = DDS_STATIC_MALLOC(16);
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    pAdapterInfo = DDS_STATIC_MALLOC(sizeof(IP_ADAPTER_INFO));
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        DDS_STATIC_FREE(pAdapterInfo);
        pAdapterInfo = DDS_STATIC_MALLOC(ulOutBufLen);
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
    }

    while (&pAdapter != NULL)
    {
        if (FuzzyMatch(&pAdapter->IpAddressList.IpAddress, ipAddr))
        {
            memcpy(szMark, &pAdapter->IpAddressList.IpMask, 16);
            pstDomainParticipant->netMask = htonl(ntohl(inet_addr(szMark)));
            DDS_STATIC_FREE(szMark);
            DDS_STATIC_FREE(pAdapterInfo);
            return TRUE;
        }
        pAdapter = pAdapter->Next;
    }
}