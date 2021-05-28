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
*  @file     InfoReply.h
*  @brief    InfoReply
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
InfoReply.h
********************************************************************/

#ifndef HIPERD_INFOREPLY_H
#define HIPERD_INFOREPLY_H

typedef struct InfoReply
{
    SubMessageHeader    subMsgHeader        ;
    LocatorList_t       unicastLocatorList  ;
    LocatorList_t       multicastLocatorList;
    BOOL                multicastFlag       ;
    BOOL                littleEndian        ;
}InfoReply;

/* 相关编解码工具宏 */
#define SERIALIZE_INFOREPLY(curInfo, memBlock)                              \
    do                                                                      \
    {                                                                       \
        SERIALIZE_SUBMESSAGEHEADER(curInfo.subMsgHeader, memBlock)          \
        SERIALIZE_LOCATOR_LIST(curInfo.unicastLocatorList, memBlock)        \
        if (curInfo.multicastFlag)                                          \
        {                                                                   \
            SERIALIZE_LOCATOR_LIST(curInfo.multicastLocatorList, memBlock)  \
        }                                                                   \
    } while (0);

#define DESERIALIZE_INFOREPLY(curInfo, memBlock)                            \
    do                                                                      \
    {                                                                       \
        DESERIALIZE_SUBMESSAGEHEADER(curInfo.subMsgHeader, memBlock)        \
        curInfo.littleEndian = subMsgHeader.m_subMsgFlags[0];             \
        curInfo.multicastFlag = subMsgHeader.m_subMsgFlags[1];            \
        DESERIALIZE_LOCATOR_LIST(curInfo.unicastLocatorList, memBlock)      \
        if (curInfo.multicastFlag)                                          \
        {                                                                   \
            DESERIALIZE_LOCATOR_LIST(curInfo.multicastLocatorList, memBlock)\
        }                                                                   \
    } while (0);
    

#endif