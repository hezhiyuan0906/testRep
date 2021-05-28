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
*  @file     FragmentNumberSet.h
*  @brief    FragmentNumberSet
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
FragmentNumberSet.h
FragmentNumber集合
********************************************************************/

#ifndef HIPERD_FRAGMENTNUMBERSET_H
#define HIPERD_FRAGMENTNUMBERSET_H

/* 可以用来表示bitmap的最大4字节个数 */
#define FRAGMENT_BIT_NUMBER  8
/* 32位乘以FRAGMENT_BIT_NUMBER定义FragmentSet最大可以表述的Fragment集合 */
#define MAX_FRAGMENTNUM_ENCODE  FRAGMENT_BIT_NUMBER * 32;

typedef struct FragmentNumberSet
{

    FragmentNumber_t base                       ; /* bitMap中有效的比特位数 */
    EX_UINT32        numBits                    ;
    EX_UINT32        bitMap[FRAGMENT_BIT_NUMBER];
}FragmentNumberSet;

/* FragmentNumberSet 相关编解码工具宏 */
#define SERIALIZE_FRAGMENTNUMBERSET(fragNumSet, memBlock)                       \
    do                                                                          \
    {                                                                           \
        PUT_UNSIGNED_INT(memBlock, fragNumSet.base)                             \
        PUT_UNSIGNED_INT(memBlock, fragNumSet.numBits)                          \
        PUT_UNSIGNED_INT_ARRAY(memBlock, fragNumSet.bitMap, SEQUENCE_BIT_NUMBER)\
    } while (0);

#define DESERIALIZE_FRAGMENTNUMBERSET(fragNumSet, memBlock)                     \
    do                                                                          \
    {                                                                           \
        GET_UNSIGNED_INT(memBlock, fragNumSet.base)                             \
        GET_UNSIGNED_INT(memBlock, fragNumSet.numBits)                          \
        GET_UNSIGNED_INT_ARRAY(memBlock, fragNumSet.bitMap, SEQUENCE_BIT_NUMBER)\
    } while (0);
  
#endif