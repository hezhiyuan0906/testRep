#include "../Include/GlobalDefine.h"

#define DATA_SIZE                   (64 * 1024)       /* ��Ƭ������ */
#define FRAG_SIZE                   (64 * 1024 - 1024)   /* �û����ݸ����� */

/*---------------------------------------------------------------------------------
-   �� �� ��: JointUSERDataMsgSendUnknownReader
-   ��������: ��װ�û����ݱ��ķ���δָ���Ķ���
-   ��    ��: ����д����������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��װ�û����ݷ��ͱ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_ReturnCode_t JointUSERDataMsgSendUnknownReader(DDS_DataWriter* pstDataWriter, HistoryData* pstHistoryData)
{
    DDS_DiscoveredReader* pstDiscReader;
    MemoryBlock           stMemBlock;
    MemoryBlock           stMemBlockMod;
    SubMessageHeader      stSubMsgHeader;
    ParameterHead         stParamHead;
    InfoTimestamp         stInfoTS;
    InfoDestination       stInfoDest;
    DataFrag              stDataFrag;
    Data                  stData;
    CHAR                  cUserBuffer[DATA_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

	pstDiscReader = pstDataWriter->pstTopic->pstDiscReader;
	if (pstDiscReader == NULL)
	{
		return DDS_RETCODE_NO_AVAILABLE_READER;
	}

    /* ��Ƭ���޸� */
    /* �Ƿ���Ҫ��Ƭ���� */
    if (pstHistoryData->uiDataLen > FRAG_SIZE)
    {
        /* �����ֽ����ʼ�������淢������ */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

        /* ��һ�����л�RTPS����ͷ */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* ʱ�䱨��ͷ���� */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;
        /* ���л�ʱ�䱨��ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* ���л�ʱ���ӱ����� */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;
        /* �ڶ������л��ӱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* ���������л��ӱ�����Ϣ�� */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* �û����ݱ���ͷ���� */
        stSubMsgHeader.submessageId = DATA_FRAG;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* �û����ݱ��������� */
        stDataFrag.extraFlags[0] = 0x00;
        stDataFrag.extraFlags[1] = 0x00;
        stDataFrag.octetsToInlineQos = 0x0010;
        stDataFrag.readerId = ENTITYID_UNKNOWN;
        stDataFrag.writerId = pstDataWriter->guid.entityId;
        stDataFrag.writerSN = pstHistoryData->seqNum;
        
        stDataFrag.serializedPayload.m_cdrType = CDR_LE;  //С���û���������
        stDataFrag.serializedPayload.m_option = 0x0000;
        stDataFrag.fragmentStartingNum = pstHistoryData->pstFragData->usFragIndex;
        stDataFrag.fragmentsInSubmessage = pstHistoryData->uiFragTotal;
        stDataFrag.dataSize = pstHistoryData->uiDataLen;
        stDataFrag.fragmentSize = pstHistoryData->pstFragData->usDataLen;

        /* ���л��û����ݱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* ���л��û����ݱ����� */
        SERIALIZE_DATAFRAG(stDataFrag, stMemBlock);

        /* ���л������б� */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* ���л������б��β */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* ���л�����Ч�غ� */
        SERIALIZE_SERIALIZEDDATA(stDataFrag.serializedPayload, stMemBlock);

        /*for (int i = 0; i < pstHistoryData->uiFragLen; i++)
        {
            printf("%c", pstHistoryData->pstFragData[i]);
        }
        printf("\n");*/
        /* ���л��û��������� */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->pstFragData->data, pstHistoryData->pstFragData->usDataLen);

        /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stDataFrag.uiStartIndex;
        
        printf("seqNum: %u, submsgLength: %u\n", stDataFrag.writerSN.low, stSubMsgHeader.submessageLength);
        /* ���л��޸��ӱ��ĳ��� */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

        while (NULL != pstDiscReader)
        {
            if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
            {
                PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* �޸�Ŀ�ĵ�GuidPrefix */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
            SERIALIZE_GUID_PREFIX(pstDiscReader->guid.prefix, stMemBlockMod);

            ///* �޸�Ŀ�ĵ�GuidEntityId */
            //INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
            //SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
            pstDiscReader = pstDiscReader->pNext;
        }
        return DDS_RETCODE_OK;
    }
    else
    {
        /* �����ֽ����ʼ�������淢������ */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

        /* ��һ�����л�RTPS����ͷ */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* ʱ�䱨��ͷ���� */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;
        /* ���л�ʱ�䱨��ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* ���л�ʱ���ӱ����� */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = GUIDPREFIX_UNKNOWN;
        /* �ڶ������л��ӱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);
        /* ���������л��ӱ�����Ϣ�� */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* �û����ݱ���ͷ���� */
        stSubMsgHeader.submessageId = DATA;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;    //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* �û����ݱ��������� */
        stData.extraFlags[0] = 0x00;
        stData.extraFlags[1] = 0x00;
        stData.octetsToInlineQos = 0x0010;
        stData.readerId = ENTITYID_UNKNOWN;
        stData.writerId = pstDataWriter->guid.entityId;
        stData.writerSN = pstHistoryData->seqNum;
        stData.serializedPayload.m_cdrType = CDR_LE;  //С���û���������
        stData.serializedPayload.m_option = 0x0000;

        /* ���л��û����ݱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* ���л��û����ݱ����� */
        SERIALIZE_DATA(stData, stMemBlock);

        /* ���л������б� */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* ���л������б��β */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* ���л�����Ч�غ� */
        SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

        /* ���л��û��������� */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

        /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

        /* ���л��޸��ӱ��ĳ��� */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);


        while (NULL != pstDiscReader)
        {
            if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
            {
                PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
            {
                pstDiscReader = pstDiscReader->pNext;
                continue;
            }

            /* �޸�Ŀ�ĵ�GuidPrefix */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stInfoDest.pcGuidPre, sizeof(GuidPrefix_t));
            SERIALIZE_GUID_PREFIX(pstDiscReader->guid.prefix, stMemBlockMod);

            /* �޸�Ŀ�ĵ�GuidEntityId */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stData.pcReaderId, sizeof(EntityId_t));
            SERIALIZE_ENTITYID(pstDiscReader->guid.entityId, stMemBlockMod);

            /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
            pstDiscReader = pstDiscReader->pNext;
        }
        return DDS_RETCODE_OK;
