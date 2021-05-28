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
*  @file     DiscoveredParticipant.h
*  @brief    DiscoveredParticipant
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

#ifndef HIPERD_DISCOVEREDPARTICIPANT_H
#define HIPERD_DISCOVEREDPARTICIPANT_H

typedef struct _DiscoveredParticipant
{
    RTPSMessageHeader             stRTPSMsgHeader;
    ParticipantData               dcpsParticipantData;
    ParticipantProxy              rtpsParticipantProxy;
    Duration_t                    leaseDuration;			//远端设置的心跳间隔
	Duration_t					  lastHeartBeat;			//远端上一次心跳的时间
	UINT32						  livelinessHeartMissNum;			//远端连续没收到心跳的次数

	UINT32						IPAddr;//IP地址
    UINT32						netMask;//子网掩码
	int							processID;//进程ID
    struct _DiscoveredParticipant* pNext;
	DDS_MUTEX threadMutex;

}DDS_DiscoveredParticipant;

extern VOID DDS_DiscoveredParticipant_Init(DDS_DiscoveredParticipant* pstSPDPDiscoveredParticipant);

extern VOID DDS_DiscoveredParticipant_Uninit(DDS_DiscoveredParticipant* pstSPDPDiscoveredParticipant);

#endif