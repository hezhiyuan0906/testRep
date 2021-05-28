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
*  @file     InfoTimestamp.h
*  @brief    InfoTimestamp
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
InfoTimestamp.h
********************************************************************/

#ifndef HIPERD_INFOTIMESTAMP_H
#define HIPERD_INFOTIMESTAMP_H

typedef struct _InfoTimestamp
{
    Time_t              value       ;
}InfoTimestamp;

/* 序列化反序列化工具宏 */
#define SERIALIZE_INFOTIMESTAMP(curInfo, memBlock)                  \
    do                                                              \
    {                                                               \
        SERIALIZE_DURATION(curInfo.value, memBlock)                 \
    } while (0);

#define DESERIALIZE_INFOTIMESTAMP(curInfo, memBlock)                \
    do                                                              \
    {                                                               \
        DESERIALIZE_DURATION(curInfo.value, memBlock)               \
    } while (0);


#endif