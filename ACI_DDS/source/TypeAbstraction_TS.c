#include "../Include/GlobalDefine.h"

typedef struct _ConnectionId
{
    DDS_Topic* pstTopic;
    DDS_DataWriter* pstDataWriter;
    DDS_DataReader* pstDataReader;

}ConnectionId;

VOID STACALL ReceiveUserData(DDS_DataReader* pstDataReader)
{
    FACE_RETURN_CODE_TYPE  return_code = FACE_NO_ERROR;
    UINT32 dataNum = 0;
    DataSeq stDataSeq;
    DataLen dataLen;
    CHAR* msg;
    /* 收数据 */
    DDS_ReturnCode_t retCode = DDS_StringDataReader_take(pstDataReader, &stDataSeq);
    if (DDS_RETCODE_NO_DATA != retCode)
    {
        for (dataNum = 0; dataNum < stDataSeq.length; dataNum++)
        {
            msg = DDS_StringSeq_get(&stDataSeq, dataNum, &dataLen);

            ((FACE_Read_Callback_send_event_Ptr)(pstDataReader->params[0]))(0, (void*)msg,  0, dataLen, 0,  &return_code);
        }
        /* 释放缓存 */
        DDS_StringDataReader_return_loan(pstDataReader, &stDataSeq);
    }
}

DDS_DomainParticipant* g_pstParticipant = NULL;