#ifdef DDS_MONITOR
        /* ���ͼ����Ϣ */
        DDS_MUTEX_LOCK(pstParticipant->monMutex);
        UserMsgSendMonitor(pstParticipant, &pstDataWriter->pstTopic->tpGuid, &stMemBlock, 0x00);
        DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: JointUSERDataMsgSendAssignReader
-   ��������: ��װ�û����ݱ��ķ���ָ���Ķ���
-   ��    ��: ����д����������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��װ�û����ݱ��ķ���ָ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\7\24       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID JointUSERDataMsgSendAssignReader(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader*  pstDiscReader, HistoryData* pstHistoryData)
{
    MemoryBlock           stMemBlock;
    MemoryBlock           stMemBlockMod;
    SubMessageHeader      stSubMsgHeader;
    InfoTimestamp         stInfoTS;
    ParameterHead         stParamHead;
    DataFrag              stDataFrag;
    InfoDestination       stInfoDest;
    Data                  stData;
    USHORT                usFragSize;
    UINT32                uiFragNum;
    CHAR                  cUserBuffer[DATA_SIZE];

    DDS_DomainParticipant* pstParticipant = pstDataWriter->pstTopic->pstParticipant;

    /* �Ƿ���Ҫ��Ƭ���� */
    if (pstHistoryData->uiDataLen > FRAG_SIZE)
    {
        /* �����Ƭ���� */
        if (0 < pstHistoryData->uiDataLen % FRAG_SIZE)
        {
            stDataFrag.fragmentsInSubmessage = (pstHistoryData->uiDataLen / FRAG_SIZE) + 1;
        }
        else
        {
            stDataFrag.fragmentsInSubmessage = (pstHistoryData->uiDataLen / FRAG_SIZE);
        }

        for (uiFragNum = 0; uiFragNum < stDataFrag.fragmentsInSubmessage; uiFragNum++)
        {
            /* ���㵱ǰ��Ƭ���� */
            if (stDataFrag.fragmentsInSubmessage - 1 == uiFragNum)
            {
                usFragSize = pstHistoryData->uiDataLen - (uiFragNum * FRAG_SIZE);
            }
            else
            {
                usFragSize = FRAG_SIZE;
            }
            /* �����ֽ����ʼ�������淢������ */
            INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, DATA_SIZE);

            /* ��һ�����л�RTPS����ͷ */
            SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

            stSubMsgHeader.flags[0] = TRUE;    //littleEndian
            stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
            stSubMsgHeader.flags[2] = TRUE;    //dataFlag
            stSubMsgHeader.flags[3] = FALSE;   //keyFlag

            /* ���л�ʱ�� */
            stSubMsgHeader.submessageId = INFO_TS;
            stSubMsgHeader.submessageLength = sizeof(Time_t);
            stInfoTS.value = pstHistoryData->timeInfo;

            /* ���л�ʱ���ӱ���ͷ */
            SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

            /* ���л�ʱ���ӱ�����Ϣ�� */
            SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

            /* ���л��û����� */
            stSubMsgHeader.submessageId = DATA_FRAG;
            stDataFrag.extraFlags[0] = 0x00;
            stDataFrag.extraFlags[1] = 0x00;
            stDataFrag.octetsToInlineQos = 0x0010;
            stDataFrag.readerId = pstDiscReader->guid.entityId;;
            stDataFrag.writerId = pstDataWriter->guid.entityId;
            stDataFrag.writerSN = pstHistoryData->seqNum;
            stDataFrag.fragmentStartingNum = uiFragNum + 1;
            stDataFrag.dataSize = pstHistoryData->uiDataLen;
            stDataFrag.fragmentSize = usFragSize;

            /* ���л��û������ӱ���ͷ */
            SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

            /* ���л��û������ӱ�����Ϣ�壬ͬʱ̽���¼readerIdλ�� */
            SERIALIZE_DATAFRAG(stDataFrag, stMemBlock);

            /* dataFlag����keyFlagΪTRUE��������� */
            SERIALIZE_SERIALIZEDDATA(stDataFrag.serializedPayload, stMemBlock);

            /* ���л��û��������� */
            PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data + (uiFragNum * FRAG_SIZE), usFragSize);

            /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
            stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stDataFrag.uiStartIndex;

            /* ���л��޸��ӱ��ĳ��� */
            INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

            PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

            /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
            RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
        }
    }
    else
    {
        /* �����ֽ����ʼ�������淢������ */
        INITIAL_MEMORYBLOCK(stMemBlock, cUserBuffer, FRAG_SIZE);

        /* ��һ�����л�RTPS����ͷ */
        SERIALIZE_RTPS_MESSAGE_HEADER(pstParticipant->stRTPSMsgHeader, stMemBlock);

        /* ʱ�䱨��ͷ���� */
        stSubMsgHeader.submessageId = INFO_TS;
        stSubMsgHeader.submessageLength = sizeof(Time_t);
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = FALSE;   //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag
        stInfoTS.value = pstHistoryData->timeInfo;

        /* ���л�ʱ�䱨��ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* ���л�ʱ���ӱ����� */
        SERIALIZE_INFOTIMESTAMP(stInfoTS, stMemBlock);

        /* InfoDT���� */
        stSubMsgHeader.submessageId = INFO_DST;
        stSubMsgHeader.submessageLength = 12;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = FALSE;
        stSubMsgHeader.flags[2] = FALSE;
        stSubMsgHeader.flags[3] = FALSE;
        stInfoDest.guidPrefix = pstDiscReader->guid.prefix;

        /* �ڶ������л��ӱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* ���������л��ӱ�����Ϣ�� */
        SERIALIZE_INFODESTINATION(stInfoDest, stMemBlock);

        /* �û����ݱ���ͷ���� */
        stSubMsgHeader.submessageId = DATA;
        stSubMsgHeader.flags[0] = TRUE;    //littleEndian
        stSubMsgHeader.flags[1] = TRUE;   //inlineQosFlag
        stSubMsgHeader.flags[2] = TRUE;    //dataFlag
        stSubMsgHeader.flags[3] = FALSE;   //keyFlag

        /* �û����ݱ��������� */
        stData.extraFlags[0] = 0x00;
        stData.extraFlags[1] = 0x00;
        stData.octetsToInlineQos = 0x0010;
        stData.readerId = pstDiscReader->guid.entityId;
        stData.writerId = pstDataWriter->guid.entityId;
        stData.writerSN = pstHistoryData->seqNum;
        stData.serializedPayload.m_cdrType = CDR_LE;  //С���û���������
        stData.serializedPayload.m_option = 0x0000;

        /* ���л��û����ݱ���ͷ */
        SERIALIZE_SUBMESSAGEHEADER(stSubMsgHeader, stMemBlock);

        /* ���л��û����ݱ����� */
        SERIALIZE_DATA(stData, stMemBlock);

        /* ���л������б� */
        stParamHead.paramType = PID_KEY_HASH;
        stParamHead.length = sizeof(GUID_t);
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);
        SERIALIZE_GUID(pstDataWriter->guid, stMemBlock);

        /* ���л������б��β */
        stParamHead.paramType = PID_SENTINEL;
        stParamHead.length = 0;
        SERIALIZE_PARAM_HEAD(stParamHead, stMemBlock);

        /* ���л�����Ч�غ� */
        SERIALIZE_SERIALIZEDDATA(stData.serializedPayload, stMemBlock);

        /* ���л��û��������� */
        PUT_BYTE_ARRAY(stMemBlock, pstHistoryData->data, pstHistoryData->uiDataLen);

        /* �����ӱ�����Ϣ�峤�ȣ�������Ϣͷ */
        stSubMsgHeader.submessageLength = stMemBlock.writeIndex - stData.uiStartIndex;

        /* ���л��޸��ӱ��ĳ��� */
        INITIAL_MEMORYBLOCK(stMemBlockMod, stSubMsgHeader.pcLenFlag, sizeof(USHORT));

        PUT_UNSIGNED_SHORT(stMemBlockMod, stSubMsgHeader.submessageLength);

        if (BEST_EFFORT_RELIABILITY_QOS == pstDataWriter->stDataWriterQos.reliability.kind && RELIABLE_RELIABILITY_QOS == pstDiscReader->stDataReaderQos.reliability.kind)
        {
            PrintLog(COMMLOG_WARN, "Reader and writer RELIABILITY_QOS do not match.\n");
            return;
        }

        if (!pstParticipant->bLoopBack && GuidPrefix_Is_Equal(&pstParticipant->stRTPSMsgHeader.guidPrefix, &pstDiscReader->guid.prefix))
        {
            return;
        }

        /* ���͵���Ӧ��ÿ��Զ���Ķ��� */
        RTPSSendMsg(&stMemBlock, &pstParticipant->socketList.user_data_sender, pstDiscReader->pstUnicastLocator);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: HandleUSERWriterDataMsg
