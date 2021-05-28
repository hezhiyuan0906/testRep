#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: NetworkByteOrderConvertDiscParticipant
-   功能描述: 网络缓字节序转换
-   输    入: 内存块、pstMonitor
-   返    回:
-   全局变量:
-   注    释: 网络字节序转换为pstMonitor
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL NetworkByteOrderConvertMonitor(MemoryBlock*  pstMemBlock, DDS_Monitor * pstMonitor)
{
    GUID_t parGuid = GUID_UNKNOWN;
    USHORT usStartIndex = 0;
    USHORT usEndIndex = 0;
    Locator_t* pstlocator = NULL;
    Locator_t* pstTempLocator = NULL;
	ParameterHead stParamHead;
    stParamHead.length = 0;
    stParamHead.paramType = PID_PAD;

    while (pstMemBlock->readIndex < pstMemBlock->totalSize)
    {
        DESERIALIZE_PARAM_HEAD(stParamHead, (*pstMemBlock));
        if (stParamHead.length == 0 || stParamHead.paramType == PID_PAD)
        {
            break;
        }
        usStartIndex = pstMemBlock->readIndex;

        switch (stParamHead.paramType)
        {

        case PID_PROTOCOL_VERSION:

            DESERIALIZE_PROTOCOL_VERSION(pstMonitor->stRTPSMsgHeader.protocolVersion, (*pstMemBlock));
            break;

        case PID_PARTICIPANT_GUID:

            DESERIALIZE_GUID(parGuid, (*pstMemBlock));
            pstMonitor->stRTPSMsgHeader.guidPrefix = parGuid.prefix;
            break;

        case PID_VENDORID:

            DESERIALIZE_VENDORID(pstMonitor->stRTPSMsgHeader.vendorId, (*pstMemBlock));
            break;

        case PID_DEFAULT_UNICAST_LOCATOR:

            pstlocator = (Locator_t*)DDS_STATIC_MALLOC(sizeof(Locator_t));
            if (NULL != pstlocator)
            {
                DESERIALIZE_LOCATOR((*pstlocator), (*pstMemBlock));
                LIST_INSERT_TAIL(pstMonitor->unicastLocator, pstlocator, pstTempLocator);
            }
            break;

        case PID_SENTINEL:

            break;

        default:

            MEMORYBLOCK_SKIP_READER((*pstMemBlock), stParamHead.length);
            break;
        }

        usEndIndex = pstMemBlock->readIndex;
        /* 偏移量矫正 */
        if (0 < stParamHead.length + usStartIndex - usEndIndex)
        {
            pstMemBlock->rdPtr += stParamHead.length + usStartIndex - usEndIndex;
            pstMemBlock->readIndex += stParamHead.length + usStartIndex - usEndIndex;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: NetworkByteOrderConvertDiscParticipant
-   功能描述: 网络缓字节序转换
-   输    入: 内存块、pstMonitor
-   返    回:
-   全局变量:
-   注    释: 网络字节序转换为pstMonitor
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL NetworkByteOrderConvertTopic(MemoryBlock*  pstMemBlock, DDS_TopicMon * pstTopicMon)
{
    BOOL status =FALSE;
    USHORT usStartIndex = 0;
    USHORT usEndIndex = 0;
    ParameterHead stParamHead;
    stParamHead.length = 0;
    stParamHead.paramType = PID_PAD;

    while (pstMemBlock->readIndex < pstMemBlock->totalSize)
    {
        DESERIALIZE_PARAM_HEAD(stParamHead, (*pstMemBlock));
        if (stParamHead.length == 0 || stParamHead.paramType == PID_PAD)
        {
            break;
        }
        usStartIndex = pstMemBlock->readIndex;

        switch (stParamHead.paramType)
        {

        case PID_TOPIC_GUID:

            DESERIALIZE_GUID(pstTopicMon->tpGuid, (*pstMemBlock));
            break;

        case PID_STATUS:

            GET_UNSIGNED_INT((*pstMemBlock), status);
            break;

        case PID_SENTINEL:

            break;

        default:

            MEMORYBLOCK_SKIP_READER((*pstMemBlock), stParamHead.length);
            break;
        }

        usEndIndex = pstMemBlock->readIndex;
        /* 偏移量矫正 */
        if (0 < stParamHead.length + usStartIndex - usEndIndex)
        {
            pstMemBlock->rdPtr += stParamHead.length + usStartIndex - usEndIndex;
            pstMemBlock->readIndex += stParamHead.length + usStartIndex - usEndIndex;
        }
    }

    return status;
}


/*---------------------------------------------------------------------------------
-   函 数 名: InitMonitor
-   功能描述: 初始化监控
-   输    入: pstMonitor
-   返    回:
-   全局变量:
-   注    释: 初始化监控
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitMonitor(DDS_Monitor* pstMonitor)
{
    pstMonitor->pNext = NULL;
    pstMonitor->deadtime = time(NULL);
    pstMonitor->unicastLocator = NULL;
    pstMonitor->pstTopicMon = NULL;
}

/*---------------------------------------------------------------------------------
-   函 数 名: UninitMonitor
-   功能描述: 析构监控
-   输    入: pstMonitor
-   返    回:
-   全局变量:
-   注    释: 析构监控
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UninitMonitor(DDS_Monitor* pstMonitor)
{
    Locator_t*  unicastLocator = NULL;
    DDS_TopicMon* pstTopicMon = NULL;;
    LIST_DELETE_ALL(pstMonitor->unicastLocator, unicastLocator);
    LIST_DELETE_ALL(pstMonitor->pstTopicMon, pstTopicMon);
}