DDS_DLL void
FACE_TypeAbstractionTS_Initialize(
/* in */ FACE_CONFIGURATION_RESOURCE configuration,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    DDS_DomainParticipantQos stDomPartQos;
    DDS_ParticipantFactory_init();

    /* 获取参与者qos */
    DDS_ReturnCode_t retureCode = DDS_DomainParticipantFactory_get_defaul_participant_qos(&stDomPartQos);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Data writer setup error !");
        return;
    }

    /* 创建参与者 */
    g_pstParticipant = DDS_DomainParticipantFactory_create_participant(78, &stDomPartQos);
    if (NULL == g_pstParticipant)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Failed to create participant!");
        return;
    }

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Create_Connection(
/* in */ FACE_CONNECTION_NAME_TYPE connection_name,
/* in */ FACE_MESSAGING_PATTERN_TYPE pattern,
/* out */ FACE_CONNECTION_ID_TYPE * connection_id,
/* out */ FACE_CONNECTION_DIRECTION_TYPE * connection_direction,
/* out */ FACE_MESSAGE_SIZE_TYPE * max_message_size,
/* in */ FACE_TIMEOUT_TYPE timeout,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    DDS_ReturnCode_t retureCode;
    DDS_TopicQos stTopicQos;
    DDS_DataWriterQos stWriterQos;

    ConnectionId* connectionId = (ConnectionId*)DDS_STATIC_MALLOC(sizeof(ConnectionId));
	if (NULL == connectionId)
	{
		PrintLog(COMMLOG_ERROR, "MALLOC connectionId failed.\n");
		return;
	}

    /* 创建主题 */
    connectionId->pstTopic = DDS_DomainParticipant_create_topic(g_pstParticipant, connection_name, connection_name, &stTopicQos);
    if (NULL == connectionId->pstTopic)
    {
        *return_code = FACE_NO_ACTION;
		DDS_STATIC_FREE(connectionId);
        printf("ERROR! Failed to create topic!");
        return;
    }

    /* 获取写入器qos */
    retureCode = DDS_Topic_get_default_datawriter_qos(connectionId->pstTopic, &stWriterQos);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
		DDS_STATIC_FREE(connectionId);
        printf("ERROR! Data writer setup error !");
        return;
    }

    /* 设置写入器可靠性 */
    stWriterQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;

    /* 创建写入器 */
    connectionId->pstDataWriter = DDS_Topic_create_datawriter(connectionId->pstTopic, &stWriterQos, NULL);
    if (NULL == connectionId->pstDataWriter)
    {
        *return_code = FACE_NO_ACTION;
		DDS_STATIC_FREE(connectionId);
        printf("ERROR! Failed to create dataWriter!");
        return;
    }

    *connection_id = (FACE_CONNECTION_ID_TYPE)connectionId;

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Destroy_Connection(
/* in */ FACE_CONNECTION_ID_TYPE connection_id,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    DDS_ReturnCode_t retureCode;
    retureCode = DDS_DomainParticipant_delete_topic(g_pstParticipant, ((ConnectionId*)connection_id)->pstTopic);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Failed to delete topic!\n");
        return;
    }

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Receive_Message(
/* in */ FACE_CONNECTION_ID_TYPE connection_id,
/* in */ FACE_TIMEOUT_TYPE timeout,
/* inout */ FACE_TRANSACTION_ID_TYPE * transaction_id,
/* in */ void * message,
/* inout */ FACE_MESSAGE_TYPE_GUID * message_type_id,
/* inout */ FACE_MESSAGE_SIZE_TYPE * message_size,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    UINT32 dataNum = 0;
    DataSeq stDataSeq;
    CHAR* msg = NULL;

    /* 收数据 */
    DDS_ReturnCode_t retCode = DDS_StringDataReader_take(((ConnectionId*)connection_id)->pstDataReader, &stDataSeq);
    if (DDS_RETCODE_NO_DATA != retCode)
    {
        for (dataNum = 0; dataNum < stDataSeq.length; dataNum++)
        {
            msg = DDS_StringSeq_get(&stDataSeq, dataNum, message_size);
            memcpy(message, msg, *message_size);
        }
        /* 释放缓存 */
        DDS_StringDataReader_return_loan(((ConnectionId*)connection_id)->pstDataReader, &stDataSeq);
    }

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Send_Message(
/* in */ FACE_CONNECTION_ID_TYPE connection_id,
/* in */ FACE_TIMEOUT_TYPE timeout,
/* inout */ FACE_TRANSACTION_ID_TYPE * transaction_id,
/* in */ void * message,
/* in */ FACE_MESSAGE_TYPE_GUID message_type_id,
/* in */ FACE_MESSAGE_SIZE_TYPE message_size,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    /* 发数据 */
     DDS_ReturnCode_t retureCode = DDS_StringDataWriter_write(((ConnectionId*)connection_id)->pstDataWriter, (const char*)message, message_size);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Failed to send message, Insufficient memory space!\n");
    }

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Register_Callback(
/* in */ FACE_CONNECTION_ID_TYPE connection_id,
/* in */ FACE_WAITSET_TYPE waitset,
/* inout */ FACE_Read_Callback_send_event_Ptr data_callback,
/* in */ FACE_MESSAGE_SIZE_TYPE max_message_size,
/* out */ FACE_RETURN_CODE_TYPE* return_code)
{
    DDS_DataReaderQos stReaderQos;

    /* 获取阅读器qos */
    DDS_ReturnCode_t retureCode = DDS_Topic_get_default_datareader_qos(((ConnectionId*)connection_id)->pstTopic, &stReaderQos);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Data reader setup error !");
        return;
    }

    /* 设置阅读器可靠性 */
    stReaderQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;

    /* 创建阅读器 */
    ((ConnectionId*)connection_id)->pstDataReader = DDS_Topic_create_datareader(((ConnectionId*)connection_id)->pstTopic, &stReaderQos, ReceiveUserData);
    if (NULL == ((ConnectionId*)connection_id)->pstDataReader)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Failed to create dataReader!");
        return;
    }

	if (data_callback != NULL)
	{
		((ConnectionId*)connection_id)->pstDataReader->params[0] = (UINT64)data_callback;
	}

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Unregister_Callback(
/* in */ FACE_CONNECTION_ID_TYPE connection_id,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{
    /* 删除阅读器 */
    DDS_ReturnCode_t retureCode = DDS_Topic_delete_datareader(((ConnectionId*)connection_id)->pstTopic);
    if (DDS_RETCODE_ERROR == retureCode)
    {
        *return_code = FACE_NO_ACTION;
        printf("ERROR! Failed to delete dataReader!");
        return;
    }

    *return_code = FACE_NO_ERROR;
}

DDS_DLL void
FACE_TypeAbstractionTS_Get_Connection_Parameters(
/* inout */ FACE_CONNECTION_NAME_TYPE * connection_name,
/* inout */ FACE_CONNECTION_ID_TYPE * connection_id,
/* out */ FACE_TRANSPORT_CONNECTION_STATUS_TYPE * connection_status,
/* out */ FACE_RETURN_CODE_TYPE * return_code)
{


}

/* face接口示例 */
#if 0

void FaceCallBack(
    /* in */ FACE_TRANSACTION_ID_TYPE type,
    /* in */ void * msg,
    /* in */ FACE_MESSAGE_TYPE_GUID guid,
    /* in */ FACE_MESSAGE_SIZE_TYPE size,
    /* in */ FACE_WAITSET_TYPE waittype,
    /* out */ FACE_RETURN_CODE_TYPE* code)
{
    printf("RECEIVE:  LEN=%d    MSG=%s\n", size, (char*)msg);
}




void main_face()
{
    int i = 0;
    FACE_char msg[256];

    FACE_RETURN_CODE_TYPE  return_code;
    FACE_CONNECTION_ID_TYPE  connection_id;
    FACE_CONNECTION_DIRECTION_TYPE  connection_direction;
    FACE_MESSAGE_SIZE_TYPE  max_message_size;
    FACE_TIMEOUT_TYPE timeout = 0;
    FACE_TRANSACTION_ID_TYPE transaction_id;

    /* 初始化资源信息 */
    FACE_TypeAbstractionTS_Initialize(NULL, &return_code);
    if (FACE_NO_ERROR != return_code)
    {
        printf("Initialize fail!");
    }

    /* 创建主题连接 */
    FACE_TypeAbstractionTS_Create_Connection("HELLO", FACE_PUB_SUB, &connection_id, &connection_direction, &max_message_size, timeout, &return_code);
    if (FACE_NO_ERROR != return_code)
    {
        printf("Create_Connection fail!");
    }

    /* 注册回调 */
    FACE_TypeAbstractionTS_Register_Callback(connection_id, "NO WAIT", FaceCallBack, 256, &return_code);
    if (FACE_NO_ERROR != return_code)
    {
        printf("Register_Callback fail!");
    }

    for (i = 0; i < 100; i++)
    {
        sprintf_s(msg, 256, "Hello World From QY %d", i);
        FACE_TypeAbstractionTS_Send_Message(connection_id, timeout, &transaction_id, (void*)msg, 0, 256, &return_code);
        if (FACE_NO_ERROR != return_code)
        {
            printf("send msg fail!");
        }

        if (10 == i)
        {
            /* 取消注册回调*/
            FACE_TypeAbstractionTS_Unregister_Callback(connection_id, &return_code);
            if (FACE_NO_ERROR != return_code)
            {
                printf("Unregister_Callback fail!");
            }
        }

        if (20 == i)
        {
            /* 注册回调 */
            FACE_TypeAbstractionTS_Register_Callback(connection_id, "NO WAIT", FaceCallBack, 256, &return_code);
            if (FACE_NO_ERROR != return_code)
            {
                printf("Register_Callback fail!");
            }
        }

        Sleep(1000);
    }
}

#endif 