-   ��������: �����û����ݱ���
-   ��    ��: ����DomainParticipantָ��
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �����û����ݱ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\13       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL HandleUSERWriterDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* ��ȡԶ��д����guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstDataMsg->writerId;

    /* ����qos��ʱ������ֱ�ӹ���*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN , 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* �ж��Ƿ�Ϊ�û���Ϣ*/
    if (!(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: There are no UserData!\n");
        return FALSE;
    }

    /* ���Ҹ��û����ݶ�Ӧ��Զ��д���� */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
		pstMemBlock->readIndex = pstMemBlock->totalSize;
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }


    
#ifdef DDS_MONITOR
    /* ���ͼ����Ϣ */
    DDS_MUTEX_LOCK(pstParticipant->monMutex);
    UserMsgSendMonitor(pstParticipant, &pstDiscWriter->pstTopic->tpGuid, pstMemBlock, 0x01);
    DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    /* �������ݽṹ�ڴ����� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData, Insufficient memory space.\n");
        return FALSE;
    }

    /* �����л�װ����Ϣ */
    DESERIALIZE_SERIALIZEDDATA(pstDataMsg->serializedPayload, (*pstMemBlock));

    if (pstMemBlock->totalSize < pstMemBlock->readIndex)
    {
        return FALSE;
    }

    /* �û�����ʱ��� */
    pstHistoryData->timeInfo = pstTimestamp->value;
	pstHistoryData->uiFragTotal = 0;
	pstHistoryData->uiFragNum = 0;

    /* �����û����ݳ��� */
    pstHistoryData->uiDataLen = pstMemBlock->totalSize - pstMemBlock->readIndex;

    /* �û������ڴ����� */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(pstHistoryData->uiDataLen);
    if (NULL == pstHistoryData->data)
    {
        /* �û����������ڴ�ʧ�����ͷ�֮ǰ���� */
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData->data, Insufficient memory space.\n");
        return FALSE;
    }

    /* ��¼��ǰ����seqNum */
    pstHistoryData->seqNum = pstDataMsg->writerSN;

    /* �����л��û����� */
    GET_CHAR_ARRAY((*pstMemBlock), pstHistoryData->data, (INT)pstHistoryData->uiDataLen);

    /* �������seqNumС����������Ϊ�������ݣ�ֱ������ */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataMsg->writerSN, pstDiscWriter->stAckNack.readerSNState.base))
    {
        if (strcmp(pstHistoryData->data, "DeadLine MSG.") == 0)
        {
            DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        }
        return TRUE;
    }

    /* ����Ϊ��״̬�Ķ���������Զ��д����״̬��ֱ�������ύ */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        DeilverStatuslessUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        return TRUE;
    }

    /* �����Ķ���Ϊ��״̬��Զ��д������״̬����ƥ�䣬ֱ������ */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg:Reader and writer states do not match.\n");
        return TRUE;
    }

    /* ������Զ�˶�Ϊ��״̬�ύ���� */
    DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);

    return TRUE;
}

