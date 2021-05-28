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
*  @file     DataFrag.h
*  @brief    DataFrag
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

#ifndef HIPERD_DATAFRAG_H
#define HIPERD_DATAFRAG_H

#define DATA_KEY_FLAG_IN_DATAFRAG   0x04            /* 常量 */
#define FRAG_NO_INLINEQOS           38              /* 不含inlineQoS的dataFrag大小 */
#define FRAGNUM_OFFSET              24              /* 分片号的偏移量 */
#define FRAG_INLINEQOS_OFFSET       28              /* 内置QoS的偏移量 */
#define FRAG_SAMPLE_SIZE_OFFSET     32              /* 总的负载大小 */

typedef struct DataFrag
{
    UCHAR               extraFlags[2]           ;
    USHORT              octetsToInlineQos       ;
    EntityId_t          readerId                ;
    EntityId_t          writerId                ;
    SequenceNumber_t    writerSN                ;
    FragmentNumber_t    fragmentStartingNum     ;
    USHORT              fragmentsInSubmessage   ;
    UINT32              dataSize                ;
    USHORT              fragmentSize            ;
    ParameterList       inlineQos               ;
    SerializedData      serializedPayload       ;
    UINT32              uiStartIndex; /* 起始位置 */
}DataFrag;

/* 序列化反序列化工具宏 */
#define SERIALIZE_DATAFRAG(curDataFrag, memBlock)                               \
    do                                                                          \
        {                                                                       \
        curDataFrag.uiStartIndex = memBlock.writeIndex;                         \
        PUT_BYTE_ARRAY(memBlock, curDataFrag.extraFlags, 2)                     \
        PUT_UNSIGNED_SHORT(memBlock, curDataFrag.octetsToInlineQos)             \
        SERIALIZE_ENTITYID(curDataFrag.readerId, memBlock)                      \
        SERIALIZE_ENTITYID(curDataFrag.writerId, memBlock)                      \
        SERIALIZE_SEQUENCENUMBER(curDataFrag.writerSN, memBlock)                \
        PUT_UNSIGNED_LONG(memBlock, curDataFrag.fragmentStartingNum )           \
        PUT_UNSIGNED_SHORT(memBlock, curDataFrag.fragmentsInSubmessage)         \
        PUT_UNSIGNED_SHORT(memBlock, curDataFrag.fragmentSize)                  \
        PUT_UNSIGNED_LONG(memBlock, curDataFrag.dataSize )                      \
    } while (0);

#define DESERIALIZE_DATAFRAG(curDataFrag, memBlock)                             \
    do                                                                          \
    {                                                                           \
        GET_BYTE_ARRAY(memBlock, curDataFrag.extraFlags, 2)                     \
        GET_UNSIGNED_SHORT(memBlock, curDataFrag.octetsToInlineQos)             \
        DESERIALIZE_ENTITYID(curDataFrag.readerId, memBlock)                    \
        DESERIALIZE_ENTITYID(curDataFrag.writerId, memBlock)                    \
        DESERIALIZE_SEQUENCENUMBER(curDataFrag.writerSN, memBlock)              \
        GET_UNSIGNED_LONG(memBlock, curDataFrag.fragmentStartingNum )           \
        GET_UNSIGNED_SHORT(memBlock, curDataFrag.fragmentsInSubmessage)         \
        GET_UNSIGNED_SHORT(memBlock, curDataFrag.fragmentSize)                  \
        GET_UNSIGNED_LONG(memBlock, curDataFrag.dataSize )                      \
    } while (0);

BOOL SetDataFragMessageLength(DataFrag * dataFrag);

#endif