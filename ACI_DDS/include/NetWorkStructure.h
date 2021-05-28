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
*  @file     NetWorkStructure.h
*  @brief    NetWorkStructure
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
#ifndef NET_WORK_STRUCTURE_H
#define NET_WORK_STRUCTURE_H

extern HANDLE g_hDev;

extern BOOL InitNetWorkStructure(DDS_DomainParticipant*  pstDomainParticipant, USHORT domainId);

extern BOOL getNetMask(DDS_DomainParticipant* pstDomainParticipant, char* ipAddr);

#endif