BOOL HandleUSERWriterDataFrag(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, DataFrag* pstDataFrag, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* ��ȡԶ��д����guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstDataFrag->writerId;

    /* ����qos��ʱ������ֱ�ӹ���*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN, 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* �ж��Ƿ�Ϊ�û���Ϣ*/
    if (!(pstSubMsgHeader->flags[2] || pstSubMsgHeader->flags[3]))
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: There are no UserData!\n");
        return FALSE;
    }

    /* ���Ҹ��û����ݶ�Ӧ��Զ��д���� */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }

    /* û��ָ���Ķ���Id��������Ϊͬһ�����µ��Ķ��� */
    //if (EntityId_Is_Equal(&ENTITYID_UNKNOWN, &pstDataMsg->readerId))
    //{
    //    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    //    if (NULL == pstDataReader)
    //    {
    //        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg: Cannot discovered local dataReader.\n");
    //        return FALSE;
    //    }
    //}
    //else
    //{
    //    /* ָ�����Ķ���id����ҪУ���Ƿ�һ�� */
    //    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    //    while (NULL != pstDataReader)
    //    {
    //        if (EntityId_Is_Equal(&pstDataReader->guid.entityId, &pstDataMsg->readerId))
    //        {
    //            break;
    //        }
    //        pstDataReader = pstDataReader->pNext;
    //    }

    //    if (NULL == pstDataReader)
    //    {
    //        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg: Cannot discovered local dataReader.\n");
    //        return FALSE;
    //    }
    //}

    /* �������seqNumС����������Ϊ�������ݣ�ֱ������ */
    if (ISLESSTHEN_SEQUENCENUMBER(pstDataFrag->writerSN, pstDiscWriter->stAckNack.readerSNState.base))
    {
        return TRUE;
    }

