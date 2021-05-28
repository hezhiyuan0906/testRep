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
*  @file     VendorId_t.h
*  @brief    VendorId_t
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
VendorId_t.h
RTPS规范中VendorId_t对应的数据类型及相关处理机制
********************************************************************/

#ifndef HIPERD_VENDORID_T_H
#define HIPERD_VENDORID_T_H

/* VendorId_t 结构体序列化成Parameter时占用空间的大小 需要考虑字节对齐*/
#define SIZE_OF_VENDORID 2

typedef struct _VendorId_t
{
    EX_UINT8 vendorId[SIZE_OF_VENDORID];
}VendorId_t;

#define VENDORID_IS_EQUAL(leftVendorid , rightVendorid) \
    (## leftVendorid ##.vendorId[0] == ## rightVendorid ##.vendorId[0]) && \
    (## leftVendorid ##.vendorId[1] == ## rightVendorid ##.vendorId[1])

/* 以下为该结构编解码工具宏 */
#define SERIALIZE_VENDORID(vendorID , memBlock)    PUT_BYTE_ARRAY(memBlock,vendorID.vendorId,2)

#define DESERIALIZE_VENDORID(vendorID , memBlock) GET_BYTE_ARRAY(memBlock,vendorID.vendorId,2)

/* 保留的VendorId_t值 */
extern VendorId_t VENDORID_UNKNOWN  ;    /* RTPS保留的VendorId_t值 */
extern VendorId_t VENDORID_EVO      ;
extern VendorId_t VENDORED_RTI      ;

#endif