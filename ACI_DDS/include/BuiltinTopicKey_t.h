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
*  @file     BuiltinTopicKey_t.h
*  @brief    BuiltinTopicKey_t
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
BuiltinTopicKey_t.h
RTPS规范中BuiltinTopicKey_t对应的数据类型及相关处理机制
********************************************************************/

#ifndef HIPERD_BUILTINTOPICKEY_T_H
#define HIPERD_BUILTINTOPICKEY_T_H

typedef struct BuiltinTopicKey_t
{
    EX_INT32 m_value[4];
}BuiltinTopicKey_t;

/*
 以下为该结构编解码工具宏
*/
#define SERIALIZE_BUILTIN_TOPIC_KEY(builtinTopicKey , memBlock) \
        PUT_INT_ARRAY(memBlock,builtinTopicKey.m_value,4)

#define DESERIALIZE_BUILTIN_TOPIC_KEY(builtinTopicKey , memBlock) \
        GET_INT_ARRAY(memBlock,builtinTopicKey.m_value,4)

#define BUILTIN_TOPIC_KEY_IS_EQUAL(lefKey , rightKey)           \
    (## lefKey ##.m_value[0] == ## rightKey ##.m_value[0]) &&   \
    (## lefKey ##.m_value[1] == ## rightKey ##.m_value[1]) &&   \
    (## lefKey ##.m_value[2] == ## rightKey ##.m_value[2]) &&   \
    (## lefKey ##.m_value[3] == ## rightKey ##.m_value[3]) 

#endif