#ifdef DDS_MONITOR
    /* ���ͼ����Ϣ */
    DDS_MUTEX_LOCK(pstParticipant->monMutex);
    UserMsgSendMonitor(pstParticipant, &pstDiscWriter->pstTopic->tpGuid, pstMemBlock, 0x01);
    DDS_MUTEX_UNLOCK(pstParticipant->monMutex);
#endif

    /* �������ݽṹ�ڴ����� */
    HistoryData* pstHistoryData = (HistoryData*)DDS_STATIC_MALLOC_T(sizeof(HistoryData));
    if (NULL == pstHistoryData)
    {
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData, Insufficient memory space.\n");
        return FALSE;
    }

    /* �����л�װ����Ϣ */
    DESERIALIZE_SERIALIZEDDATA(pstDataFrag->serializedPayload, (*pstMemBlock));

    if (pstMemBlock->totalSize < pstMemBlock->readIndex)
    {
        return FALSE;
    }

    /* �û�����ʱ��� */
    pstHistoryData->timeInfo = pstTimestamp->value;

    /* �����û����ݳ��� */
    pstHistoryData->uiDataLen = pstDataFrag->dataSize;//pstMemBlock->totalSize - pstMemBlock->readIndex;

    /* �û������ڴ����� */
    pstHistoryData->data = (CHAR*)DDS_STATIC_MALLOC_T(pstHistoryData->uiDataLen);

    pstHistoryData->uiFragLen = pstDataFrag->fragmentSize;
    if (NULL == pstHistoryData->data)
    {
        /* �û����������ڴ�ʧ�����ͷ�֮ǰ���� */
        DDS_STATIC_FREE(pstHistoryData);
        PrintLog(COMMLOG_ERROR, "HandleUSERWriterDataMsg:Failed to create HistoryData->data, Insufficient memory space.\n");
        return FALSE;
    }

    /* ��¼��ǰ����seqNum */
    pstHistoryData->seqNum = pstDataFrag->writerSN;

    /* �����л��û����� */
    GET_CHAR_ARRAY((*pstMemBlock), pstHistoryData->data, (INT)pstHistoryData->uiFragLen);

    pstHistoryData->uiFragTotal = pstDataFrag->fragmentsInSubmessage;
    pstHistoryData->uiFragNum = pstDataFrag->fragmentStartingNum;
    pstHistoryData->uiFragLen = pstDataFrag->fragmentSize;


    /* ����Ϊ��״̬�Ķ���������Զ��д����״̬��ֱ�������ύ */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDataReader->stDataReaderQos.reliability.kind)
    {
        DeilverStatuslessUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);
        return TRUE;
    }

    /* �����Ķ���Ϊ��״̬��Զ��д������״̬����ƥ�䣬ֱ������ */
    if (BEST_EFFORT_RELIABILITY_QOS == pstDiscWriter->stDataWriterQos.reliability.kind)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg:Reader and writer states do not match.\n");
        return TRUE;
    }

    /* ������Զ�˶�Ϊ��״̬�ύ���� */
    DeilverStatusFulUserMsg(pstDataReader, pstDiscWriter, pstHistoryData);

    return FALSE;
}

