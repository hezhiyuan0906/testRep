/*********************************************************************************
*  OpenST Basic tool library
*  Copyright (C) 2019 QiYuan
*
*
*  This program is security software
*  without permission you cannot MEMUSE or distribute the software
*  otherwise, legal liability will be investigated
*
*  If you want to MEMUSE the software, please contact the authorities to obtain a license
*  < http://www.iamqy.com.cn >.
*
*  @file     ParticipantData.h
*  @brief    
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/07/29
*  @license  GNU General Public License (GPL)
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/07/29 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

/********************************************************************
ParticipantData.h
RTPS的Discovery模块中用于SPDP的ParticipantData结构
********************************************************************/

#ifndef HIPERD_PARTICIPANT_BUILTIN_TOPIC_DATA_H
#define HIPERD_PARTICIPANT_BUILTIN_TOPIC_DATA_H

typedef struct _ParticipantData
{
    BuiltinTopicKey_t   key     ;
    UserDataQosPolicy   userData;
    UINT32              domainID; /* domainID           */
    BoundedString128    name    ; /* Participant的名称  */
	PropertyList        propertyList; /* 属性列表，这里使用了特定的PropertyList */
} ParticipantData;


/*
以下为该结构编解码工具宏
*/
#define SERIALIZE_PARTICIPANT_DATA(data , memBlock)    \
    do                                                               \
    {                                                                \
        SERIALIZE_BUILTIN_TOPIC_KEY(data.key, memBlock)              \
        SERIALIZE_USER_DATA_QOS_POLICY(data.userData, memBlock)      \
    } while (0);

#define DESERIALIZE_PARTICIPANT_DATA(data , memBlock)  \
    do                                                               \
    {                                                                \
        DESERIALIZE_BUILTIN_TOPIC_KEY(data.key, memBlock)            \
        DESERIALIZE_USER_DATA_QOS_POLICY(data.userData, memBlock)    \
    } while (0);

#endif