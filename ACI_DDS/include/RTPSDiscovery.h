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
*  @file     RTPSDiscovery.h
*  @brief    RTPSDiscovery
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
RTPSDiscovery.h
RTPS规范中与Discovery Module相关的功能结构，例如SPDP、SEDP等
********************************************************************/
#ifndef HIPERD_DISCOVERY_H
#define HIPERD_DISCOVERY_H

typedef struct RTPSDiscovery
{
    /* 计算端口号的参数，参见参见RTPS规范的P197   */
    /* 端口号由两个字节表示                       */
    unsigned short pb;
    unsigned short dg;
    unsigned short pg;
    unsigned short d0;
    unsigned short d1;
    unsigned short d2;
    unsigned short d3;

    const char*     groupAddr           ; /* 默认组播地址                             */
    unsigned int    rootParticipantId   ; /* 唯一分配给Participant                    */
    char            unicastAddr[16]     ; /* 本节点的IP地址，可以通过配置蓝图惊醒设置 */
  //  GuidPrefix_t    guidPrefix          ; /* 系统初始化阶段生成的GUID_Prefix          */

}RTPSDiscovery;

extern RTPSDiscovery g_rtps_discovery_info;

extern BOOL InitializeDiscoveryInfo(const char* ipAddr, unsigned int rootParticipantId);

#endif