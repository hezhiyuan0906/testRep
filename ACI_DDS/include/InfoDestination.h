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
*  @file     InfoDestination.h
*  @brief    InfoDestination
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
InfoDestination.h
********************************************************************/

#ifndef HIPERD_INFODESTINATION_H
#define HIPERD_INFODESTINATION_H

typedef struct InfoDestination
{
    GuidPrefix_t        guidPrefix      ;
    CHAR*               pcGuidPre;
}InfoDestination;

/* 相关编解码工具宏 */

#define SERIALIZE_INFODESTINATION(curInfo, memBlock)                \
    do                                                              \
    {                                                               \
        curInfo.pcGuidPre = memBlock.wrPtr;                         \
        SERIALIZE_GUID_PREFIX(curInfo.guidPrefix, memBlock)         \
    } while (0);

#define DESERIALIZE_INFODESTINATION(curInfo, memBlock)              \
    do                                                              \
    {                                                               \
        DESERIALIZE_GUID_PREFIX(curInfo.guidPrefix, memBlock)       \
    } while (0);

#endif