BOOL HandleGapMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Gap* pstGapMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant)
{
    GUID_t stDiscWtGuid;
    DDS_DataReader* pstDataReader = NULL;

    /* ��ȡԶ��д����guid */
    stDiscWtGuid.prefix = pstRTPSMsgHeader->guidPrefix;
    stDiscWtGuid.entityId = pstGapMsg->writerId;

    /* ����qos��ʱ������ֱ�ӹ���*/
    if (pstSubMsgHeader->flags[1])
    {
        InlineQos stInlineQos = { GUID_UNKNOWN, 0 };
        NetworkByteOrderConvertInlineQos(pstMemBlock, &stInlineQos);
    }

    /* ���Ҹ��û����ݶ�Ӧ��Զ��д���� */
    DDS_DiscoveredWriter* pstDiscWriter = GetDiscoveredWriterFromParticipantByGuid(pstParticipant, &stDiscWtGuid);
    if (NULL == pstDiscWriter)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered remote dataWriter.\n");
        return FALSE;
    }

    pstDataReader = pstDiscWriter->pstTopic->pstLocalReader;
    if (NULL == pstDataReader)
    {
        PrintLog(COMMLOG_WARN, "HandleUSERWriterDataMsg: Cannot discovered dataReader.\n");
        return FALSE;
    }


    CorrectDiscoveredWriterGapMsg(pstDataReader, pstDiscWriter, pstGapMsg);

    if (pstDataReader->pstTopic->stUserData.pstHead)
    {
        while (NULL != pstDataReader)
        {

            /* �ص������������û����� */
            if (NULL != pstDataReader->recvCallBack)
            {
#ifdef QYDDS_TIME_BASED_FILTER_QOS
                if (pstDataReader->stDataReaderQos.time_based_filter.minimum_separation.sec != 0 || pstDataReader->stDataReaderQos.time_based_filter.minimum_separation.nanosec != 0)
                {
                    Time_t nowTime = GetNowTime();
                    Duration_t timeUsed = Sub_Duration(nowTime, pstDataReader->lastRecvTime);
                    if (Compare_Duration(timeUsed, pstDataReader->stDataReaderQos.time_based_filter.minimum_separation))
                    {
                        (pstDataReader->recvCallBack)(pstDataReader);
                        pstDataReader->lastRecvTime = nowTime;
                    }
                }
                else
                {
                    (pstDataReader->recvCallBack)(pstDataReader);
                }
            
#else
                (pstDataReader->recvCallBack)(pstDataReader);
#endif
            }
                pstDataReader = pstDataReader->pNext;

        }

            UninitHistoryCache(&pstDiscWriter->pstTopic->stUserData);
    }
    return TRUE;
}
