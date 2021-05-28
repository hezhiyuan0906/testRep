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
*  @file     Gap.h
*  @brief    Gap
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
Gap.h
Gap子消息
********************************************************************/

#ifndef HIPERD_GAP_H
#define HIPERD_GAP_H

typedef struct Gap
{
    SubMessageHeader    subMsgHeader; /* 子消息头 */
    EntityId_t          readerId    ;
    EntityId_t          writerId    ;
    SequenceNumber_t    gapStart    ;
    SequenceNumberSet   gapList     ;
}Gap;

/* Gap 子消息相关编解码工具宏 */
#define SERIALIZE_GAP(_gap , memBlock)                          \
    do                                                          \
    {                                                           \
        SERIALIZE_SUBMESSAGEHEADER(_gap.subMsgHeader, memBlock) \
        SERIALIZE_ENTITYID(_gap.readerId, memBlock)             \
        SERIALIZE_ENTITYID(_gap.writerId, memBlock)             \
        SERIALIZE_SEQUENCENUMBER(_gap.gapStart, memBlock)       \
        SERIALIZE_SEQUENCENUMBERSET(_gap.gapList, memBlock)     \
    } while (0);

#define DESERIALIZE_GAP(_gap , memBlock)                            \
    do                                                              \
    {                                                               \
        DESERIALIZE_SUBMESSAGEHEADER(_gap.subMsgHeader, memBlock)   \
        DESERIALIZE_ENTITYID(_gap.readerId, memBlock)               \
        DESERIALIZE_ENTITYID(_gap.writerId, memBlock)               \
        DESERIALIZE_SEQUENCENUMBER(_gap.gapStart, memBlock)         \
        DESERIALIZE_SEQUENCENUMBERSET(_gap.gapList, memBlock)       \
    } while (0);

#endif