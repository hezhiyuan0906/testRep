// DISTRIBUTION STATEMENT A. Approved for public release; distribution is unlimited. 
// 
// Product produced under DoD SENSIAC contract HC104705D4000 under
// the sponsorship of the Defense Technical Information Center,
// ATTN: DTIC-AI, 8723 John J. Kingman Rd., Ste 0944, Fort Belvoir, VA
// 22060-6218.  SENSIAC is a DoD Information Analysis Center Sponsored
// by the Defense Technical Information Center.
// 
// HANDLING AND DESTRUCTION NOTICE - Comply with distribution statement
// and destroy by any method that will prevent disclosure of
// contents or reconstruction of the document.

#ifndef _FACE_TYPEABSTRACTION_TS_H_
#define _FACE_TYPEABSTRACTION_TS_H_

#ifndef DDS_DLL
#define DDS_DLL __declspec(dllexport)
#endif

#include "TS_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef
void (*FACE_Read_Callback_send_event_Ptr) (
  /* in */ FACE_TRANSACTION_ID_TYPE,
  /* in */ void *,
  /* in */ FACE_MESSAGE_TYPE_GUID,
  /* in */ FACE_MESSAGE_SIZE_TYPE,
  /* in */ FACE_WAITSET_TYPE,
  /* out */ FACE_RETURN_CODE_TYPE*);


extern DDS_DLL void
FACE_TypeAbstractionTS_Initialize (
  /* in */ FACE_CONFIGURATION_RESOURCE configuration,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Create_Connection (
  /* in */ FACE_CONNECTION_NAME_TYPE connection_name,
  /* in */ FACE_MESSAGING_PATTERN_TYPE pattern,
  /* out */ FACE_CONNECTION_ID_TYPE * connection_id,
  /* out */ FACE_CONNECTION_DIRECTION_TYPE * connection_direction,
  /* out */ FACE_MESSAGE_SIZE_TYPE * max_message_size,
  /* in */ FACE_TIMEOUT_TYPE timeout,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Destroy_Connection (
  /* in */ FACE_CONNECTION_ID_TYPE connection_id,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Receive_Message (
  /* in */ FACE_CONNECTION_ID_TYPE connection_id,
  /* in */ FACE_TIMEOUT_TYPE timeout,
  /* inout */ FACE_TRANSACTION_ID_TYPE * transaction_id,
  /* in */ void * message,
  /* inout */ FACE_MESSAGE_TYPE_GUID * message_type_id,
  /* inout */ FACE_MESSAGE_SIZE_TYPE * message_size,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Send_Message (
  /* in */ FACE_CONNECTION_ID_TYPE connection_id,
  /* in */ FACE_TIMEOUT_TYPE timeout,
  /* inout */ FACE_TRANSACTION_ID_TYPE * transaction_id,
  /* in */ void * message,
  /* in */ FACE_MESSAGE_TYPE_GUID message_type_id,
  /* in */ FACE_MESSAGE_SIZE_TYPE message_size,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Register_Callback (
  /* in */ FACE_CONNECTION_ID_TYPE connection_id,
  /* in */ FACE_WAITSET_TYPE waitset,
  /* inout */ FACE_Read_Callback_send_event_Ptr data_callback,
  /* in */ FACE_MESSAGE_SIZE_TYPE max_message_size,
  /* out */ FACE_RETURN_CODE_TYPE* return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Unregister_Callback (
  /* in */ FACE_CONNECTION_ID_TYPE connection_id,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

extern DDS_DLL void
FACE_TypeAbstractionTS_Get_Connection_Parameters (
  /* inout */ FACE_CONNECTION_NAME_TYPE * connection_name,
  /* inout */ FACE_CONNECTION_ID_TYPE * connection_id,
  /* out */ FACE_TRANSPORT_CONNECTION_STATUS_TYPE * connection_status,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef */