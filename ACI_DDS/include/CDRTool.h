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
*  @file     CDRTool.h
*  @brief    CDRTool
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

/********************************************************************************
CDRTool.h
OMG对IDL的CDR编解码相关机制，使用频繁，采用宏

序列化宏：
    用来将对应类型的变量编码到MemoryBlock中。

    单变量操作:第一个参数为MemoryBlock指针，第二个参数为要编码到MemoryBlock中的值。
    PUT_BYTE(MemoryBlock*,EX_UINT8)
    PUT_CHAR(MemoryBlock*,EX_INT8)
    PUT_BOOL(MemoryBlock*,EX_BOOLEN)
    PUT_UNSIGNED_SHORT(MemoryBlock*,EX_UINT16)
    PUT_SHORT(MemoryBlock*,EX_INT16)
    PUT_UNSIGNED_INT(MemoryBlock*,EX_UINT32)
    PUT_INT(MemoryBlock*,EX_INT32)
    PUT_UNSIGNED_LONG(MemoryBlock*,EX_UINT32)
    PUT_LONG(MemoryBlock*,EX_INT32)
    PUT_FLOAT(MemoryBlock*,EX_FLOAT32)
    PUT_UNSIGNED_LONG_LONG(MemoryBlock*,EX_UINT64)
    PUT_LONG_LONG(MemoryBlock*,EX_INT64)
    PUT_DOUBLE(MemoryBlock*,EX_DOUBLE64)

    数组操作:第一个参数为MemoryBlock指针，第二个参数为数组起始地址，第三个参数为数组长度
    PUT_BYTE_ARRAY(MemoryBlock*,EX_UINT8*,int)
    PUT_CHAR_ARRAY(MemoryBlock*,EX_INT*,int)
    PUT_BOOL_ARRAY(MemoryBlock*,EX_BOOLEAN*,int)
    PUT_UNSIGNED_SHORT_ARRAY(MemoryBlock*,EX_UINT16*,int)
    PUT_SHORT_ARRAY(MemoryBlock*,EX_INT16*,int)
    PUT_UNSIGNED_INT_ARRAY(MemoryBlock*,EX_UINT32*,int)
    PUT_INT_ARRAY(MemoryBlock*,EX_INT32*,int)
    PUT_UNSIGNED_LONG_ARRAY(MemoryBlock*,EX_UINT32*,int)
    PUT_LONG_ARRAY(MemoryBlock*,EX_INT32*,int)
    PUT_FLOAT_ARRAY(MemoryBlock*,EX_FLOAT32*,int)
    PUT_UNSIGNED_LONG_LONG_ARRAY(MemoryBlock*,EX_UINT64*,int)
    PUT_LONG_LONG_ARRAY(MemoryBlock*,EX_INT64*,int)
    PUT_DOUBLE_ARRAY(MemoryBlock*,EX_DOUBLE64*,int)

反序列化宏:
    用来从MemoryBlock中解码出相应的值:

    单变量操作:第一个参数为MemoryBlock指针，第二个参数用来存放解析结果。
    GET_BYTE(MemoryBlock*,EX_UINT8)
    GET_CHAR(MemoryBlock*,EX_INT8)
    GET_BOOL(MemoryBlock*,EX_BOOLEAN)
    GET_UNSIGNED_SHORT(MemoryBlock*,EX_UINT16)
    GET_SHORT(MemoryBlock*,EX_INT16)
    GET_UNSIGNED_INT(MemoryBlock*,EX_UINT32)
    GET_INT(MemoryBlock*,EX_INT32)
    GET_UNSIGNED_LONG(MemoryBlock*,EX_UINT32)
    GET_LONG(MemoryBlock*,EX_INT32)
    GET_FLOAT(MemoryBlock*,EX_FLOAT32)
    GET_UNSIGNED_LONG_LONG(MemoryBlock*,ZR_UINT64)
    GET_LONG_LONG(MemoryBlock*,EX_INT64)
    GET_DOUBLE(MemoryBlock*,EX_DOUBLE64)

    数组操作:第一个参数为MemoryBlock指针，第二个参数用来存放解析结果的起始地址，第三个参数为数组长度。
    GET_BYTE_ARRAY(MemoryBlock*,EX_UINT8*,int)
    GET_CHAR_ARRAY(MemoryBlock*,EX_UINT8*,int)
    GET_BOOL_ARRAY(MemoryBlock*,EX_BOOLEAN*,int)
    GET_UNSGINED_SHORT_ARRAY(MemoryBlock*,EX_UINT16*,int)
    GET_SHORT_ARRAY(MemoryBlock*,EX_INT16*,int)
    GET_UNSIGNED_INT_ARRAY(MemoryBlock*,EX_UINT32*,int)
    GET_INT_ARRAY(MemoryBlock*,EX_INT32*,int)
    GET_UNSIGNED_LONG_ARRRAY(MemoryBlock*,EX_UINT32*,int)
    GET_LONG_ARRAY(MemoryBlock*,EX_INT32*,int)
    GET_FLOAT_ARRAY(MemoryBlock*,EX_FLOAT32*,int)
    GET_UNSIGNED_LONG_LONG_ARRAY(MemoryBlock*,EX_UINT64*,int)
    GET_LONG_LONG_ARRAY(MemoryBlock*,EX_INT64*,int)
    GET_DOUBLE_ARRAY(MemoryBlock*,ZR_DOUBLE64*,int)

使用例子:
    void func()
    {
        ...
        char a = 0x09, ar[10];
        ...
        PUT_BYTE(curMemoryBlock, a)
        PUT_BYTE_ARRAY(curMemoryBlock, ar, 10)
        ...
        GET_BYTE(curMemoryBlock, a)
        GET_BYTE_ARRAY(curMemoryBlock, ar, 10)
        ...
    }
        
********************************************************************************/

