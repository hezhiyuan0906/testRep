#include "../Include/GlobalDefine.h"

 //_CrtMemState g_startMemState;
 //_CrtMemState g_endMemState;
 //_CrtMemState g_diffMemState;

 CHAR g_ipAddr[32] = "";

 /* �ڴ�й©��� */
 //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //_CrtMemCheckpoint(&g_startMemState);
 //CHECK_MEMORY();

void HiperDGetHostName(BoundedString256 * name)
{
    //��ȡ������������
    if (gethostname(name->value, 256) < 0)
    {
        printf("Error when getting local host name.");
        return ;
    }
    name->length = (UINT32)strlen(name->value);
}

BOOL FuzzyMatch(char* srcStr, char*destStr)
{
    unsigned int i = 0;
    char tmpStr[32];
    for (i = 0; i < 32; i++)
    {
        if ('*' == destStr[i])
        {
            tmpStr[i] = '\0';
            break;
        }
        tmpStr[i] = destStr[i];
    }

    if (!strstr(srcStr, tmpStr))
    {
        return FALSE;
    }

    return TRUE;
}

// ��ȡ������IP��ַ�б�
int getHostIPv4AddrList(BoundedString256 * ipList)
{
    int listSize = 0;
#if defined(_WIN32)
    // ��ʼ��winsock
    WSADATA dat;
    if (0 != WSAStartup(MAKEWORD(2, 2), &dat))
    {
        printf("��ʼ��winsocketʧ�ܣ�");
    }
    char host_name[255];
    //��ȡ������������
    if (gethostname(host_name, sizeof(host_name)) < 0)
    {
        printf("Error when getting local host name.");
        return 0;
    }
    struct hostent *phe = gethostbyname(host_name);
    if (phe == 0)
    {
        printf("Bad host lookup.");
        return 0;
    }

    /* ѭ���ó����ػ�������IP��ַ */
    /* ֻʹ�õ�һ������ */
    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
    {

        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        ipList->length = (UINT32)(strlen(inet_ntoa(addr)) + 1);

        if (!strcmp(g_ipAddr, ""))
        {
            memcpy(ipList->value, inet_ntoa(addr), ipList->length);
            listSize++;
            return listSize;
        }
        else
        {
            if (FuzzyMatch(inet_ntoa(addr), g_ipAddr))
            {
                memcpy(ipList->value, inet_ntoa(addr), ipList->length);
                listSize++;
                return listSize;
            }
        }
    }
#elif defined(_LINUX)
    struct ifaddrs* ifAddrStruct = NULL;
    void* tmpAddrPtr = NULL;
    // ��ȡ��������
    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == NULL)
    {
        printf("Error when getifaddrs.\n");
        return -1;
    }
    struct ifaddrs* ifaddrIter = ifAddrStruct;
    char tmpIP[16] = { 0 };
    while (ifaddrIter != NULL)
    {
        if (ifaddrIter->ifa_addr->sa_family == AF_INET)
        {
            // ��ipv4
            if (strcmp(ifaddrIter->ifa_name, "lo") == 0)
            {
                // �������ػ���
                ifaddrIter = ifaddrIter->ifa_next;
                continue;
            }
            tmpAddrPtr = &((struct sockaddr_in *)ifaddrIter->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, tmpIP, 16);
            ipList->length = 16;
            memcpy(ipList->value, tmpIP, 16);
            listSize++;
        }
        ifaddrIter = ifaddrIter->ifa_next;
    }
    freeifaddrs(ifAddrStruct);
#elif defined(_VXWORKS)
    char host_name[255];
    //��ȡ������������
    if (gethostname(host_name, sizeof(host_name)) < 0)
    {
        printf("Error when getting local host name.");
        return 0;
    }
    int hostAddr = hostGetByName(host_name);
    if (hostAddr == ERROR)
    {
        printf("Error when called hostGetByName.");
        return 0;
    }
    struct in_addr iaddr;
    char* inet;
    iaddr.s_addr = hostAddr;
    inet = inet_ntoa(iaddr);
    ipList->length = strlen(inet_ntoa(iaddr));
    memcpy(ipList->value,inet_ntoa(iaddr),ipList->length);
    listSize++;
#else
    printf("δʵ�ֵķ���.\n");
    return -1;
#endif

    printf("Network Card not exist!\n");
    return listSize;
}

// ��ʮ��ʽIP���long��
long getLongFromIPAddr(const char* ipAddr)
{
    return ntohl(inet_addr(ipAddr));
}

