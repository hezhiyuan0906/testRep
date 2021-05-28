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
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  @file     NetWorkOrder.h
*  @brief    Network order convert
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/07/24
*  @license  GNU General Public License (GPL)
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/07/28 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_NETWORK_ORDER_H
#define HIPERD_NETWORK_ORDER_H

typedef struct _ParameterHead
{
	CHAR* pcLen;
	/* 唯一标识参数类型 */
	ParamType paramType;
	/* 该参数值的长度 */
	USHORT length;

}ParameterHead;

#define SERIALIZE_PARAM_HEAD(stParamHead , memBlock)               \
    do                                                             \
    {                                                              \
        PUT_UNSIGNED_SHORT(memBlock, stParamHead.paramType)        \
        stParamHead.pcLen = memBlock.wrPtr;                        \
        PUT_UNSIGNED_SHORT(memBlock, stParamHead.length)           \
    } while (0);

#define DESERIALIZE_PARAM_HEAD(stParamHead , memBlock)             \
    do                                                             \
    {                                                              \
        GET_UNSIGNED_SHORT(memBlock, stParamHead.paramType)        \
        GET_UNSIGNED_SHORT(memBlock, stParamHead.length)           \
    } while (0);

/*网络字节转换函数声明*/


/* 远端写入器链表结构*/
typedef struct _DDS_DiscWriter
{
    GUID_t  guid;

    struct _DDS_DiscWriter*  pNext;

}DDS_DiscWriter;

/* 远端阅读器链表结构*/
typedef struct _DDS_DiscReader
{
    GUID_t  guid;

    struct _DDS_DiscReader*  pNext;

}DDS_DiscReader;

/* 本地写入器*/
typedef struct _DDS_Writer
{
    GUID_t  guid;

    DDS_DiscReader* pstDiscReader;

}DDS_Writer;

/* 本地阅读器*/
typedef struct _DDS_Reader
{
    GUID_t  guid;

    DDS_DiscWriter* pstDiscWriter;

}DDS_Reader;

/* 本地主题链表结构 */
typedef struct _DDS_Topic
{
    GUID_t  tpGuid;

    BoundedString256  topicName;
    BoundedString256  topicType;

    DDS_Writer* pstWriter;
    DDS_Reader* pstReader;

    struct _DDS_Topic*  pNext;

}DDS_MonTopic;

/* 参与者链表结构 */
typedef struct _DDS_Participant
{
    RTPSMessageHeader  stRTPSMsgHeader;
    Locator_t*         unicastLocator;
    BoundedString256   appName;
    DDS_MonTopic*      pstTopic;

    struct _DDS_Participant*  pNext;

}DDS_Participant;

extern VOID MonParticipantConvertNetworkByteOrder(const DDS_DomainParticipant * pstParticipant, MemoryBlock* pstMemBlock);

extern BOOL NetworkByteOrderConvertMonParticant(MemoryBlock*  pstMemBlock, DDS_Participant * pstParticipant);

extern VOID ParticipantConvertNetworkByteOrder(const DDS_DomainParticipant* pstParticipant, MemoryBlock* pstMemBlock);

extern VOID DataWriterConvertNetworkByteOrder(const DDS_Topic* pstTopic, const DDS_DataWriter* pstDataWriter, MemoryBlock* pstMemBlock);

extern VOID DataReaderConvertNetworkByteOrder(const DDS_Topic* pstTopic, const DDS_DataReader* pstDataReader, MemoryBlock* pstMemBlock);

extern BOOL NetworkByteOrderConvertDiscParticipant(MemoryBlock* pstMemBlock, DDS_DiscoveredParticipant * pstDiscParticipant);

extern VOID NetworkByteOrderConvertDiscDataWriter(MemoryBlock* pstMemBlock, DDS_Topic* pstTopic, DDS_DiscoveredWriter* pstDataWriter);

extern VOID NetworkByteOrderConvertDiscDataReader(MemoryBlock* pstMemBlock, DDS_Topic* pstTopic, DDS_DiscoveredReader* pstDataReader);

extern VOID NetworkByteOrderConvertInlineQos(MemoryBlock*  pstMemBlock, InlineQos* pstInlineQos);
#endif