#ifndef HIPERD_CDRTOOL_H
#define HIPERD_CDRTOOL_H

/***************************************************************************************************/
/***************************************************************************************************/

/* Determin if the machine is little endian */
#define IS_MACHINE_LITTLE_ENDIAN (((union { EX_UINT32 x; EX_UINT8 c; }){1}).c)

/*
Put a byte in MemoryBlock

PUT_BYTE(MemoryBlock*,EX_UINT8)
*/
#define PUT_BYTE(curMemoryBlock,c)                              \
    do{                                                         \
        if((curMemoryBlock.wrPtr + sizeof(unsigned char)) <=    \
           (curMemoryBlock.base + curMemoryBlock.totalSize))    \
        {                                                       \
            *(curMemoryBlock.wrPtr) = c;                        \
            curMemoryBlock.wrPtr += sizeof(unsigned char);      \
            curMemoryBlock.writeIndex += sizeof(unsigned char); \
        }                                                       \
        else                                                    \
        {                                                       \
            /* The remaining memory is less than one byte */    \
            /* TODO: handle error */                            \
        }                                                       \
    } while (0);

/*
PUT_CHAR(MemoryBlock*,EX_INT8)
*/
#define PUT_CHAR(curMemoryBlock,c)    PUT_BYTE(curMemoryBlock,c)

/*
PUT_BOOL(MemoryBlock*,EX_BOOLEN)
*/
#define PUT_BOOL(curMemoryBlock,b)    PUT_BYTE(curMemoryBlock,b)

/*
PUT_UNSIGNED_SHORT(MemoryBlock*,EX_UINT16)
*/
#define PUT_UNSIGNED_SHORT(curMemoryBlock,us1)                                                  \
    do                                                                                          \
    {                                                                                           \
        unsigned int cdr_i;                                                                     \
        unsigned int cdr_offset;                                                                \
        /* handle align*/                                                                       \
        cdr_offset = (((unsigned int)curMemoryBlock.writeIndex & 1) == 0) ? 0 :                 \
                   SHORT_SIZE - ((unsigned int)curMemoryBlock.writeIndex & 1);                  \
        if((curMemoryBlock.writeIndex + SHORT_SIZE + cdr_offset) <= curMemoryBlock.totalSize)   \
        {                                                                                       \
            memset(curMemoryBlock.wrPtr, 0, cdr_offset);                                        \
            curMemoryBlock.wrPtr += cdr_offset;                                                 \
            curMemoryBlock.writeIndex += cdr_offset + SHORT_SIZE;                               \
            /* put data into memoryblock */                                                     \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                        \
            {                                                                                   \
                memcpy(curMemoryBlock.wrPtr, (unsigned char *)(&us1), SHORT_SIZE);              \
                curMemoryBlock.wrPtr += SHORT_SIZE;                                             \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < SHORT_SIZE; cdr_i++)                                    \
                {                                                                               \
                    *(curMemoryBlock.wrPtr)=*((unsigned char *)(&us1)+SHORT_SIZE-cdr_i-1);      \
                    curMemoryBlock.wrPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            /* memory out of range */                                                           \
            /* TODO: handle error */                                                            \
        }                                                                                       \
    } while (0);

/*
PUT_SHORT(MemoryBlock*,EX_INT16)
*/
#define PUT_SHORT(curMemoryBlock,s) PUT_UNSIGNED_SHORT(curMemoryBlock,s)

/*
PUT_UNSIGNED_LONG(MemoryBlock*,EX_UINT32)
*/
#define PUT_UNSIGNED_LONG(curMemoryBlock,ul)                                                    \
    do                                                                                          \
    {                                                                                           \
        int cdr_i;                                                                              \
        int cdr_offset;                                                                         \
        /* handle align*/                                                                       \
        cdr_offset = ((curMemoryBlock.writeIndex & 3) == 0) ? 0 :                               \
                   LONG_SIZE - (curMemoryBlock.writeIndex & 3);                                 \
        if((curMemoryBlock.writeIndex + LONG_SIZE + cdr_offset) <= curMemoryBlock.totalSize)    \
        {                                                                                       \
            memset(curMemoryBlock.wrPtr, 0, cdr_offset);                                        \
            curMemoryBlock.wrPtr += cdr_offset;                                                 \
            curMemoryBlock.writeIndex += cdr_offset + LONG_SIZE;                                \
            /* put data into memoryblock */                                                     \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                        \
            {                                                                                   \
                memcpy(curMemoryBlock.wrPtr, (unsigned char *)(&ul), LONG_SIZE);                \
                curMemoryBlock.wrPtr += LONG_SIZE;                                              \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < LONG_SIZE; cdr_i++)                                     \
                {                                                                               \
                    *(curMemoryBlock.wrPtr) = *((unsigned char *)(&ul) + LONG_SIZE-cdr_i-1);    \
                    curMemoryBlock.wrPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            /* memory out of range */                                                           \
            /* TODO: handle error */                                                            \
        }                                                                                       \
    } while (0);

/*
PUT_LONG(MemoryBlock*,EX_INT32)
*/
#define PUT_LONG(curMemoryBlock,l) PUT_UNSIGNED_LONG(curMemoryBlock,l)    

/*
PUT_UNSIGNED_INT(MemoryBlock*, EX_UINT32)
*/
#define PUT_UNSIGNED_INT(curMemoryBlock,ui) PUT_UNSIGNED_LONG(curMemoryBlock,ui)    

/*
PUT_INT(MemoryBlock*, EX_INT32)
*/
#define PUT_INT(curMemoryBlock,ii) PUT_UNSIGNED_LONG(curMemoryBlock,ii)

/*
PUT_FLOAT(MemoryBlock*,EX_FLOAT32)
*/
#define PUT_FLOAT(curMemoryBlock,f) PUT_UNSIGNED_LONG(curMemoryBlock,f)

/*
PUT_UNSIGNED_LONG_LONG(MemoryBlock*,EX_UINT64)
*/
#define PUT_UNSIGNED_LONG_LONG(curMemoryBlock,ull)                                                      \
    do                                                                                                  \
    {                                                                                                   \
        int cdr_i;                                                                                      \
        int cdr_offset;                                                                                 \
        /* handle align*/                                                                               \
        cdr_offset = ((curMemoryBlock.writeIndex & 7) == 0) ? 0 :                                       \
                   LONGLONG_SIZE - (curMemoryBlock.writeIndex & 7);                                     \
        if((curMemoryBlock.writeIndex + LONGLONG_SIZE + cdr_offset) <= curMemoryBlock.totalSize)        \
        {                                                                                               \
            memset(curMemoryBlock.wrPtr, 0, cdr_offset);                                                \
            curMemoryBlock.wrPtr += cdr_offset;                                                         \
            curMemoryBlock.writeIndex += cdr_offset + LONGLONG_SIZE;                                    \
            /* put data into memoryblock */                                                             \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                                \
            {                                                                                           \
                memcpy(curMemoryBlock.wrPtr, (unsigned char *)(&ull), LONGLONG_SIZE);                   \
                curMemoryBlock.wrPtr += LONGLONG_SIZE;                                                  \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                for (cdr_i = 0; cdr_i < LONGLONG_SIZE; cdr_i++)                                         \
                {                                                                                       \
                    *(curMemoryBlock.wrPtr) = *((unsigned char *)(&ull) + LONGLONG_SIZE - cdr_i - 1);   \
                    curMemoryBlock.wrPtr += 1;                                                          \
                }                                                                                       \
            }                                                                                           \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
            /* memory out of range */                                                                   \
            /* TODO: handle error */                                                                    \
        }                                                                                               \
    } while (0);

/*
PUT_LONG_LONG(MemoryBlock*,EX_INT64)
*/
#define PUT_LONG_LONG(curMemoryBlock,ll) PUT_UNSIGNED_LONG_LONG(curMemoryBlock,ll)

/*
PUT_DOUBLE(MemoryBlock*,EX_DOUBLE64)
*/
#define PUT_DOUBLE(curMemoryBlock,d) PUT_UNSIGNED_LONG_LONG(curMemoryBlock,d)

/*
PUT_BYTE_ARRAY(MemoryBlock*,EX_UINT8*,int)
*/
#define PUT_BYTE_ARRAY(curMemoryBlock,c,size)                       \
    do                                                              \
    {                                                               \
        if(size > 0)                                                \
        {                                                           \
            if((curMemoryBlock.wrPtr + size) <=                     \
                (curMemoryBlock.base + curMemoryBlock.totalSize))   \
            {                                                       \
                memcpy(curMemoryBlock.wrPtr, (char *)c, size);      \
                curMemoryBlock.wrPtr += size;                       \
                curMemoryBlock.writeIndex += size;                  \
            }                                                       \
            else                                                    \
            {                                                       \
                /* TODO: handle error */                            \
            }                                                       \
        }                                                           \
    } while (0);

/*
PUT_CHAR_ARRAY(MemoryBlock*,EX_INT*,int)
*/
#define PUT_CHAR_ARRAY(curMemoryBlock,c,size) PUT_BYTE_ARRAY(curMemoryBlock,c,size)

/*
PUT_BOOL_ARRAY(MemoryBlock*,EX_BOOLEAN*,int)
*/
#define PUT_BOOL_ARRAY(curMemoryBlock,c,size) PUT_BYTE_ARRAY(curMemoryBlock,c,size)

/*
PUT_UNSIGNED_SHORT_ARRAY(MemoryBlock*,EX_UINT16*,int)
*/
#define PUT_UNSIGNED_SHORT_ARRAY(curMemoryBlock,us,size)                                \
    do                                                                                  \
    {                                                                                   \
        unsigned int cdr_i;                                                             \
        unsigned short val;                                                             \
        unsigned int cdr_remainBytes;                                                   \
        if(size > 0)                                                                    \
        {                                                                               \
            /* handle align*/                                                           \
            PUT_UNSIGNED_SHORT(curMemoryBlock, us[0]);                                  \
            cdr_remainBytes = size*SHORT_SIZE - SHORT_SIZE;                             \
            if ((curMemoryBlock.wrPtr + cdr_remainBytes) <=                             \
                (curMemoryBlock.base + curMemoryBlock.totalSize) && cdr_remainBytes > 0)\
            {                                                                           \
                if(IS_MACHINE_LITTLE_ENDIAN)                                            \
                {                                                                       \
                    memcpy(curMemoryBlock.wrPtr, (char *)(us + 1), cdr_remainBytes);    \
                    curMemoryBlock.wrPtr += cdr_remainBytes;                            \
                    curMemoryBlock.writeIndex += cdr_remainBytes;                       \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                              \
                    {                                                                   \
                        val = us[cdr_i];                                                \
                        PUT_UNSIGNED_SHORT(curMemoryBlock, val);                        \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                /* TODO: handle error */                                                \
            }                                                                           \
        }                                                                               \
    } while (0);

/*
PUT_SHORT_ARRAY(MemoryBlock*,EX_INT16*,int)
*/
#define PUT_SHORT_ARRAY(curMemoryBlock,s,size) PUT_UNSIGNED_SHORT_ARRAY(curMemoryBlock,s,size)

/*
PUT_UNSIGNED_LONG_ARRAY(MemoryBlock*,EX_UINT32*,int)
*/                                                                                      \
#define PUT_UNSIGNED_LONG_ARRAY(curMemoryBlock,ul,size)                                 \
    do                                                                                  \
    {                                                                                   \
        int cdr_i;                                                                      \
        int cdr_remainBytes;                                                            \
        /* handle align*/                                                               \
        if(size > 0)                                                                    \
        {                                                                               \
            PUT_UNSIGNED_LONG(curMemoryBlock, ul[0]);                                   \
            cdr_remainBytes = size*LONG_SIZE - LONG_SIZE;                               \
            if ((curMemoryBlock.wrPtr + cdr_remainBytes) <=                             \
                (curMemoryBlock.base + curMemoryBlock.totalSize) && cdr_remainBytes > 0)\
            {                                                                           \
                if(IS_MACHINE_LITTLE_ENDIAN)                                            \
                {                                                                       \
                    memcpy(curMemoryBlock.wrPtr, (char *)(ul + 1), cdr_remainBytes);    \
                    curMemoryBlock.wrPtr += cdr_remainBytes;                            \
                    curMemoryBlock.writeIndex += cdr_remainBytes;                       \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                              \
                    {                                                                   \
                        PUT_UNSIGNED_LONG(curMemoryBlock, ul[cdr_i]);                   \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                /* TODO: handle error */                                                \
            }                                                                           \
        }                                                                               \
    } while (0);

/*
PUT_LONG_ARRAY(MemoryBlock*,EX_INT32*,int)
*/
#define PUT_LONG_ARRAY(curMemoryBlock,l,size) PUT_UNSIGNED_LONG_ARRAY(curMemoryBlock,l,size)

/*
PUT_UNSIGNED_INT_ARRAY(MemoryBlock*, EX_UINT32*, int)
*/
#define PUT_UNSIGNED_INT_ARRAY(curMemoryBlock,ui,size) PUT_UNSIGNED_LONG_ARRAY(curMemoryBlock,ui,size)

/*
PUT_INT_ARRAY(MemoryBlock*, EX_INT32*, int)
*/
#define PUT_INT_ARRAY(curMemoryBlock,ii,size) PUT_UNSIGNED_LONG_ARRAY(curMemoryBlock,ii,size)

/*
PUT_FLOAT_ARRAY(MemoryBlock*,EX_FLOAT32*,int)
*/
#define PUT_FLOAT_ARRAY(curMemoryBlock,f,size) PUT_UNSIGNED_LONG_ARRAY(curMemoryBlock,f,size)

/*
PUT_UNSIGNED_LONG_LONG_ARRAY(MemoryBlock*,EX_UINT64*,int)
*/
#define PUT_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,ull,size)                           \
    do                                                                                  \
    {                                                                                   \
        int cdr_i;                                                                      \
        int cdr_remainBytes;                                                            \
        /* handle align*/                                                               \
        if(size > 0)                                                                    \
        {                                                                               \
            PUT_UNSIGNED_LONG_LONG(curMemoryBlock, ull[0]);                             \
            cdr_remainBytes = size*LONGLONG_SIZE - LONGLONG_SIZE;                       \
            if ((curMemoryBlock.wrPtr + cdr_remainBytes) <=                             \
                (curMemoryBlock.base + curMemoryBlock.totalSize) && cdr_remainBytes > 0)\
            {                                                                           \
                if(IS_MACHINE_LITTLE_ENDIAN)                                            \
                {                                                                       \
                    memcpy(curMemoryBlock.wrPtr, (char *)(ull + 1), cdr_remainBytes);   \
                    curMemoryBlock.wrPtr += cdr_remainBytes;                            \
                    curMemoryBlock.writeIndex += cdr_remainBytes;                       \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                              \
                    {                                                                   \
                    PUT_UNSIGNED_LONG_LONG(curMemoryBlock, ull[cdr_i]);                 \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                /* TODO: handle error */                                                \
            }                                                                           \
        }                                                                               \
    } while (0);
    
/*
PUT_LONG_LONG_ARRAY(MemoryBlock*,EX_INT64*,int)
*/
#define PUT_LONG_LONG_ARRAY(curMemoryBlock,ll,size) PUT_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,ll,size)

/*
PUT_DOUBLE_ARRAY(MemoryBlock*,EX_DOUBLE64*,int)
*/
#define PUT_DOUBLE_ARRAY(curMemoryBlock,d,size) PUT_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,d,size)
    
/***************************************************************************************************/
/***************************************************************************************************/

/*
Get a byte form MemoryBlock and then move the read pointer of the
memoryblock one byte forward

GET_BYTE(MemoryBlock*,EX_UINT8)
*/                                                              \
#define GET_BYTE(curMemoryBlock,c)                              \
    do{                                                         \
        /* Determine if the memoryblock length is valid */      \
        if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr >= 1)   \
        {                                                       \
            c = *(curMemoryBlock.rdPtr);                        \
            curMemoryBlock.rdPtr += 1;                          \
            curMemoryBlock.readIndex += 1;                      \
        }                                                       \
        else                                                    \
        {                                                       \
            /* The remaining length is less than one byte */    \
            /* TODO: handle error */                            \
        }                                                       \
    } while (0);

/*
Get a char from MemoryBlock 

GET_CHAR(MemoryBlock*,EX_INT8)
*/
#define GET_CHAR(curMemoryBlock,c)    GET_BYTE(curMemoryBlock,c)

/*
Get a bool from MemoryBlock

GET_BOOL(MemoryBlock*,EX_BOOLEAN)
*/
#define GET_BOOL(curMemoryBlock,b) GET_BYTE(curMemoryBlock,b)

/*
Get a unsigned short from MemoryBlock and then move the read pointer of the
memoryblock two byte forward

GET_UNSIGNED_SHORT(MemoryBlock*,EX_UINT16)
*/
#define GET_UNSIGNED_SHORT(curMemoryBlock,us)                                                   \
    do{                                                                                         \
        int cdr_i;                                                                              \
        int cdr_offset;                                                                         \
        /* Determine if the memoryblock length is valid */                                      \
        /* handle align*/                                                                       \
        cdr_offset = ((curMemoryBlock.readIndex % SHORT_SIZE) == 0) ? 0 :                       \
                    SHORT_SIZE - curMemoryBlock.readIndex % SHORT_SIZE;                         \
        if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= SHORT_SIZE)             \
        {                                                                                       \
            curMemoryBlock.rdPtr += cdr_offset;                                                 \
            curMemoryBlock.readIndex += cdr_offset + SHORT_SIZE;                                \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                        \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < SHORT_SIZE; cdr_i++)                                    \
                {                                                                               \
                    *(((unsigned char*)(&us)) + cdr_i) = *(curMemoryBlock.rdPtr);               \
                    curMemoryBlock.rdPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < SHORT_SIZE; cdr_i++)                                    \
                {                                                                               \
                    *(((unsigned char*)(&us))+SHORT_SIZE-1-cdr_i) = *(curMemoryBlock.rdPtr);    \
                    curMemoryBlock.rdPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            /* The remaining length is less than two byte */                                    \
            /* TODO: handle error */                                                            \
        }                                                                                       \
    } while (0);

/*
GET_SHORT(MemoryBlock*,EX_INT16)
*/
#define GET_SHORT(curMemoryBlock,s) GET_UNSIGNED_SHORT(curMemoryBlock,s)

/*
GET_UNSIGNED_LONG(MemoryBlock*,EX_UINT32)
*/
#define GET_UNSIGNED_LONG(curMemoryBlock,ul)                                                    \
    do{                                                                                         \
        int cdr_i;                                                                              \
        int cdr_offset;                                                                         \
        /* Determine if the memoryblock length is valid */                                      \
        /* handle align*/                                                                       \
        cdr_offset = ((curMemoryBlock.readIndex % LONG_SIZE) == 0) ? 0 :                        \
                    LONG_SIZE - curMemoryBlock.readIndex % LONG_SIZE;                           \
        if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= LONG_SIZE)              \
        {                                                                                       \
            curMemoryBlock.rdPtr += cdr_offset;                                                 \
            curMemoryBlock.readIndex += cdr_offset + LONG_SIZE;                                 \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                        \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < LONG_SIZE; cdr_i++)                                     \
                {                                                                               \
                    *(((unsigned char*)(&ul)) + cdr_i) = *(curMemoryBlock.rdPtr);               \
                    curMemoryBlock.rdPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                for (cdr_i = 0; cdr_i < LONG_SIZE; cdr_i++)                                     \
                {                                                                               \
                    *(((unsigned char*)(&ul)) + LONG_SIZE-1-cdr_i) = *(curMemoryBlock.rdPtr);   \
                    curMemoryBlock.rdPtr += 1;                                                  \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            /* The remaining length is less than four byte */                                   \
            /* TODO: handle error */                                                            \
        }                                                                                       \
    } while (0);
/*
GET_LONG(MemoryBlock*,EX_INT32)
*/
#define GET_LONG(curMemoryBlock,l) GET_UNSIGNED_LONG(curMemoryBlock,l)

/*
GET_UNSIGNED_INT(MemoryBlock*, EX_UINT32)
*/
#define GET_UNSIGNED_INT(curMemoryBlock,ui) GET_UNSIGNED_LONG(curMemoryBlock,ui)

/*
GET_INT(MemoryBlock*, EX_INT32)
*/
#define GET_INT(curMemoryBlock,ii) GET_UNSIGNED_LONG(curMemoryBlock,ii)

/*
GET_FLOAT(MemoryBlock*,EX_FLOAT32)
*/
#define GET_FLOAT(curMemoryBlock,f) GET_UNSIGNED_LONG(curMemoryBlock,f)

/*
GET_UNSIGNED_LONG_LONG(MemoryBlock*,ZR_UINT64)
*/
#define GET_UNSIGNED_LONG_LONG(curMemoryBlock,ull)                                                  \
    do{                                                                                             \
        int cdr_i;                                                                                  \
        int cdr_offset;                                                                             \
        /* Determine if the memoryblock length is valid */                                          \
        /* handle align*/                                                                           \
        cdr_offset = ((curMemoryBlock.readIndex % LONGLONG_SIZE) == 0) ? 0 :                        \
                    LONGLONG_SIZE - curMemoryBlock.readIndex % LONGLONG_SIZE;                       \
        if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= LONGLONG_SIZE)              \
        {                                                                                           \
            curMemoryBlock.rdPtr += cdr_offset;                                                     \
            curMemoryBlock.readIndex += cdr_offset + LONGLONG_SIZE;                                 \
            if(IS_MACHINE_LITTLE_ENDIAN)                                                            \
            {                                                                                       \
                for (cdr_i = 0; cdr_i < LONGLONG_SIZE; cdr_i++)                                     \
                {                                                                                   \
                    *(((unsigned char*)(&ull)) + cdr_i) = *(curMemoryBlock.rdPtr);                  \
                    curMemoryBlock.rdPtr += 1;                                                      \
                }                                                                                   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                for (cdr_i = 0; cdr_i < LONGLONG_SIZE; cdr_i++)                                     \
                {                                                                                   \
                    *(((unsigned char*)(&ull)) + LONGLONG_SIZE-1-cdr_i) = *(curMemoryBlock.rdPtr);  \
                    curMemoryBlock.rdPtr += 1;                                                      \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            /* The remaining length is less than eight byte */                                      \
            /* TODO: handle error */                                                                \
        }                                                                                           \
    } while (0);

/*
GET_LONG_LONG(MemoryBlock*,EX_INT64)
*/
#define GET_LONG_LONG(curMemoryBlock,ll) GET_UNSIGNED_LONG_LONG(curMemoryBlock,ll)


/*
GET_DOUBLE(MemoryBlock*,EX_DOUBLE64)
*/
#define GET_DOUBLE(curMemoryBlock,d) GET_UNSIGNED_LONG_LONG(curMemoryBlock,d)

/*
GET_BYTE_ARRAY(MemoryBlock*,EX_UINT8*,int)
*/
#define GET_BYTE_ARRAY(curMemoryBlock,c,size)                           \
    do{                                                                 \
        if(size > 0)                                                    \
        {                                                               \
            if ((UINT32)(curMemoryBlock.wrPtr - curMemoryBlock.rdPtr) >= size)    \
            {                                                           \
                memcpy(c, curMemoryBlock.rdPtr, size);                  \
                curMemoryBlock.rdPtr += size;                           \
                curMemoryBlock.readIndex += size;                       \
            }                                                           \
            else                                                        \
            {                                                           \
                /* TODO: handle error */                                \
            }                                                           \
        }                                                               \
    } while (0);

/*
GET_CHAR_ARRAY(MemoryBlock*,EX_UINT8*,int)
*/
#define GET_CHAR_ARRAY(curMemoryBlock,c,size) GET_BYTE_ARRAY(curMemoryBlock,c,size)

/*
GET_BOOL_ARRAY(MemoryBlock*,EX_BOOLEAN*,int)
*/
#define GET_BOOL_ARRAY(curMemoryBlock,b,size) GET_BYTE_ARRAY(curMemoryBlock,b,size)

/*
GET_UNSGINED_SHORT_ARRAY(MemoryBlock*,EX_UINT16*,int)
*/
#define GET_UNSIGNED_SHORT_ARRAY(curMemoryBlock,us,size)                                    \
    do{                                                                                     \
        int cdr_i;                                                                          \
        int cdr_offset;                                                                     \
        unsigned short * pVal;                                                              \
        int cdr_remainBytes;                                                                \
        /* handle align*/                                                                   \
        if(size > 0)                                                                        \
        {                                                                                   \
            cdr_offset = (((unsigned int)curMemoryBlock.rdPtr % SHORT_SIZE) == 0) ? 0 :     \
                SHORT_SIZE - (unsigned int)curMemoryBlock.rdPtr % SHORT_SIZE;               \
            if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= SHORT_SIZE*size)\
            {                                                                               \
                curMemoryBlock.rdPtr += cdr_offset;                                         \
                curMemoryBlock.readIndex += cdr_offset;                                     \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                /* The remaining length is less than two byte */                            \
                /* TODO: handle error */                                                    \
            }                                                                               \
            GET_UNSIGNED_SHORT(curMemoryBlock, us[0]);                                      \
            cdr_remainBytes = size*SHORT_SIZE - SHORT_SIZE;                                 \
            if(cdr_remainBytes > 0)                                                         \
            {                                                                               \
                if(IS_MACHINE_LITTLE_ENDIAN)                                                \
                {                                                                           \
                    memcpy((char *)(us + 1),curMemoryBlock.rdPtr,cdr_remainBytes);          \
                    curMemoryBlock.rdPtr += cdr_remainBytes;                                \
                    curMemoryBlock.readIndex += cdr_remainBytes;                            \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                                  \
                    {                                                                       \
                        pVal = &us[cdr_i];                                                  \
                        GET_UNSIGNED_SHORT(curMemoryBlock, *pVal);                          \
                    }                                                                       \
                }                                                                           \
            }                                                                               \
        }                                                                                   \
    } while (0);

/*
GET_SHORT_ARRAY(MemoryBlock*,EX_INT16*,int)
*/
#define GET_SHORT_ARRAY(curMemoryBlock,s,size) GET_UNSIGNED_SHORT_ARRAY(curMemoryBlock,s,size)

/*
GET_UNSIGNED_LONG_ARRRAY(MemoryBlock*,EX_UINT32*,int)
*/
#define GET_UNSIGNED_LONG_ARRAY(curMemoryBlock,ul,size)                                     \
    do{                                                                                     \
        int cdr_i;                                                                          \
        int cdr_offset;                                                                     \
        int cdr_remainBytes;                                                                \
        if(size > 0 )                                                                       \
        {                                                                                   \
            cdr_offset = (((unsigned int)curMemoryBlock.rdPtr % LONG_SIZE) == 0) ? 0 :      \
                LONG_SIZE - (unsigned int)curMemoryBlock.rdPtr % LONG_SIZE;                 \
            if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= LONG_SIZE*size) \
            {                                                                               \
                curMemoryBlock.rdPtr += cdr_offset;                                         \
                curMemoryBlock.readIndex += cdr_offset;                                     \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                /* The remaining length is less than two byte */                            \
                /* TODO: handle error */                                                    \
            }                                                                               \
            GET_UNSIGNED_LONG(curMemoryBlock, ul[0]);                                       \
            cdr_remainBytes = size*LONG_SIZE - LONG_SIZE;                                   \
            if(cdr_remainBytes > 0)                                                         \
            {                                                                               \
                if(IS_MACHINE_LITTLE_ENDIAN)                                                \
                {                                                                           \
                    memcpy((char *)(ul + 1),curMemoryBlock.rdPtr,cdr_remainBytes);          \
                    curMemoryBlock.rdPtr += cdr_remainBytes;                                \
                    curMemoryBlock.readIndex += cdr_remainBytes;                            \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                                  \
                    {                                                                       \
                        GET_UNSIGNED_LONG(curMemoryBlock, ul[cdr_i]);                       \
                    }                                                                       \
                }                                                                           \
            }                                                                               \
        }                                                                                   \
    } while (0);

/*
GET_LONG_ARRAY(MemoryBlock*,EX_INT32*,int)
*/
#define GET_LONG_ARRAY(curMemoryBlock,l,size) GET_UNSIGNED_LONG_ARRAY(curMemoryBlock,l,size)

/*
GET_UNSIGNED_INT_ARRAY(MemoryBlock*, EX_UINT32*, int)
*/
#define GET_UNSIGNED_INT_ARRAY(curMemoryBlock,ui,size) GET_UNSIGNED_LONG_ARRAY(curMemoryBlock,ui,size)

/*
GET_INT_ARRAY(MemoryBlock*, EX_INT32*, int)
*/
#define GET_INT_ARRAY(curMemoryBlock,ii,size) GET_UNSIGNED_LONG_ARRAY(curMemoryBlock,ii,size)

/*
GET_FLOAT_ARRAY(MemoryBlock*,EX_FLOAT32*,int)
*/
#define GET_FLOAT_ARRAY(curMemoryBlock,f,size) GET_UNSIGNED_LONG_ARRAY(curMemoryBlock,f,size)

/*
GET_UNSIGNED_LONG_LONG_ARRAY(MemoryBlock*,EX_UINT64*,int)
*/                                                                                              \
#define GET_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,ull,size)                                   \
    do{                                                                                         \
        int cdr_i;                                                                              \
        int cdr_offset;                                                                         \
        int cdr_remainBytes;                                                                    \
        if(size > 0)                                                                            \
        {                                                                                       \
            cdr_offset = (((unsigned int)curMemoryBlock.rdPtr % LONGLONG_SIZE) == 0) ? 0 :      \
                LONGLONG_SIZE - (unsigned int)curMemoryBlock.rdPtr % LONGLONG_SIZE;             \
            if (curMemoryBlock.wrPtr - curMemoryBlock.rdPtr - cdr_offset >= LONGLONG_SIZE*size) \
            {                                                                                   \
                curMemoryBlock.rdPtr += cdr_offset;                                             \
                curMemoryBlock.readIndex += cdr_offset;                                         \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                /* The remaining length is less than two byte */                                \
                /* TODO: handle error */                                                        \
            }                                                                                   \
            GET_UNSIGNED_LONG_LONG(curMemoryBlock, ull[0]);                                     \
            cdr_remainBytes = size*LONGLONG_SIZE - LONGLONG_SIZE;                               \
            if (cdr_remainBytes > 0)                                                            \
            {                                                                                   \
                if(IS_MACHINE_LITTLE_ENDIAN)                                                    \
                {                                                                               \
                    memcpy((char *)(ull + 1),curMemoryBlock.rdPtr,cdr_remainBytes);             \
                    curMemoryBlock.rdPtr += cdr_remainBytes;                                    \
                    curMemoryBlock.readIndex += cdr_remainBytes;                                \
                }                                                                               \
                else                                                                            \
                {                                                                               \
                    for (cdr_i = 1; cdr_i < size; cdr_i++)                                      \
                    {                                                                           \
                        GET_UNSIGNED_LONG_LONG(curMemoryBlock, ull[cdr_i]);                     \
                    }                                                                           \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
    } while (0);

/*
GET_LONG_LONG_ARRAY(MemoryBlock*,EX_INT64*,int)
*/
#define GET_LONG_LONG_ARRAY(curMemoryBlock,ll,size) GET_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,ll,size)

/*
GET_DOUBLE_ARRAY(MemoryBlock*,ZR_DOUBLE64*,int)
*/
#define GET_DOUBLE_ARRAY(curMemoryBlock,d,size) GET_UNSIGNED_LONG_LONG_ARRAY(curMemoryBlock,d,size)



#define BIG_LITTLE_SWAP_UINT16(A) (((A & 0xff00) >> 8) | ((A & 0x00ff) << 8))
 
#define BIG_LITTLE_SWAP_UINT32(A) (((A & 0xff000000) >> 24) | ((A & 0x00ff0000) >> 8) | ((A & 0x000000ff) << 24))

#define BIG_LITTLE_SWAP_UINT64(A) (((A & 0xff00000000000000) >> 56) | ((A & 0x00ff000000000000) >> 40) | ((A & 0x0000ff0000000000) >> 24)  | ((A & 0x000000ff00000000) >> 8) \
                              (A & 0x00000000000000ff) << 8) | ((A & 0x000000000000ff00) << 24) | ((A & 0x0000000000ff0000) << 40)  | ((A & 0x00000000ff000000) << 56))
                              
/***************************************************************************************************/
/***************************************************************************************************/

#endif