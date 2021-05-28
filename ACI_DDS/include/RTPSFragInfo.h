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
*  @file     RTPSFragInfo.h
*  @brief    RTPSFragInfo
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
RTPSFragInfo.h
存储某条链路的分片收到的信息
********************************************************************/

#ifndef HIPERD_RTPSFRAGINFO_H
#define HIPERD_RTPSFRAGINFO_H

#define RTPSFRAGINFO_MEMSIZE 16

/* TODO 必须初始化*/
typedef struct RTPSFragInfo
{
    unsigned int        expectTotalFragNum                  ; /* 分片数量                                           */
 //   ReceivedDataSample  deliverRecvSample                   ; /* 提交的样本头                                       */
    GUID_t              writerId                            ; /* 对端writer                                         */ 
    DisjointSequence    recvFragNum                         ; /* 收到的FragmentNumber,此处复用DisjointSequence      */
    BOOL                fragDataValid[RTPSFRAGINFO_MEMSIZE] ; /* 保存的分片数据 ,直接拿FragmentNumber_t当数组下标   */
    MemoryBlock   fragData[RTPSFRAGINFO_MEMSIZE]      ; /* FragmentNumber_t fragDataFN[RTPSFRAGINFO_MEMSIZE]; */ 
    SequenceNumber_t    tmpSeq                              ; /* 缓存的sequenceNumber,避免每次都构造一个临时对象    */
    SequenceNumber_t    upBoundSeq                          ; 
    DisjointSequence    exclude                             ; /* 缓存的空的Disjoint,永远为空                        */
}RTPSFragInfo;

BOOL Initial_RTPSFragInfo(RTPSFragInfo * curInfo);

BOOL returnHeldData_RTPSFragInfo(RTPSFragInfo * curInfo);

BOOL insertNewFrag_RTPSFragInfo(RTPSFragInfo * curInfo, FragmentNumber_t fragNum, MemoryBlock * head);

BOOL isDeliverable_RTPSFragInfo(RTPSFragInfo * curInfo);

#endif
