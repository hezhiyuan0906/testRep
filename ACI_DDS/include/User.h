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
*  @file     User.h
*  @brief    User
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/01
*  @license  
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/01 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_USER_H
#define HIPERD_USER_H

/* 组装用户数据报文发送位指定reader */
extern DDS_ReturnCode_t JointUSERDataMsgSendUnknownReader(DDS_DataWriter* pstDataWriter, HistoryData* pstHistoryData);

/* 组装用户数据报文发送指定阅读器 */
extern VOID JointUSERDataMsgSendAssignReader(DDS_DataWriter* pstDataWriter, DDS_DiscoveredReader*  pstDiscReader, HistoryData* pstHistoryData);

/* 处理用户数据报文 */
extern BOOL HandleUSERWriterDataMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Data* pstDataMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant);

extern BOOL HandleUSERWriterDataFrag(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, DataFrag* pstDataFrag, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant);

/*处理Gap报文*/
extern BOOL HandleGapMsg(RTPSMessageHeader* pstRTPSMsgHeader, SubMessageHeader*  pstSubMsgHeader, InfoTimestamp* pstTimestamp, Gap* pstGapMsg, MemoryBlock* pstMemBlock, DDS_DomainParticipant* pstParticipant);

#endif