/*---------------------------------------------------------------------------------
-   �� �� ��: IpUintToStr
-   ��������: ����IPת�����ַ���
-   ��    ��: ��
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ����IPת�����ַ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID IpUintToStr(ULONG ulAddr, CHAR* cpAddr)
{
	memset(cpAddr, 0, LOCATOR_ADDR_IP4_STR_BEGIN);
    cpAddr[LOCATOR_ADDR_IP4_STR_BEGIN + 3] = ulAddr & 0xff;
    cpAddr[LOCATOR_ADDR_IP4_STR_BEGIN + 2] = (ulAddr >> 8) & 0xff;
    cpAddr[LOCATOR_ADDR_IP4_STR_BEGIN + 1] = (ulAddr >> 16) & 0xff;
    cpAddr[LOCATOR_ADDR_IP4_STR_BEGIN + 0] = (ulAddr >> 24) & 0xff;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: IpStrToUint
-   ��������: �ַ���IPת��������
-   ��    ��: IP��ַ
-   ��    ��: ����������
-   ȫ�ֱ���:
-   ע    ��: �ַ���IPת��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
ULONG IpStrToUint(CHAR* address)
{
    ULONG ulAddr = 0;
    ulAddr |= (address[LOCATOR_ADDR_IP4_STR_BEGIN + 3]) & 0xff;
    ulAddr |= (address[LOCATOR_ADDR_IP4_STR_BEGIN + 2] << 8) & 0xff00;
    ulAddr |= (address[LOCATOR_ADDR_IP4_STR_BEGIN + 1] << 16) & 0xff0000;
    ulAddr |= (address[LOCATOR_ADDR_IP4_STR_BEGIN + 0] << 24) & 0xff000000;

    return htonl(ulAddr);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: InitSendSocketInfo
-   ��������: ��ʼ�����Ͷ�socket
-   ��    ��: socket��Ϣ
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ��ʼ�����Ͷ�socket
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitSendSocketInfo(SocketInfo* socketInfo, const char *localAddr)
{
#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0���� 1������ */
#else
    u_long mode = 0; /* 0���� 1������ */
#endif

    int sendbufsize = UDP_BUF_SIZE;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return FALSE;
    }

    socketInfo->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == socketInfo->socket)
    {
        return FALSE;
    }

    //���÷��Ͷ�socket����
    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_SNDBUF, (char *)&sendbufsize, sizeof(sendbufsize)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    /* ����ģʽ���� */
    if (SOCKET_ERROR == ioctlsocket(socketInfo->socket, FIONBIO, (u_long*)&mode))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    socketInfo->sockAddr.sin_family = AF_INET;
    socketInfo->sockAddr.sin_addr.s_addr = inet_addr(localAddr);
    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: InitUnicastReceiveSocketInfo
