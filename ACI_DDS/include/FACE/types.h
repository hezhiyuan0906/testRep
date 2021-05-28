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

/*
   This file contains type definitions for types given in 
     Tables 141, 143, and 144 of the FACE 2.1 Standard.
   Relevant information from these tables is replicated below:
 
   C Type:                    Size (bytes):    Range of Values:
   --------                   ------------     ---------------
   FACE_boolean               1                0 to (2^1 - 1)
   FACE_char                  1                0 to (2^8 - 1)
   FACE_wchar                 unspecified      unknown
   FACE_octet                 1                0 to (2^8 - 1)
   FACE_double                8                IEEE double precision floating point
   FACE_long_double           10               IEEE extended double precision floating point
   FACE_float                 4                IEEE single precision floating point
   FACE_short                 2                -2^15 to (2^15 - 1)
   FACE_long                  4                -2^31 to (2^31 - 1)
   FACE_long_long             8                -2^63 to (2^63 - 1)
   FACE_unsigned_short        2                0 to (2^16 - 1)
   FACE_unsigned_long         4                0 to (2^32 - 1)
   FACE_unsigned_long_long    8                0 to (2^64 - 1)
   
   Default definitions for these types are provided below,
     but they should be edited for your particular system.
*/

#ifndef _FACE_TYPES_H_
#define _FACE_TYPES_H_

#include <stddef.h> // whcar_t

//typedef _Bool               FACE_boolean;
typedef char                FACE_boolean;
typedef unsigned char       FACE_char;
typedef wchar_t             FACE_wchar;
typedef unsigned char       FACE_octet;
typedef double              FACE_double;
typedef long double         FACE_long_double;
typedef float               FACE_float;
typedef signed short        FACE_short;
typedef signed long         FACE_long;
typedef signed long long    FACE_long_long;
typedef unsigned short      FACE_unsigned_short;
typedef unsigned long       FACE_unsigned_long;
typedef unsigned long long  FACE_unsigned_long_long;

#endif /* _FACE_TYPES_H_ */
