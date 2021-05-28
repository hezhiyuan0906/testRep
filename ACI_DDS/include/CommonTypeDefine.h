/*********************************************************************************
*  OpenST Basic tool library
*  Copyright (C) 2019 QiYuan
*
*
*  This program is security software
*  without permission you cannot use or distribute the software
*  otherwise, legal liability will be investigated
*
*  If you want to use the software, please contact the authorities to obtain a license
*  < http://www.iamqy.com.cn >.
*
*  @file     CommonTypeDefine.h
*  @brief    CommonTypeDefine
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/08
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/08 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef COMMONTYPEDEFINE_H
#define COMMONTYPEDEFINE_H

/**
* 类型映射，屏蔽平台类型差异
*/
#if defined(_WIN32) || defined(_LINUX) || defined(_VXWORKS)
//typedef boolean             EX_BOOLEAN  ; 
typedef char                EX_INT8     ;
typedef unsigned char       EX_UINT8    ;
typedef short               EX_INT16    ;
typedef unsigned short      EX_UINT16   ;
typedef int                 EX_INT32    ;
typedef unsigned int        EX_UINT32   ;
typedef long long           EX_INT64    ;
typedef unsigned long long  EX_UINT64   ;
typedef float               EX_FLOAT32  ;
typedef double              EX_DOUBLE64 ;

#endif 

#define SHORT_SIZE 2
#define LONG_SIZE 4
#define LONGLONG_SIZE 8

#endif