-   ��������: ��ʼ�����ն˵���socket
-   ��    ��: socket��Ϣ
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ��ʼ�����ն˵���socket
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitUnicastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port,  Locator_t* locator)
{

#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0���� 1������ */
#else
    u_long mode = 0; /* 0���� 1������ */
#endif

    u_int bOptval = 1;
    int recvbufsize = UDP_BUF_SIZE;

    socketInfo->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == socketInfo->socket)
    {
        return FALSE;
    }

    //���ý��ն�socket����
    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvbufsize, sizeof(recvbufsize)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    /* ����ģʽ���� */
    if (SOCKET_ERROR == ioctlsocket(socketInfo->socket, FIONBIO, (u_long*)&mode))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    socketInfo->sockAddr.sin_family = AF_INET;
    socketInfo->sockAddr.sin_addr.s_addr = inet_addr(localAddr);

    while (TRUE)
    {
        socketInfo->sockAddr.sin_port = htons(port);
        if (SOCKET_ERROR != bind(socketInfo->socket, (struct sockaddr *)&socketInfo->sockAddr, sizeof(socketInfo->sockAddr)))
        {
            IpUintToStr(ntohl(inet_addr(localAddr)), locator->address);
            locator->kind = LOCATOR_KIND_UDPv4;
            locator->port = port;
            break;
        }
        port++;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------------
-   �� �� ��: InitMulticastReceiveSocketInfo
-   ��������: ��ʼ�����ն��鲥socket
-   ��    ��: socket��Ϣ
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: ��ʼ�����ն��鲥socket
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitMulticastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port, const char*multicastAddr, Locator_t* locator)
{

#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0���� 1������ */
#else
    u_long mode = 0; /* 0���� 1������ */
#endif

    u_int bOptval = 1;
    int recvbufsize = UDP_BUF_SIZE;

    socketInfo->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == socketInfo->socket)
    {
        return FALSE;
    }

    if (SOCKET_ERROR == ioctlsocket(socketInfo->socket, FIONBIO, (u_long*)&mode))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    //���ý��ն�socket����
    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvbufsize, sizeof(recvbufsize)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    socketInfo->sockAddr.sin_family = AF_INET;
    socketInfo->sockAddr.sin_addr.s_addr = inet_addr(localAddr);
    socketInfo->sockAddr.sin_port = htons(port);


    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptval, sizeof(bOptval)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }
    if (SOCKET_ERROR == bind(socketInfo->socket, (struct sockaddr FAR *)&socketInfo->sockAddr, sizeof(struct sockaddr)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    struct ip_mreq multiCast;
    multiCast.imr_multiaddr.s_addr = inet_addr(multicastAddr);
    multiCast.imr_interface.s_addr = inet_addr(localAddr);

    IpUintToStr(ntohl(inet_addr(multicastAddr)), locator->address);
    locator->kind = LOCATOR_KIND_UDPv4;
    locator->port = port;

    if (SOCKET_ERROR == setsockopt(socketInfo->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR *)&multiCast, sizeof(struct ip_mreq)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: RapIOSendMulticastMsg
-   ��������: RapIO�����鲥��Ϣ
-   ��    ��: �ڴ�顢socket��Ϣ�����͵�ַ
-   ��    ��: ����
-   ȫ�ֱ���:
-   ע    ��: RapIO�����鲥��Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/

BOOL SendMulticastMsg(MemoryBlock * memBlock, SocketInfo * socketInfo, Locator_t * locator)
{
    Locator_t* pstLocarot = locator;
    while (NULL != pstLocarot)
    {
        if (0 != pstLocarot->port)
        {
            socketInfo->sockAddr.sin_addr.s_addr = IpStrToUint(pstLocarot->address);
            socketInfo->sockAddr.sin_port = htons(pstLocarot->port);

            int iLen = sendto(socketInfo->socket,
                memBlock->base,
                memBlock->writeIndex,
                0,
                (struct sockaddr FAR *) & (socketInfo->sockAddr),
                sizeof(socketInfo->sockAddr));

            if (iLen == SOCKET_ERROR)
            {
                int err = WSAGetLastError();
                if (err == WSAEWOULDBLOCK || err == WSAEMSGSIZE)
                {
                    printf("UDP�����ӳ� :%d\n", err);
                }
                else
                {
                    printf("UDP���ʹ��� :%d\n", err);
                }
                return FALSE;
            }
        }
        else
        {
#ifdef RAPID_IO_CHANNEL
            if (g_RapdIOValid)
            {
                long location = 1;

                /* ���û�ȡ�����豸id */
                if (!GetLongValue("rapidio", "location", &location))
                {
                    printf("location not exist!");
                    return FALSE;
                }

                for (int i = 0; i < NUM_OF_RIO_NODE; i++)
                {
                    if (location != RIO_NODE_ARRAY[i] && 0 != RIO_NODE_ARRAY[i])
                    {
                        UnicastSend(RIO_NODE_ARRAY[i], memBlock->base, memBlock->writeIndex);
                    }

                }
            }
#endif
        }

        pstLocarot = pstLocarot->pNext;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   �� �� ��: RTPSReceiveMsg
-   ��������: ��Ϣ����
-   ��    ��: �ڴ�顢socket��Ϣ�����͵�ַ
-   ��    ��:��Ϣ����
-   ȫ�ֱ���:
-   ע    ��: RapIO�����鲥��Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
int RTPSReceiveMsg(SocketInfo * locator, char * buffer)
{
#ifdef RAPID_IO_CHANNEL
    if (g_RapdIOValid)
    {
        int msgLen = GetRioData(buffer);
        if (0 < msgLen)
        {
            return msgLen;
        }
    }
#endif
    int addrLen = sizeof(struct sockaddr);

    return recvfrom(locator->socket, buffer, MAX_UDP_MSG_SIZE, 0, (struct sockaddr *)&locator->sockAddr, &addrLen);
}

/*---------------------------------------------------------------------------------
-   �� �� ��: RTPSSendMsg
-   ��������: ��Ϣ����
-   ��    ��: �ڴ�顢socket��Ϣ�����͵�ַ
-   ��    ��:��Ϣ����
-   ȫ�ֱ���:
-   ע    ��: RapIO�����鲥��Ϣ
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL RTPSSendMsg(MemoryBlock * memBlock, SocketInfo * socketInfo, Locator_t * locator)
{
    Locator_t* pstLocarot = locator;
    while (NULL != pstLocarot)
    {
        if (0 != pstLocarot->port)
        {
            socketInfo->sockAddr.sin_addr.s_addr = IpStrToUint(pstLocarot->address);
            socketInfo->sockAddr.sin_port = htons(pstLocarot->port);

            int iLen = sendto(socketInfo->socket,
                memBlock->base,
                memBlock->writeIndex,
                0,
                (struct sockaddr FAR *) & (socketInfo->sockAddr),
                sizeof(socketInfo->sockAddr));

            if (iLen == SOCKET_ERROR)
            {
                int err = WSAGetLastError();
                if (err == WSAEWOULDBLOCK || err == WSAEMSGSIZE)
                {
                    printf("UDP�����ӳ� :%d\n", err);
                }
                else
                {
                    printf("UDP���ʹ��� :%d\n", err);
                }
                return FALSE;
            }
            return TRUE;

        }
        else
        {
#ifdef RAPID_IO_CHANNEL
            if (g_RapdIOValid)
            {
                long location = 1;

                /* ���û�ȡ�����豸id */
                if (!GetLongValue("rapidio", "location", &location))
                {
                    printf("location not exist!");
                    return FALSE;
                }

                if (location != atol(pstLocarot->address))
                {
                    UnicastSend(atol(pstLocarot->address), memBlock->base, memBlock->writeIndex);
                }

                return TRUE;
            }
#endif
        }

        pstLocarot = pstLocarot->pNext;
    }

    return TRUE;
}
