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

#ifndef _FACE_HMFM_H_
#define _FACE_HMFM_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef FACE_long FACE_HMFM_FAULT_MESSAGE_SIZE_TYPE;

typedef void *FACE_HMFM_FAULT_MESSAGE_ADDRESS_TYPE;

typedef FACE_long FACE_HMFM_THREAD_ID_TYPE;

#define FACE_HMFM_FAULT_MESSAGE_MAXIMUM_SIZE ((FACE_HMFM_FAULT_MESSAGE_SIZE_TYPE) 128)

typedef FACE_unsigned_long FACE_HMFM_STACK_SIZE_TYPE;

typedef FACE_char FACE_HMFM_FAULT_MESSAGE_TYPE[128];

typedef enum
{
  FACE_HMFM_DEADLINE_MISSED,
  FACE_HMFM_APPLICATION_ERROR,
  FACE_HMFM_NUMERIC_ERROR,
  FACE_HMFM_ILLEGAL_REQUEST,
  FACE_HMFM_STACK_OVERFLOW,
  FACE_HMFM_MEMORY_VIOLATION,
  FACE_HMFM_HARDWARE_FAULT,
  FACE_HMFM_POWER_FAIL
}   FACE_HMFM_FAULT_CODE_TYPE;

typedef struct {
  FACE_HMFM_FAULT_CODE_TYPE CODE;
  FACE_HMFM_FAULT_MESSAGE_TYPE MESSAGE;
  FACE_HMFM_FAULT_MESSAGE_SIZE_TYPE LENGTH;
  FACE_HMFM_THREAD_ID_TYPE FAILED_THREAD_ID;
  FACE_SYSTEM_ADDRESS_TYPE FAILED_ADDRESS;
}   FACE_HMFM_FAULT_STATUS_TYPE;

typedef void (*FACE_HMFM_FAULT_HANDLER_ENTRY_TYPE) (void);

void
FACE_HMFM_Initialize (void);

void
FACE_HMFM_Create_Fault_Handler (
  /* in */ FACE_HMFM_FAULT_HANDLER_ENTRY_TYPE entry_point,
  /* in */ FACE_HMFM_STACK_SIZE_TYPE stack_size,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

void
FACE_HMFM_Report_Application_Message (
  /* in */ FACE_HMFM_FAULT_MESSAGE_ADDRESS_TYPE fault,
  /* in */ FACE_HMFM_FAULT_MESSAGE_SIZE_TYPE length,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

void
FACE_HMFM_Get_Fault_Status (
  /* out */ FACE_HMFM_FAULT_STATUS_TYPE * fault,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

void
FACE_HMFM_Raise_Application_Fault (
  /* in */ FACE_HMFM_FAULT_CODE_TYPE code,
  /* in */ FACE_HMFM_FAULT_MESSAGE_ADDRESS_TYPE message,
  /* in */ FACE_HMFM_FAULT_MESSAGE_SIZE_TYPE length,
  /* out */ FACE_RETURN_CODE_TYPE * return_code);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef */