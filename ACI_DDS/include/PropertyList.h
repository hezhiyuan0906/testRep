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
*  @file     PropertyList.h
*  @brief    PropertyList
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

/********************************************************************
PropertyList.h
存储字符串化名值对的列表结构
********************************************************************/

#ifndef HIPERD_PROPERTY_LIST_H
#define HIPERD_PROPERTY_LIST_H

typedef struct _Property_t
{
    BoundedString32     name    ;
    BoundedString256    value   ;
}Property_t;

#define SERIALIZE_PROPERTY(property , memBlock)             \
    do                                                      \
    {                                                       \
        SERIALIZE_BOUNDED_STRING(property.name, memBlock)   \
        SERIALIZE_BOUNDED_STRING(property.value, memBlock)  \
    } while (0);

#define DESERIALIZE_PROPERTY(property , memBlock)           \
    do                                                      \
    {                                                       \
        DESERIALIZE_BOUNDED_STRING(property.name, memBlock) \
        DESERIALIZE_BOUNDED_STRING(property.value, memBlock)\
     } while (0);


typedef struct PropertyList
{
	unsigned int PropertySize;
	Property_t listBuf[8];
	
}PropertyList;

#define SERIALIZE_PROPERTY_LIST_SPDP(propertyList, memBlock)                        \
    do                                                                              \
    {                                                                               \
		unsigned int i = 0;                                                         \
		PUT_UNSIGNED_INT(memBlock, propertyList.PropertySize)                       \
		for (i = 0; i < propertyList.PropertySize; i++)                             \
		{                                                                           \
			SERIALIZE_PROPERTY(propertyList.listBuf[i], memBlock)                   \
		}                                                                           \
	} while (0);

#define DESERIALIZE_PROPERTY_LIST_SPDP(propertyList, memBlock)                      \
    do                                                                              \
    {                                                                               \
        unsigned int i = 0;                                                         \
        GET_UNSIGNED_INT(memBlock, propertyList.PropertySize)                       \
		for(i = 0; i < propertyList.PropertySize; i++)                              \
		{                                                                           \
			DESERIALIZE_PROPERTY(propertyList.listBuf[i], memBlock)                 \
		}                                                                           \
    } while (0);

extern VOID SetPropertyList(PropertyList* pstPropertyList);

#endif