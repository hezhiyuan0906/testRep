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
*  @file     SubMessageHeader.h
*  @brief    SubMessageHeader
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

/*
SubMessageHeader.h
RTPS子消息头，标明子消息类型标志及长度
*/

#ifndef HIPERD_SUBMESSAGE_HEADER_H
#define HIPERD_SUBMESSAGE_HEADER_H

/**
* SubmessageFlag类型用来制定Submessage标志
*/
typedef BOOL SubmessageFlag;

/**
* SubmessageKind类型用来枚举标识Submessage的类型,一个字节，0x00到0x7f由协议指定
*/
typedef enum _SubmessageKind
{
    PAD                 = 0x01, /* Pad              */
    ACKNACK             = 0x06, /* AckNack          */
    GAP                 = 0x08, /* Gap              */
    INFO_TS             = 0x09, /* InfoTimestamp    */
    HEARTBEAT           = 0x07, /* Heartbeat        */
    INFO_SRC            = 0x0c, /* InfoSource       */
    INFO_REPLY_IP4      = 0x0d, /* InfoReplyIp4     */
    INFO_DST            = 0x0e, /* InfoDestination  */
    INFO_REPLY          = 0x0f, /* InfoReply        */
    NACK_FRAG           = 0x12, /* NackFrag         */
    HEARTBEAT_FRAG      = 0x13, /* HeartbeatFrag    */
    DATA                = 0x15, /* Data             */
    DATA_FRAG           = 0x16, /* DataFrag         */
    MONITOR             = 0x20, /* Mointor          */
    TOPIC               = 0x21, /* Mointor          */
    UNKNOWN_SUBMSGKIND  = 0x00
}SubmessageKind;

/* 自消息长度 Kind(1) + Flag(1) + length(2) */
#define SUBMSGHEADERLEN  4
/* 标志位个数 */
#define SUBMSG_FLAGS_NUM  8

typedef struct _SubMessageHeader
{
    SubmessageKind  submessageId                    ;  /* 消息类型                        */
    SubmessageFlag  flags[SUBMSG_FLAGS_NUM]         ;  /* 标志位 序列化时会压缩到一个字节 */
    USHORT          submessageLength                ;  /* 消息长度                        */
    CHAR*           pcLenFlag;
}SubMessageHeader;

#define GET_SIZE_SUBMESSAGEHEADER(curSubMsg, curSize) curSize = 4;

/*
以下为该结构编解码工具宏
*/

#define SERIALIZE_SUBMESSAGEHEADER(subMsgHeader, memBlock)                                      \
    do                                                                                          \
    {                                                                                           \
        UINT32 uiIndex;                                                                         \
        CHAR cFlag =0x00;                                                                       \
        PUT_BYTE(memBlock, subMsgHeader.submessageId)                                           \
        for (uiIndex = 0; uiIndex < 4; uiIndex++)                                               \
        {                                                                                       \
            cFlag |= ((subMsgHeader.flags[uiIndex] & 0x01) << uiIndex);                         \
        }                                                                                       \
        PUT_BYTE(memBlock, cFlag)                                                               \
        subMsgHeader.pcLenFlag = memBlock.wrPtr;                                                \
        PUT_UNSIGNED_SHORT(memBlock, subMsgHeader.submessageLength)                             \
    } while (0);

#define DESERIALIZE_SUBMESSAGEHEADER(subMsgHeader , memBlock)                                   \
    do                                                                                          \
    {                                                                                           \
        UINT32 uiIndex;                                                                         \
        CHAR cFlag = 0x00;                                                                      \
        GET_BYTE(memBlock, subMsgHeader.submessageId)                                           \
        GET_BYTE(memBlock, cFlag)                                                                \
        for (uiIndex = 0; uiIndex < 4; uiIndex++)                                               \
        {                                                                                       \
            subMsgHeader.flags[uiIndex] = ((cFlag >> uiIndex) & 0x01);                          \
            subMsgHeader.flags[uiIndex + 4] = 0;                                                \
        }                                                                                       \
        GET_UNSIGNED_SHORT(memBlock, subMsgHeader.submessageLength)                             \
    } while (0);

#endif
