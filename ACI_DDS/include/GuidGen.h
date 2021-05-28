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
*  @file     GuidGen.h
*  @brief    GuidGen
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
GuidGen.h
用于生成GUID
*/

#ifndef HIPERD_GUIDGEN_H
#define HIPERD_GUIDGEN_H

/* 返回状态枚举 */
typedef enum RETSTATUS
{
    STA_OK                  ,
    STA_HAS_BEEN_REGISTERED ,
    STA_HAS_BEEN_USED       ,
    STA_ID_NOT_EXIST        ,
    STA_UNREG_PREFIX        ,
    STA_ENTITYID_EXHAUSTED  ,
    STA_BUILTIN_ENTITYID    ,
    STA_ERROR
}RETSTATUS;

typedef enum AUTO_GEN_PREFIX_KIND
{
    AUTO_GUID_FROM_IP,
    AUTO_GUID_FROM_MAC
}AUTO_GEN_PREFIX_KIND;

extern const EX_UINT32 GEN_INVALID;

extern const EX_UINT32 ENTITY_KEY_MASK  ;
extern const EX_UINT32 ENTITY_KIND_MASK ;
extern const EX_UINT32 AUTO_ENTITYID    ;

/* entity类型 */
extern const EX_UINT8 ENTITYKIND_TOPIC              ;
extern const EX_UINT8 ENTITYKIND_WRITER_NO_KEY      ;
extern const EX_UINT8 ENTITYKIND_READER_NO_KEY      ;
extern const EX_UINT8 ENTITYKIND_WRITER_WITH_KEY    ;
extern const EX_UINT8 ENTITYKIND_READER_WITH_KEY    ;

struct _DomainParticipant;
/* 生成前缀 */
RETSTATUS GenPrefix(struct _DomainParticipant *  pstDomainParticipant, AUTO_GEN_PREFIX_KIND gen_kind);

/* 生成GUID */
/* 其中entityID[31:24]bit位为Kind                                              */
/*     entityID[32: 0]bit位为Key                                               */
/* 如果参数中entityID的Key为AUTO_ENTITYID，则自动生成对应的kind下面的ID        */
/* 否则使用用户指定的ID                                                        */
RETSTATUS GenGuid(GUID_t * guid_t, const GuidPrefix_t*  guidPrefix, UINT8 kind);

/* 分配内部ID */
EX_UINT32 gen_internal_id();

#endif