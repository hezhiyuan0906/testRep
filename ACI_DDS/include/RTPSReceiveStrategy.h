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
*  @file     RTPSReceiveStrategy.h
*  @brief    RTPSReceiveStrategy
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

#ifndef HIPERD_NETWORK_PARSE_H
#define HIPERD_NETWORK_PARSE_H

/* 网络字节序按RTPS协议解析为子报文 */
extern BOOL ParseAllNetworkOrder(CHAR * buffer, UINT32 length, DDS_DomainParticipant* pstParticipant);

extern BOOL ParseNoBlockNetworkOrder(CHAR * buffer, UINT32 length, DDS_DomainParticipant* pstParticipant);

extern BOOL ParseBlockNetworkOrder(CHAR * buffer, UINT32 length, DDS_DomainParticipant* pstParticipant);

#endif