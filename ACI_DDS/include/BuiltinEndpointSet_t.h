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
*  @file     BuiltinEndpointSet_t.h
*  @brief    BuiltinEndpointSet_t
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
BuiltinEndpointSet_t.h
RTPS规范中BuiltinEndpointSet_t对应的数据类型及相关处理机制
********************************************************************/

#ifndef HIPERD_BUILTINENDPOINTSET_T_H
#define HIPERD_BUILTINENDPOINTSET_T_H


extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER       ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR        ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER       ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR        ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER      ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR       ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_ANNOUNCER ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_DETECTOR  ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_ANNOUNCER ;
extern const unsigned int DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_DETECTOR  ;
extern const unsigned int BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER  ;
extern const unsigned int BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER  ;


#define SIZE_OF_BUILTINENDPOINTSET 4

typedef EX_UINT32 BuiltinEndpointSet_t;

/* 
以下为BuiltinEndpointSet_t 的编解码宏
*/
#define SERIALIZE_BUILTINENDPOINTSET(endpoint, memBlock) PUT_UNSIGNED_INT(memBlock, endpoint)

#define DESERIALIZE_BUILTINENDPOINTSET(endpoint, memBlock) GET_UNSIGNED_INT(memBlock, endpoint)

// 是否包含指定Endpoint
#define BuiltinEndpointSetContains(builitinSet, spec) ((builitinSet & spec) != 0 )


#endif
