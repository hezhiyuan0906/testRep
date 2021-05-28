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
*  @file     AckNack.h
*  @brief
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/20
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/20 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_ACKNACK_H
#define HIPERD_ACKNACK_H

/****************************************************************************************************************************************/
//结构体定义
/****************************************************************************************************************************************/

typedef struct _AckNack
{
    EntityId_t          readerId; 
    EntityId_t          writerId;
    SequenceNumberSet   readerSNState;
    UINT32              count;
    UINT32              uiStartIndex; /* 起始位置 */
}AckNack;

/* AckNack相关的编解码宏 */
#define SERIALIZE_ACKNACK(cakNack, memBlock)                        \
    do                                                              \
    {                                                               \
        cakNack.uiStartIndex = memBlock.writeIndex;                 \
        SERIALIZE_ENTITYID(cakNack.readerId, memBlock)              \
        SERIALIZE_ENTITYID(cakNack.writerId, memBlock)              \
        SERIALIZE_SEQUENCENUMBERSET(cakNack.readerSNState, memBlock)\
        PUT_UNSIGNED_INT(memBlock, cakNack.count)                   \
    } while (0);

#define DESERIALIZE_ACKNACK(cakNack, memBlock)                          \
    do                                                                  \
    {                                                                   \
        DESERIALIZE_ENTITYID(cakNack.readerId, memBlock)                \
        DESERIALIZE_ENTITYID(cakNack.writerId, memBlock)                \
        DESERIALIZE_SEQUENCENUMBERSET(cakNack.readerSNState, memBlock)  \
        GET_UNSIGNED_INT(memBlock, cakNack.count)                       \
    } while (0);

/****************************************************************************************************************************************/
//函数声明
/****************************************************************************************************************************************/


#endif
