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
*  @file     SequenceNumberSet.h
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

#ifndef HIPERD_SEQUENCENUMBERSET_H
#define HIPERD_SEQUENCENUMBERSET_H

// 可以用来表示bitmap的最大4字节个数
#define SEQUENCE_BIT_NUMBER  8

/****************************************************************************************************************************************/
//结构体定义
/****************************************************************************************************************************************/

typedef struct _SequenceNumberSet
{
    SequenceNumber_t base                       ; 
    EX_UINT32        numBits                    ;   /* bitMap中有效的比特位数 */
    EX_UINT32        bitMap[SEQUENCE_BIT_NUMBER];

}SequenceNumberSet;

/* SequenceNumberSet编解码相关工具宏 */
#define SERIALIZE_SEQUENCENUMBERSET(seqNumSet, memBlock)                        \
    do                                                                          \
	    {                                                                           \
        SERIALIZE_SEQUENCENUMBER(seqNumSet.base, memBlock)                      \
        PUT_UNSIGNED_INT(memBlock, seqNumSet.numBits)                           \
        int number = ((seqNumSet.numBits + 31) >> 5);                           \
        PUT_UNSIGNED_INT_ARRAY(memBlock, seqNumSet.bitMap, number)              \
	    } while (0);

#define DESERIALIZE_SEQUENCENUMBERSET(seqNumSet, memBlock)                      \
    do                                                                          \
	    {                                                                           \
        DESERIALIZE_SEQUENCENUMBER(seqNumSet.base, memBlock)                    \
        GET_UNSIGNED_INT(memBlock, seqNumSet.numBits)                           \
        int number = 8;/*目前只使用8位，且与numBits无关，bitMap未使用，暂时不需要反序列化((seqNumSet.numBits + 31) >> 5);*/                           \
        GET_UNSIGNED_INT_ARRAY(memBlock, seqNumSet.bitMap, number)              \
     } while (0);

/****************************************************************************************************************************************/
//函数声明
/****************************************************************************************************************************************/

#endif
