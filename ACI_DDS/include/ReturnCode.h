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
*  @file     TimeTool.h
*  @brief    TimeTool
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

/********************************************************************
TimeTool.h
高精度时间相关的数据结构和处理机制
********************************************************************/
#ifndef HIPERD_RETURNCODE_H
#define HIPERD_RETURNCODE_H

typedef enum _ReturnCode_t{
    DDS_RETCODE_OK = 0,  
    DDS_RETCODE_ERROR = 1,
    DDS_RETCODE_UNSUPPORTED = 2, 
    DDS_RETCODE_BAD_PARAMETER = 3, 
    DDS_RETCODE_PRECONDITION_NOT_MET = 4,  
    DDS_RETCODE_OUT_OF_RESOURCES = 5,  
    DDS_RETCODE_NOT_ENABLED = 6, 
    DDS_RETCODE_IMMUTABLE_POLICY = 7,  
    DDS_RETCODE_INCONSISTENT_POLICY = 8,  
    DDS_RETCODE_ALREADY_DELETED = 9,
    DDS_RETCODE_TIMEOUT = 10, 
    DDS_RETCODE_NO_DATA = 11, 
    DDS_RETCODE_ILLEGAL_OPERATION = 12, 
    DDS_RETCODE_NOT_ALLOWED_BY_SEC = 13,
    DDS_RETCODE_NO_AVAILABLE_READER = 14
} DDS_ReturnCode_t;

#endif