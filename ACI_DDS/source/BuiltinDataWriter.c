#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: InitBuiltinDataWriter
-   功能描述: 内建写入器初始化
-   输    入: 内建写入器
-   返    回: 
-   全局变量:
-   注    释: 内建写入器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitBuiltinDataWriter(BuiltinDataWriter* pstBuiltinDataWriter)
{
    if (NULL != pstBuiltinDataWriter)
    {
        pstBuiltinDataWriter->pstBuiltinDiscReader = NULL;
        pstBuiltinDataWriter->stHeartbeat.count = 0;
        pstBuiltinDataWriter->seqNum = SEQUENCENUMBER_ZERO;
        InitHistoryCache(&pstBuiltinDataWriter->stHistoryCache);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: UnInitBuiltinDiscReader
-   功能描述: 内建写入器初始化
-   输    入: 内建写入器
-   返    回:
-   全局变量:
-   注    释: 内建写入器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UnInitBuiltinDiscReader(BuiltinDiscReader*  pstBuiltinDiscReader)
{
    if (NULL != pstBuiltinDiscReader)
    {
        //Locator_t* pstTempNode = NULL;

        ///* 析构单播地址 */
        //LIST_DELETE_ALL(pstBuiltinDiscReader->pstUnicastLocator, pstTempNode);

        ///* 析构组播地址 */
        //LIST_DELETE_ALL(pstBuiltinDiscReader->pstMulticastLocator, pstTempNode);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: UnInitBuiltinDataWriter
-   功能描述: 内建写入器初始化
-   输    入: 内建写入器
-   返    回:
-   全局变量:
-   注    释: 内建写入器初始化
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\03\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UnInitBuiltinDataWriter(BuiltinDataWriter* pstBuiltinDataWriter)
{
    if (NULL != pstBuiltinDataWriter)
    {
        UninitHistoryCache(&pstBuiltinDataWriter->stHistoryCache);
        BuiltinDiscReader*  pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
        while (NULL != pstBuiltinDiscReader)
        {
            pstBuiltinDataWriter->pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
            UnInitBuiltinDiscReader(pstBuiltinDiscReader);
            DDS_STATIC_FREE(pstBuiltinDiscReader);

            pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
        }
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointSEDPDataMsgSendALLBuiltinReader
-   功能描述: 组装SEDP报文发送所有远端内建阅读器
-   输    入: RTPS头、远端阅读器EntityId、缓存数据、本地内建写入器
-   返    回:
-   全局变量:
-   注    释: 组装SEDP报文发送所有远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointSEDPDataMsgSendALLBuiltinReader(RTPSMessageHeader* pstRTPSMsgHeader, HistoryData* pstHistoryData, BuiltinDataWriter* pstBuiltinDataWriter)
{
    MemoryBlock       stMemBlock;
    MemoryBlock       stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    ParameterHead     stParamHead;
    InfoTimestamp     stInfoTS;
    InfoDestination   stInfoDest;
    Data              stData;
    CHAR              buff[NETWORK_BYTE_MAX_SIZE];

    /* 网络字节序初始化，保存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* data报文头设置 */
    stSubMsgHeader.submessageId = DATA;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data报文体设置 */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_UNKNOWN;
    stData.writerId = pstBuiltinDataWriter->guid.entityId;
    stData.writerSN = pstHistoryData->seqNum;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_DATA(stData, stMemBlock);

    /* 序列化参数列表 */
    stParamHead.paramType = PID_KEY_HASH;
    stParamHead.length = sizeof(GUID_t);
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
    SERIALIZE_GUID(pstHistoryData->stGuid, stMemBlock);

    /* 序列化参数列表结尾 */
    stParamHead.paramType = PID_SENTINEL;
    stParamHead.length = 0;
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

    /* 序列化的有效载荷 */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

    /* 序列化写入器信息 */
    PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* 发送发布报文 */
    BuiltinDiscReader*  pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
    while (NULL != pstBuiltinDiscReader)
    {
        /* 修改目的地GuidPrefix */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));

        SERIALIZE_GUID_PREFIX(pstBuiltinDiscReader->guid.prefix, stMemBlockMod);

        /* 修改目的地GuidEntityId */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));

        SERIALIZE_ENTITYID(pstBuiltinDiscReader->guid.entityId, stMemBlockMod);

        RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);

        pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointSEDPDataMsgSendAssignBuiltinReader
-   功能描述: 组装SEDP报文发送指定远端内建阅读器
-   输    入: RTPS头、远端阅读器EntityId、缓存数据、本地内建写入器、远端内建阅读器
-   返    回:
-   全局变量:
-   注    释: 组装SEDP报文发送指定远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointSEDPDataMsgSendAssignBuiltinReader(RTPSMessageHeader* pstRTPSMsgHeader, HistoryData* pstHistoryData, BuiltinDataWriter* pstBuiltinDataWriter, BuiltinDiscReader*  pstBuiltinDiscReader)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    ParameterHead     stParamHead;
    InfoTimestamp     stInfoTS;
    InfoDestination   stInfoDest;
    Data              stData;
    CHAR              buff[NETWORK_BYTE_MAX_SIZE];

    /* 网络字节序初始化，保存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = pstBuiltinDiscReader->guid.prefix;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* data报文头设置 */
    stSubMsgHeader.submessageId = DATA;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = TRUE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = TRUE;    //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data报文体设置 */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = pstBuiltinDiscReader->guid.entityId;
    stData.writerId = pstBuiltinDataWriter->guid.entityId;
    stData.writerSN = pstHistoryData->seqNum;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_DATA(stData, stMemBlock);

    /* 序列化参数列表 */
    stParamHead.paramType = PID_KEY_HASH;
    stParamHead.length = sizeof(GUID_t);
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
    SERIALIZE_GUID(pstHistoryData->stGuid, stMemBlock);

    /* 序列化参数列表结尾 */
    stParamHead.paramType = PID_SENTINEL;
    stParamHead.length = 0;
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

    /* 序列化的有效载荷 */
    SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

    /* 序列化写入器信息 */
    PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointSEDPDataMsgSendBuiltinReader
-   功能描述: 组装SEDP报文发送远端内建阅读器
-   输    入: RTPS头、缓存数据、本地内建写入器、远端内建阅读器
-   返    回:
-   全局变量:
-   注    释: 组装SEDP报文发送指定远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointSEDPDataMsgSendBuiltinReader(RTPSMessageHeader* pstRTPSMsgHeader, HistoryData* pstHistoryData, BuiltinDataWriter* pstBuiltinDataWriter, BuiltinDiscReader*  pstBuiltinDiscReader)
{
    MemoryBlock       stMemBlock;
    MemoryBlock       stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    ParameterHead     stParamHead;
    InfoTimestamp     stInfoTS;
    InfoDestination   stInfoDest;
    Data              stData;
    CHAR              buff[NETWORK_BYTE_MAX_SIZE];

    /* 网络字节序初始化，保存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* data报文头设置 */
    stSubMsgHeader.submessageId = DATA;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
    stSubMsgHeader.flags[2] = (pstHistoryData->data) ? TRUE : FALSE;  //dataPresent
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag

    /* data报文体设置 */
    stData.extraFlags[0] = 0x00;
    stData.extraFlags[1] = 0x00;
    stData.octetsToInlineQos = 0x0010;
    stData.readerId = ENTITYID_UNKNOWN;
    stData.writerId = pstBuiltinDataWriter->guid.entityId;
    stData.writerSN = pstHistoryData->seqNum;
    stData.serializedPayload.m_cdrType = PL_CDR_LE;
    stData.serializedPayload.m_option = 0x0000;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_DATA(stData, stMemBlock);

    /* 序列化参数列表 */
    stParamHead.paramType = PID_KEY_HASH;
    stParamHead.length = sizeof(GUID_t);
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
    SERIALIZE_GUID(pstHistoryData->stGuid, stMemBlock);

    /* 序列化在线QOS取消发布订阅 */
    if (pstHistoryData->bCancel)
    {
        /* 序列化参数列表 */
        UINT32 Flags = 0x01000000;
        stParamHead.paramType = PID_STATUS_INFO;
        stParamHead.length = sizeof(UINT32);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        PUT_UNSIGNED_INT(stMemBlock, Flags);
    }

    /* 序列化参数列表结尾 */
    stParamHead.paramType = PID_SENTINEL;
    stParamHead.length = 0;
    SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

    /* 序列化用户数据 */
    if (pstHistoryData->data)
    {
        /* 序列化的有效载荷 */
        SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

        /* 序列化写入器信息 */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);
    }

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    /* 发送到指定对端阅读器 */
    if (NULL != pstBuiltinDiscReader)
    {
        /* 修改目的地GuidPrefix */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
        SERIALIZE_GUID_PREFIX(pstBuiltinDiscReader->guid.prefix, stMemBlockMod);

        /* 修改目的地GuidEntityId */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
        SERIALIZE_ENTITYID(pstBuiltinDiscReader->guid.entityId, stMemBlockMod);

        RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);
        return;
    }

    /* 发送到所有对端阅读器 */
    pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
    while (NULL != pstBuiltinDiscReader)
    {
        /* 修改目的地GuidPrefix */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
        SERIALIZE_GUID_PREFIX(pstBuiltinDiscReader->guid.prefix, stMemBlockMod);

        /* 修改目的地GuidEntityId */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
        SERIALIZE_ENTITYID(pstBuiltinDiscReader->guid.entityId, stMemBlockMod);

        RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);
        pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
    }

    return;
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointHeartbeatMsgSendAllBuiltinReader
-   功能描述: 组装心跳报文发送指定内建阅读器
-   输    入: 本地DomainParticipant指针
-   返    回:
-   全局变量:
-   注    释: 组装心跳报文发送指定内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointHeartbeatMsgSendAllBuiltinReader(RTPSMessageHeader* pstRTPSMsgHeader, BuiltinDataWriter* pstBuiltinDataWriter)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    InfoDestination   stInfoDest;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* 本地无缓存，不发送心跳 */
    if (NULL == pstBuiltinDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* 网络字节序初始化，缓存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* InfoDT报文 */
    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* 心跳报文头 */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;

    /* 心跳报文消息体 */
    pstBuiltinDataWriter->stHeartbeat.readerId = ENTITYID_UNKNOWN;
    pstBuiltinDataWriter->stHeartbeat.writerId = pstBuiltinDataWriter->guid.entityId;
    pstBuiltinDataWriter->stHeartbeat.firstSN = pstBuiltinDataWriter->stHistoryCache.pstHead->seqNum;
    pstBuiltinDataWriter->stHeartbeat.lastSN = pstBuiltinDataWriter->stHistoryCache.pstTail->seqNum;
    pstBuiltinDataWriter->stHeartbeat.count++;

    /* 序列化用户数据子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化用户数据子报文消息体，同时探针记录readerId位置 */
    SERIALIZE_HEARTBEAT(pstBuiltinDataWriter->stHeartbeat, stMemBlock);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstBuiltinDataWriter->stHeartbeat.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));
    
    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

    BuiltinDiscReader* pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
    while (NULL != pstBuiltinDiscReader)
    {
		if (pstBuiltinDiscReader->livelinessDuration.sec == 0 && pstBuiltinDiscReader->livelinessDuration.nanosec == 0)
		{
			/* 断开连接，清除所有远端信息 */
			if (HEARTBEAT_DISCONNECT < pstBuiltinDiscReader->uiSendHeartBeatNum)// || Compare_Duration(usedTime,pstBuiltinDataWriter->pstParticipant->durationEdge))
			{
				DDS_DiscoveredParticipant* pstDiscParticipant = pstBuiltinDataWriter->pstParticipant->pstDiscParticipant;
				while (pstDiscParticipant != NULL)
				{
					if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstBuiltinDiscReader->guid.prefix))
					{
						int k[4];
						UINT32 addr = pstDiscParticipant->IPAddr;
						k[0] = addr & 0xFF;
						addr >>= 8;
						k[1] = addr & 0xFF;
						addr >>= 8;
						k[2] = addr & 0xFF;
						addr >>= 8;
						k[3] = addr & 0xFF;
						printf("The remote participant disconnect! IPAddr: %d.%d.%d.%d, ProcessID: %d.\n", k[0], k[1], k[2], k[3], pstDiscParticipant->processID);
                        DeleteALLDiscoveredFromByGuidPrefix(pstBuiltinDataWriter->pstParticipant, pstBuiltinDiscReader->guid.prefix);
						break;
					}
					pstDiscParticipant = pstDiscParticipant->pNext;
				}
				return;
			}
		}

        /* 序列化修改子infoDT */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
        SERIALIZE_GUID_PREFIX(pstBuiltinDiscReader->guid.prefix, stMemBlockMod);

        /* 序列化修改心跳readId*/
        INITIAL_MEMORYBLOCK(stMemBlockMod, pstBuiltinDataWriter->stHeartbeat.pcReadId, sizeof(EntityId_t));
        SERIALIZE_ENTITYID(pstBuiltinDiscReader->guid.entityId, stMemBlockMod);

        /* 发送到对应的远端阅读器 */
        RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);

        /* 每发送一次，心跳累积一次 */
        pstBuiltinDiscReader->uiSendHeartBeatNum++;
        
        pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: JointHeartbeatMsgSendAssignBuiltinReader
