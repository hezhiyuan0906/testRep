#include "../Include/GlobalDefine.h"

 //_CrtMemState g_startMemState;
 //_CrtMemState g_endMemState;
 //_CrtMemState g_diffMemState;

 CHAR g_ipAddr[32] = "";

 /* 内存泄漏检查 */
 //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //_CrtMemCheckpoint(&g_startMemState);
 //CHECK_MEMORY();

void HiperDGetHostName(BoundedString256 * name)
{
    //获取本地主机名称
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

// 获取本机的IP地址列表
int getHostIPv4AddrList(BoundedString256 * ipList)
{
    int listSize = 0;
#if defined(_WIN32)
    // 初始化winsock
    WSADATA dat;
    if (0 != WSAStartup(MAKEWORD(2, 2), &dat))
    {
        printf("初始化winsocket失败！");
    }
    char host_name[255];
    //获取本地主机名称
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

    /* 循环得出本地机器所有IP地址 */
    /* 只使用第一个网卡 */
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
    // 获取网卡链表
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
            // 是ipv4
            if (strcmp(ifaddrIter->ifa_name, "lo") == 0)
            {
                // 跳过本地环回
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
    //获取本地主机名称
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
    printf("未实现的方法.\n");
    return -1;
#endif

    printf("Network Card not exist!\n");
    return listSize;
}

// 点十形式IP变成long型
long getLongFromIPAddr(const char* ipAddr)
{
    return ntohl(inet_addr(ipAddr));
}

/*---------------------------------------------------------------------------------
-   函 数 名: IpUintToStr
-   功能描述: 整形IP转换成字符串
-   输    入: 无
-   返    回: 布尔
-   全局变量:
-   注    释: 整形IP转换成字符串
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: IpStrToUint
-   功能描述: 字符串IP转换成整形
-   输    入: IP地址
-   返    回: 网络主机序
-   全局变量:
-   注    释: 字符串IP转换成整形
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: InitSendSocketInfo
-   功能描述: 初始化发送端socket
-   输    入: socket信息
-   返    回: 布尔
-   全局变量:
-   注    释: 初始化发送端socket
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitSendSocketInfo(SocketInfo* socketInfo, const char *localAddr)
{
#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0阻塞 1非阻塞 */
#else
    u_long mode = 0; /* 0阻塞 1非阻塞 */
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

    //设置发送端socket参数
    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_SNDBUF, (char *)&sendbufsize, sizeof(sendbufsize)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    /* 阻塞模式设置 */
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
-   函 数 名: InitUnicastReceiveSocketInfo
-   功能描述: 初始化接收端单播socket
-   输    入: socket信息
-   返    回: 布尔
-   全局变量:
-   注    释: 初始化接收端单播socket
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitUnicastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port,  Locator_t* locator)
{

#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0阻塞 1非阻塞 */
#else
    u_long mode = 0; /* 0阻塞 1非阻塞 */
#endif

    u_int bOptval = 1;
    int recvbufsize = UDP_BUF_SIZE;

    socketInfo->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == socketInfo->socket)
    {
        return FALSE;
    }

    //设置接收端socket参数
    if (SOCKET_ERROR == setsockopt(socketInfo->socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvbufsize, sizeof(recvbufsize)))
    {
        closesocket(socketInfo->socket);
        return FALSE;
    }

    /* 阻塞模式设置 */
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
-   函 数 名: InitMulticastReceiveSocketInfo
-   功能描述: 初始化接收端组播socket
-   输    入: socket信息
-   返    回: 布尔
-   全局变量:
-   注    释: 初始化接收端组播socket
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\04\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InitMulticastReceiveSocketInfo(SocketInfo* socketInfo, const char *localAddr, unsigned int port, const char*multicastAddr, Locator_t* locator)
{

#ifdef RAPID_IO_CHANNEL
    u_long mode = 1; /* 0阻塞 1非阻塞 */
#else
    u_long mode = 0; /* 0阻塞 1非阻塞 */
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

    //设置接收端socket参数
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
-   函 数 名: RapIOSendMulticastMsg
-   功能描述: RapIO发送组播消息
-   输    入: 内存块、socket信息、发送地址
-   返    回: 布尔
-   全局变量:
-   注    释: RapIO发送组播消息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
                    printf("UDP发送延迟 :%d\n", err);
                }
                else
                {
                    printf("UDP发送错误 :%d\n", err);
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

                /* 配置获取本机设备id */
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
-   函 数 名: RTPSReceiveMsg
-   功能描述: 消息接收
-   输    入: 内存块、socket信息、发送地址
-   返    回:消息长度
-   全局变量:
-   注    释: RapIO发送组播消息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
-   函 数 名: RTPSSendMsg
-   功能描述: 消息发送
-   输    入: 内存块、socket信息、发送地址
-   返    回:消息长度
-   全局变量:
-   注    释: RapIO发送组播消息
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
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
                    printf("UDP发送延迟 :%d\n", err);
                }
                else
                {
                    printf("UDP发送错误 :%d\n", err);
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

                /* 配置获取本机设备id */
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
