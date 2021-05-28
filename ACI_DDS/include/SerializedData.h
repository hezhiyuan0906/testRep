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
*  @file     SerializedData.h
*  @brief    SerializedData
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
SerializedData.h 
*/

#ifndef HIPERD_SERIALIZED_DATA_H
#define HIPERD_SERIALIZED_DATA_H

typedef enum _CDRTYPE
{
    CDR_BE      = 0x0000,
    CDR_LE      = 0x0100,
    PL_CDR_BE   = 0x0002,
    PL_CDR_LE   = 0x0300,
    CDR_UNKNOWN = 0x0004
}CDRTYPE;

#define SERIALIZEDDATA_SIZE 4

typedef struct _SerializedData
{
    CDRTYPE     m_cdrType   ;
    EX_UINT16   m_option    ;
}SerializedData;


/*
以下为SerializedData的编解码工具宏
*/

#define SERIALIZE_SERIALIZEDDATA(serData, memBlock)     \
    do                                                  \
    {                                                   \
        PUT_SHORT(memBlock, serData.m_cdrType)          \
        PUT_UNSIGNED_SHORT(memBlock, serData.m_option)  \
    } while (0);


#define DESERIALIZE_SERIALIZEDDATA(serData, memBlock)   \
    do                                                  \
    {                                                   \
        unsigned short tmpcdrType = 0;                  \
        GET_SHORT(memBlock, tmpcdrType)                 \
        serData.m_cdrType = (unsigned int)tmpcdrType;   \
        GET_UNSIGNED_SHORT(memBlock, serData.m_option)  \
    } while (0);


#endif