-   功能描述: 组装心跳报文发送指定内建阅读器
-   输    入: 本地内建写入器、远端内建阅读器
-   返    回:
-   全局变量:
-   注    释: 组装心跳报文发送指定远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointHeartbeatMsgSendAssignBuiltinReader(RTPSMessageHeader* pstRTPSMsgHeader, BuiltinDataWriter* pstBuiltinDataWriter, BuiltinDiscReader* pstBuiltinDiscReader)
{
    MemoryBlock stMemBlock;
    MemoryBlock stMemBlockMod;
    SubMessageHeader  stSubMsgHeader;
    InfoTimestamp     stInfoTS;
    InfoDestination   stInfoDest;
    CHAR buff[NETWORK_BYTE_MAX_SIZE];

    /* 本地无缓存，不发送心跳 */
    if (NULL == pstBuiltinDataWriter->stHistoryCache.pstHead)
    {
        return;
    }

    /* 网络字节序初始化，缓存发布报文 */
    INITIAL_MEMORYBLOCK(stMemBlock, buff, NETWORK_BYTE_MAX_SIZE);

    /* 第一步序列化RTPS报文头 */
    SERIALIZE_RTPS_MESSAGE_HEADER((*pstRTPSMsgHeader), stMemBlock);

    /* 时间报文头设置 */
    stSubMsgHeader.submessageId = INFO_TS;
    stSubMsgHeader.submessageLength = sizeof(Time_t);
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
    stSubMsgHeader.flags[2] = FALSE;   //dataFlag
    stSubMsgHeader.flags[3] = FALSE;   //keyFlag
    stInfoTS.value = GetNowTime();

    /* 序列化时间报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化时间子报文体 */
    SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

    /* InfoDT报文 */
    stSubMsgHeader.submessageId = INFO_DST;
    stSubMsgHeader.submessageLength = 12;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;
    stInfoDest.guidPrefix = pstBuiltinDiscReader->guid.prefix;

    /* 第二步序列化子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 第三步序列化子报文消息体 */
    SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

    /* 心跳报文头 */
    stSubMsgHeader.submessageId = HEARTBEAT;
    stSubMsgHeader.flags[0] = TRUE;    //littleEndian
    stSubMsgHeader.flags[1] = FALSE;
    stSubMsgHeader.flags[2] = FALSE;
    stSubMsgHeader.flags[3] = FALSE;

    /* 心跳报文消息体 */
    pstBuiltinDataWriter->stHeartbeat.readerId = pstBuiltinDiscReader->guid.entityId;
    pstBuiltinDataWriter->stHeartbeat.writerId = pstBuiltinDataWriter->guid.entityId;
    pstBuiltinDataWriter->stHeartbeat.firstSN = pstBuiltinDataWriter->stHistoryCache.pstHead->seqNum;
    pstBuiltinDataWriter->stHeartbeat.lastSN = pstBuiltinDataWriter->stHistoryCache.pstTail->seqNum;
    pstBuiltinDataWriter->stHeartbeat.count++;

    /* 序列化用户数据子报文头 */
    SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

    /* 序列化用户数据子报文消息体，同时探针记录readerId位置 */
    SERIALIZE_HEARTBEAT(pstBuiltinDataWriter->stHeartbeat, stMemBlock);

    /* 计算子报文消息体长度，不带消息头 */
    stSubMsgHeader.submessageLength = stMemBlock.writeIndex - pstBuiltinDataWriter->stHeartbeat.uiStartIndex;

    /* 序列化修改子报文长度 */
    INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

    PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);
	if (pstBuiltinDiscReader->livelinessDuration.sec == 0 && pstBuiltinDiscReader->livelinessDuration.nanosec == 0)
	{
		if (HEARTBEAT_DISCONNECT < pstBuiltinDiscReader->uiSendHeartBeatNum)// || Compare_Duration(usedTime, pstBuiltinDataWriter->pstParticipant->durationEdge))
		{
			DDS_DiscoveredParticipant* pstDiscParticipant = pstBuiltinDataWriter->pstParticipant->pstDiscParticipant;
			while (pstDiscParticipant != NULL)
			{
				if (GuidPrefix_Is_Equal(&pstDiscParticipant->stRTPSMsgHeader.guidPrefix, &pstBuiltinDiscReader->guid.prefix))
				{
					int k[4];
					UINT32 addr = pstDiscParticipant->IPAddr;
					k[0] = addr & 0xFF;
					addr >>= 8;
					k[1] = addr & 0xFF;
					addr >>= 8;
					k[2] = addr & 0xFF;
					addr >>= 8;
					k[3] = addr & 0xFF;
					printf("The remote participant disconnect! IPAddr: %d.%d.%d.%d, ProcessID: %d.\n", k[0], k[1], k[2], k[3], pstDiscParticipant->processID);
                    DeleteALLDiscoveredFromByGuidPrefix(pstBuiltinDataWriter->pstParticipant, pstBuiltinDiscReader->guid.prefix);
					break;
				}
				pstDiscParticipant = pstDiscParticipant->pNext;
			}
			return;
		}
	}
	/* 断开连接，清除所有远端信息 */
	

    RTPSSendMsg(&stMemBlock, &pstBuiltinDataWriter->pstParticipant->socketList.sedp_data_sender, pstBuiltinDiscReader->pstUnicastLocator);

    /* 每发送一次，心跳累积一次 */
    pstBuiltinDiscReader->uiSendHeartBeatNum++;
}

