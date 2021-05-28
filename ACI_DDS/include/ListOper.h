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
*  @file     ListOper.h
*  @brief    ListOper
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

#ifndef __LIST_OPER_H_INCLUDED__
#define __LIST_OPER_H_INCLUDED__

#define   FUNC_NAME    __FUNCTION__ //(__func__)

/* 指针校验宏 */
//若无返回值则retVal置RETURN_VOID
#define RETURN_VOID
#define CHECK_SINGLE_POINTER(ptr1, retVal) do{\
    if(NULL == (ptr1)) \
    {   \
        \
            printf("[%s(%d)]Null Pointer: "#ptr1"!\n\r", FUNC_NAME, __LINE__); \
            return retVal; \
    } \
}while (0)

#define CHECK_DOUBLE_POINTER(ptr1, ptr2, retVal) do{\
    if((NULL == (ptr1)) || (NULL == (ptr2))) \
        { \
        printf("[%s(%d)]Null Pointer: "#ptr1"(%p), "#ptr2"(%p)!\n\r", FUNC_NAME, __LINE__, ptr1, ptr2); \
        return retVal; \
        } \
        }while(0)

#define CHECK_TRIPLE_POINTER(ptr1, ptr2, ptr3, retVal) do{\
    if((NULL == (ptr1)) || (NULL == (ptr2)) || (NULL == (ptr3))) \
        { \
        printf("[%s(%d)]Null Pointer: "#ptr1"(%p), "#ptr2"(%p), "#ptr3"(%p)!\n\r", FUNC_NAME, __LINE__, ptr1, ptr2, ptr3); \
        return retVal; \
        } \
        }while(0)

// 创建结点为作为链表头以生成双向循环空链表
#define LIST_INIT_NODE(pNode) do{              \
    (pNode)->pNext = (pNode)->pPrev = (pNode); \
        }while(0)

//"孤立"链表结点，避免通过该结点访问其前驱和后继结点(进而遍历链表)
#define LIST_ISOL_NODE(pNode) do{           \
    (pNode)->pNext = (pNode)->pPrev = NULL; \
        }while(0)

//判断链表是否仅含头结点
#define LIST_WITH_HEAD(pHeadNode) do{                                           \
    (((pHeadNode)->pPrev == (pHeadNode)) && ((pHeadNode->pNext == pHeadNode))); \
        }while(0)

//插入链表结点
#define LIST_INSERT_NODE(prevNode, insertNode) do{ \
    (insertNode)->pNext      = (prevNode)->pNext;  \
    (insertNode)->pPrev      = (prevNode);         \
    (prevNode)->pNext->pPrev = (insertNode);       \
    (prevNode)->pNext        = (insertNode);       \
        }while(0)

//删除链表结点
#define LIST_REMOVE_NODE(removeNode) do{ \
    (removeNode)->pPrev->pNext = (removeNode)->pNext;  \
    (removeNode)->pNext->pPrev = (removeNode)->pPrev;  \
        }while(0)

//获取链表结点及其数据(不做安全性检查)
#define GET_NODE_NUM(pList)      ((pList)->dwNodeNum)
#define GET_HEAD_NODE(pList)     ((pList)->pHead)
#define GET_TAIL_NODE(pList)     ((pList)->pTail)
#define GET_PREV_NODE(pNode)     ((pNode)->pPrev)
#define GET_NEXT_NODE(pNode)     ((pNode)->pNext)
#define GET_NODE_DATA(pNode)     ((pNode)->pvNodeData)

//双向循环链表遍历校验宏
#define LIST_ITER_CHECK(pList, retVal) do{               \
    CHECK_SINGLE_POINTER((pList), retVal);               \
    CHECK_SINGLE_POINTER((pList)->pHead, retVal);        \
    CHECK_SINGLE_POINTER((pList)->pHead->pNext, retVal); \
        }while(0)

//双向循环链表遍历宏
//pList: 链表指针；pLoopNode: 链表结点，用作循环计数器；
//pTmpNode: 链表结点，用作删除pLoopNode时临时保存pLoopNode->pNext
//某些情况下采用遍历宏代替OmciLocateListNode或OmciTraverseListNode函数可提高执行效率。
//如外部数据和结点数据需按共同的规则转换时，采用遍历宏可使外部数据不必重复转换。
#define LIST_ITER_LOOP(pList, pLoopNode) \
  for(pLoopNode = (pList)->pHead->pNext; \
      pLoopNode != (pList)->pHead;       \
      pLoopNode = pLoopNode->pNext)

#define LIST_ITER_LOOP_SAFE(pList, pLoopNode, pTmpNode)               \
  for(pLoopNode = (pList)->pHead->pNext, pTmpNode = pLoopNode->pNext; \
      pLoopNode != (pList)->pHead;                                    \
      pLoopNode = pTmpNode, pTmpNode = pLoopNode->pNext)

//安全释放内存
#define SAFE_FREE(pointer) do{ \
    if(pointer != NULL){         \
        free(pointer);           \
        pointer = NULL;          \
    }while(0);

//链表结点头插
#define LIST_INSERT_HEAD(headNode, insertNode)      \
    do{                                             \
        insertNode->pNext = headNode;                \
        headNode = insertNode;                        \
    }while(0)

//链表结点尾插
#define LIST_INSERT_TAIL(headNode, insertNode, tempNode)          \
    do{                                                           \
        tempNode = headNode;                                      \
        insertNode->pNext = NULL;                                 \
        if(NULL == tempNode)                                      \
        {                                                         \
            headNode = insertNode;                                \
        }                                                         \
        else                                                      \
        {                                                          \
            while (NULL != tempNode->pNext)                       \
            {                                                     \
                tempNode = tempNode->pNext;                       \
            }                                                     \
            tempNode->pNext = insertNode;                         \
        }                                                          \
    }while(0)

//链表结点删除
#define LIST_DELETE_ALL(headNode, tempNode)                       \
    do{                                                           \
        while (NULL != headNode)                                  \
        {                                                         \
            tempNode = headNode;                                  \
            headNode = headNode->pNext;                           \
            tempNode->pNext = NULL;                               \
            DDS_STATIC_FREE(tempNode);                            \
        }                                                         \
    }while(0)

//链表结点删除
#define LIST_DELETE_GUID(headNode, tempNode)                      \
    do{                                                           \
        while (NULL != headNode)                                  \
        {                                                         \
            tempNode = headNode;                                  \
            headNode = headNode->pNext;                           \
            tempNode->pNext = NULL;                               \
            DDS_STATIC_FREE(tempNode);                            \
        }                                                         \
    }while(0)

//带表头、表尾单链表尾插
#define DOUBLE_INSERT_TAIL(headNode, tailNode, insertNode)  \
    do{                                                 \
        if(NULL == headNode || NULL == tailNode)        \
        {                                               \
            headNode = insertNode;                      \
            tailNode = insertNode;                      \
        }                                               \
        else                                            \
        {                                               \
            tailNode->pNext = insertNode;               \
			tailNode = insertNode;                      \
	    }                                               \
        insertNode->pNext =NULL;                        \
    }while(0)                                           

#endif

