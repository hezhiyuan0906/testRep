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
*  @file     BoundedString.h
*  @brief    BoundedString
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
BoundedString.h
固定长度的字符串结构
********************************************************************/

#ifndef HIPERD_BOUNDED_STRING_H
#define HIPERD_BOUNDED_STRING_H

typedef struct BoundedString32
{
    unsigned int   length      ;
    char            value[32]   ;
}BoundedString32;

typedef struct BoundedString64
{
    unsigned int   length;
    char            value[64]   ;
}BoundedString64;

typedef struct BoundedString128
{
    unsigned int   length;
    char            value[128]  ;
}BoundedString128;

typedef struct BoundedString256
{
    unsigned int   length;
    char            value[256]  ;
}BoundedString256;

/*
 以下为字符串的编解码工具宏
*/
/* 字符串统一编码工具宏 */
#define SERIALIZE_BOUNDED_STRING(boundedStr , memBlock)                 \
    do                                                                  \
    {                                                                   \
        PUT_UNSIGNED_INT(memBlock, boundedStr.length)                   \
        PUT_CHAR_ARRAY(memBlock, boundedStr.value, boundedStr.length)   \
    } while (0);


#define DESERIALIZE_BOUNDED_STRING(boundedStr , memBlock)               \
    do                                                                  \
    {                                                                   \
        GET_UNSIGNED_INT(memBlock, boundedStr.length)                   \
        GET_CHAR_ARRAY(memBlock, boundedStr.value, boundedStr.length)   \
    } while (0);

/*
 以下为面向字符串的工具宏，需要进一步完善
*/
#define STRCMP(leftStr , rightStr) strcmp(## leftStr ##.value , ## rightStr ##.value)

#define STRNCPY(tgtStr , srcStr , len) strncpy(## leftStr ##.value , ## rightStr ##.value , len)

#define STRLEN(Str) strlen(## Str ##.value)


#endif