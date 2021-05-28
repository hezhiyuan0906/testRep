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
*  @file     GuidSeq.h
*  @brief    GuidSeq
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
GuidSeq.h
********************************************************************/

#ifndef HIPERD_GUIDSEQ_H
#define HIPERD_GUIDSEQ_H

typedef struct GuidSeq
{
	EX_BOOLEAN valid;  //
	EX_BOOLEAN isRemote; //‘∂∂À±Í÷æ
    GUID_t guid;
}GuidSeq;

BOOL Initial_GuidSeq(GuidSeq * curGuidSeq);

#endif