/*---------------------------------------------------------------------------------
-   函 数 名: RecoverBuiltinWriterMissSeqNum
-   功能描述: 复原丢失分片，进行网络重传
-   输    入: 本地内建写入器、远端内建阅读器，ack报文
-   返    回:
-   全局变量:
-   注    释: 复原内建丢失分片，进行网络重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\14       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID RecoverBuiltinWriterMissSeqNum(BuiltinDataWriter* pstBuiltinDataWriter, BuiltinDiscReader* pstBuiltinDiscReader, AckNack* pstAckNackMsg)
{
    SequenceNumber_t seqNumMiss = SEQUENCENUMBER_ZERO;
    SequenceNumber_t seqNumOffset = SEQUENCENUMBER_ZERO;
    DDS_ReturnCode_t returnCode = DDS_RETCODE_OK;
    const UINT32  bitStart = 0x80000000;
    UINT32 uiNumbit = 0;

    for (uiNumbit = 0; uiNumbit < pstAckNackMsg->readerSNState.numBits; uiNumbit++)
    {
        /* 位运算计算bitMap中每一位值，大于0说明该位有效，通过偏移量计算出缺失 */
        if (0 < ((bitStart >> (uiNumbit >> 5))&(pstAckNackMsg->readerSNState.bitMap[uiNumbit >> 5])))
        {
            seqNumOffset.low = uiNumbit;
            seqNumMiss = Add_SequenceNumber(pstAckNackMsg->readerSNState.base, seqNumOffset);

            /* 缺失分片重发 */
            returnCode = BuiltinDataWriterSendCacheMsg(pstBuiltinDataWriter, pstBuiltinDiscReader, seqNumMiss);
            if (DDS_RETCODE_OK != returnCode)
            {
                //todo发送gap报文
                 //DataWriterSendGapMsg(pstBuiltinDataWriter, pstBuiltinDiscReader, seqNumMiss);
//                 printf("gap send 772\n");
            }
        }
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: BuiltinDataWriterInsertDiscDataReader
-   功能描述: 内建写入器初插入远端阅读器
-   输    入: 内建写入器、远端guid、地址
-   返    回:
-   全局变量:
-   注    释: 内建写入器初插入远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL BuiltinDataWriterInsertDiscDataReader(BuiltinDataWriter* pstBuiltinDataWriter, GUID_t* pstGuid, Locator_t* pstUnicastLocator)
{
    if (NULL != pstBuiltinDataWriter)
    {
        /* 申请静态内存创建内建远端阅读器 */
        BuiltinDiscReader*  pstBuiltinDiscReader = (BuiltinDiscReader*)DDS_STATIC_MALLOC(sizeof(BuiltinDiscReader));
        if (NULL == pstBuiltinDiscReader)
        {
            PrintLog(COMMLOG_ERROR, "BuiltinDiscReader: Failed to create BuiltinDiscReader, Insufficient memory space.\n");
            return FALSE;
        }

        pstBuiltinDiscReader->guid = *pstGuid;
        pstBuiltinDiscReader->stAckNack.count = 0;
        pstBuiltinDiscReader->uiSendHeartBeatNum = 0;
		pstBuiltinDiscReader->livelinessDuration.sec = 0;
		pstBuiltinDiscReader->livelinessDuration.nanosec = 0;
        pstBuiltinDiscReader->stAckNack.readerSNState.base = SEQUENCENUMBER_ZERO;
        pstBuiltinDiscReader->pstUnicastLocator = pstUnicastLocator;

        /* 链表头插 */
        LIST_INSERT_HEAD(pstBuiltinDataWriter->pstBuiltinDiscReader, pstBuiltinDiscReader);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetBuiltinDiscReaderFromBuiltinDataWriterByPrefix
-   功能描述: 查找远端内建阅读器
-   输    入: 内建写入器、EntityId
-   返    回:
-   全局变量:
-   注    释: 查找远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BuiltinDiscReader* GetBuiltinDiscReaderFromBuiltinDataWriterByPrefix(BuiltinDataWriter* pstBuiltinDataWriter, GuidPrefix_t* pstPrefix)
{
    BuiltinDiscReader* pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;

    while (NULL != pstBuiltinDiscReader)
    {
        if (GuidPrefix_Is_Equal(&pstBuiltinDiscReader->guid.prefix, pstPrefix))
        {
            return pstBuiltinDiscReader;
        }
        pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
    }

    return NULL;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DataWriterSendCacheMsg
-   功能描述: 内建缓存数据重传
-   输    入: 写入器、远端阅读器，缺失seqNum
-   返    回: DDS_RETCODE_OK、DDS_RETCODE_ERROR
-   全局变量:
-   注    释: 内建缓存数据重传
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\20       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_ReturnCode_t BuiltinDataWriterSendCacheMsg(BuiltinDataWriter* pstBuiltinDataWriter, BuiltinDiscReader* pstBuiltinDiscReader, SequenceNumber_t seqNum)
{
    /* 本地无缓存数据或者ack报文无丢失，直接返回不处理 */
    if (NULL == pstBuiltinDataWriter->stHistoryCache.pstHead)
    {
        return DDS_RETCODE_ERROR;
    }

    /* 缺失报文不在缓存区间，直接返回不处理 */
    if (ISLESSTHEN_SEQUENCENUMBER(pstBuiltinDataWriter->stHistoryCache.pstTail->seqNum, seqNum) || ISLESSTHEN_SEQUENCENUMBER(seqNum, pstBuiltinDataWriter->stHistoryCache.pstHead->seqNum))
    {
        return DDS_RETCODE_ERROR;
    }

    HistoryData* pstHistoryData = pstBuiltinDataWriter->stHistoryCache.pstHead;

    while (NULL != pstHistoryData)
    {
        if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, seqNum))
        {
            /* 组装重传报文并发送 */
            JointSEDPDataMsgSendBuiltinReader(&pstBuiltinDataWriter->pstParticipant->stRTPSMsgHeader, pstHistoryData, pstBuiltinDataWriter, pstBuiltinDiscReader);

            return DDS_RETCODE_OK;
        }
        pstHistoryData = pstHistoryData->pNext;
    }

    return DDS_RETCODE_ERROR;
}

/*---------------------------------------------------------------------------------
-   函 数 名: ModifySendDiscReaderHeartbeat
-   功能描述: 修改心跳机制
-   输    入: 内建写入器、标志位
-   返    回: 
-   全局变量:
-   注    释: 修改心跳机制
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\27       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID ModifySendDiscReaderHeartbeat(BuiltinDataWriter* pstBuiltinDataWriter, BOOL bFlag)
{
    BuiltinDiscReader* pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;
    while (NULL != pstBuiltinDiscReader)
    {
        pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DeleteBuiltinDiscReaderFromBuiltinWriterByGuidPrefix
-   功能描述: 删除远端内建阅读器
-   输    入: 主题、Guid
-   返    回:
-   全局变量:
-   注    释: 主题中删除指定远端内建阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DeleteBuiltinDiscReaderFromBuiltinWriterByGuidPrefix(BuiltinDataWriter* pstBuiltinDataWriter, GuidPrefix_t*  pstPrefix)
{
    BuiltinDiscReader*  pstPrevBuiltinDiscReader = NULL;
    BuiltinDiscReader*  pstBuiltinDiscReader = pstBuiltinDataWriter->pstBuiltinDiscReader;

    while (NULL != pstBuiltinDiscReader)
    {
        if (GuidPrefix_Is_Equal(&pstBuiltinDiscReader->guid.prefix, pstPrefix))
        {
            if (!pstPrevBuiltinDiscReader)
            {
                pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
                DDS_STATIC_FREE(pstBuiltinDataWriter->pstBuiltinDiscReader);
                pstBuiltinDataWriter->pstBuiltinDiscReader = pstBuiltinDiscReader;
            }
            else
            {
                pstPrevBuiltinDiscReader->pNext = pstBuiltinDiscReader->pNext;
                DDS_STATIC_FREE(pstBuiltinDiscReader);
                pstBuiltinDiscReader = pstPrevBuiltinDiscReader->pNext;
            }
        }
        else
        {
            pstPrevBuiltinDiscReader = pstBuiltinDiscReader;
            pstBuiltinDiscReader = pstBuiltinDiscReader->pNext;
        }
    }
}
