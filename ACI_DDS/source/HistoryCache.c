#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: InitHistoryData
-   功能描述: 初始化历史数据
-   输    入: 缓存器
-   返    回:
-   全局变量:
-   注    释: 初始化历史数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitHistoryData(HistoryData* pstHistoryData)
{
    if (NULL != pstHistoryData)
    {
        pstHistoryData->bCancel = FALSE;
        pstHistoryData->data = NULL;
        pstHistoryData->seqNum = SEQUENCENUMBER_ZERO;
        pstHistoryData->stGuid = GUID_UNKNOWN;
        pstHistoryData->pNext = NULL;
        pstHistoryData->uiDataLen = 0;

        pstHistoryData->pstFragData = NULL;
        pstHistoryData->uiFragTotal = 0;
        pstHistoryData->uiFragNum = 0;
        pstHistoryData->uiFragLen = 0;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: UninitHistoryData
-   功能描述: 析构历史数据
-   输    入: 缓存器
-   返    回:
-   全局变量:
-   注    释: 析构历史缓存
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UninitHistoryData(HistoryData* pstHistoryData)
{
    if (NULL != pstHistoryData)
    {
        pstHistoryData->bCancel = FALSE;

        pstHistoryData->seqNum = SEQUENCENUMBER_ZERO;
        pstHistoryData->stGuid = GUID_UNKNOWN;
        pstHistoryData->pNext = NULL;

		if (NULL != pstHistoryData->data)
        {
            DDS_STATIC_FREE(pstHistoryData->data);
        }

        pstHistoryData->uiDataLen = 0;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: InitHistoryCache
-   功能描述: 初始化历史缓存
-   输    入: 缓存器
-   返    回:
-   全局变量:
-   注    释: 初始化历史缓存数据
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitHistoryCache(HistoryCache* pstHistoryCache)
{
    if (NULL != pstHistoryCache)
    {
        pstHistoryCache->pstHead = NULL;
        pstHistoryCache->pstTail = NULL;
        pstHistoryCache->uiHistoryDataNum = 0;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: InitHistoryCache
-   功能描述: 析构历史缓存
-   输    入: 缓存器
-   返    回:
-   全局变量:
-   注    释: 析构历史缓存
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID UninitHistoryCache(HistoryCache* pstHistoryCache)
{
    if (NULL != pstHistoryCache)
    {
        HistoryData* pstHistoryData = pstHistoryCache->pstHead;
        while (NULL != pstHistoryData)
        {
            pstHistoryCache->pstHead = pstHistoryData->pNext;
            UninitHistoryData(pstHistoryData);
            DDS_STATIC_FREE(pstHistoryData);

            pstHistoryData = pstHistoryCache->pstHead;
        }
        DDS_STATIC_FREE(pstHistoryCache->pstHead);
        pstHistoryCache->uiHistoryDataNum = 0;
        pstHistoryCache->pstTail = NULL;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: InsertHistoryCacheTail
-   功能描述: 缓存数据尾插缓存器
-   输    入: 缓存器、缓存数据结构
-   返    回:
-   全局变量:
-   注    释: 数据尾插缓存器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InsertHistoryCacheTail(HistoryCache* pstHistoryCache, HistoryData* pstHistoryData)
{
    //printf("%d\n", pstHistoryCache->uiHistoryDataNum);
    if (NULL != pstHistoryCache->pstTail && pstHistoryCache->uiHistoryDataNum != 0)
    {
        pstHistoryCache->pstTail->pNext = pstHistoryData;
        pstHistoryCache->pstTail = pstHistoryData;
        pstHistoryData->pNext = NULL;
    }
    else
    {
        pstHistoryCache->pstHead = pstHistoryData;
        pstHistoryCache->pstTail = pstHistoryData;
        pstHistoryData->pNext = NULL;
    }

    pstHistoryCache->uiHistoryDataNum++;
}

/*---------------------------------------------------------------------------------
-   函 数 名: InsertHistoryCacheOrder
-   功能描述: 按照seqNum顺序大小插入缓存器
-   输    入: 缓存器、缓存数据结构
-   返    回:
-   全局变量:
-   注    释: 按照seqNum顺序大小先从尾部比较插入,再从头部遍历插入
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InsertHistoryCacheOrder(HistoryCache* pstHistoryCache, HistoryData* pstNewHistoryData)
{
    HistoryData* pstTempHistoryData = NULL;
    HistoryData* pstHistoryData  = pstHistoryCache->pstHead;
    while (NULL != pstHistoryData)
    {
        /* 已存在过滤 */
        if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, pstNewHistoryData->seqNum))
        {
            return FALSE;
        }

        /* 优先考虑尾部插入，提高插入效率 */
        if (ISLESSTHEN_SEQUENCENUMBER(pstHistoryCache->pstTail->seqNum, pstNewHistoryData->seqNum))
        {
            DOUBLE_INSERT_TAIL(pstHistoryCache->pstHead, pstHistoryCache->pstTail, pstNewHistoryData);
            pstHistoryCache->uiHistoryDataNum++;
            //printf("reader historyDataNum: %d\n", pstHistoryCache->uiHistoryDataNum);
            return TRUE;
        }

        if (ISLESSTHEN_SEQUENCENUMBER(pstNewHistoryData->seqNum, pstHistoryData->seqNum))
        {
            if (NULL == pstTempHistoryData)
            {
                pstNewHistoryData->pNext = pstHistoryCache->pstHead;
                pstHistoryCache->pstHead = pstNewHistoryData;
                break;
            }
            else
            {
                pstNewHistoryData->pNext = pstTempHistoryData->pNext;
                pstTempHistoryData->pNext = pstNewHistoryData;
                break;
            }
        }
        else 
        {
            pstTempHistoryData = pstHistoryData;
            pstHistoryData = pstHistoryData->pNext;
        }
    }

    if (NULL == pstHistoryData)
    {
        DOUBLE_INSERT_TAIL(pstHistoryCache->pstHead, pstHistoryCache->pstTail, pstNewHistoryData);
    }

    pstHistoryCache->uiHistoryDataNum++;

    return TRUE;
}
