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

#ifndef _FACE_COMMON_H_
#define _FACE_COMMON_H_

//#include <FACE/types.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef FACE_long_long FACE_SYSTEM_TIME_TYPE;

#define FACE_INF_TIME_VALUE ((FACE_SYSTEM_TIME_TYPE) -1)

typedef FACE_char FACE_CONFIGURATION_RESOURCE[256];

typedef enum
{
  FACE_NO_ERROR,
  FACE_NO_ACTION,
  FACE_NOT_AVAILABLE,
  FACE_ADDR_IN_USE,
  FACE_INVALID_PARAM,
  FACE_INVALID_CONFIG,
  FACE_PERMISSION_DENIED,
  FACE_INVALID_MODE,
  FACE_TIMED_OUT,
  FACE_MESSAGE_STALE,
  FACE_CONNECTION_IN_PROGRESS,
  FACE_CONNECTION_CLOSED,
  FACE_DATA_BUFFER_TOO_SMALL
}   FACE_RETURN_CODE_TYPE;

typedef void *FACE_SYSTEM_ADDRESS_TYPE;

typedef FACE_SYSTEM_TIME_TYPE FACE_TIMEOUT_TYPE;

typedef FACE_long FACE_MESSAGE_RANGE_TYPE;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef */