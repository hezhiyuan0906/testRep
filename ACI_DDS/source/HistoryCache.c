#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: InitHistoryData
-   ��������: ��ʼ����ʷ����
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ʼ����ʷ����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: UninitHistoryData
-   ��������: ������ʷ����
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ʷ����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: InitHistoryCache
-   ��������: ��ʼ����ʷ����
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ʼ����ʷ��������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: InitHistoryCache
-   ��������: ������ʷ����
-   ��    ��: ������
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ������ʷ����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: InsertHistoryCacheTail
-   ��������: ��������β�建����
-   ��    ��: ���������������ݽṹ
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����β�建����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: InsertHistoryCacheOrder
-   ��������: ����seqNum˳���С���뻺����
-   ��    ��: ���������������ݽṹ
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ����seqNum˳���С�ȴ�β���Ƚϲ���,�ٴ�ͷ����������
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\12       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL InsertHistoryCacheOrder(HistoryCache* pstHistoryCache, HistoryData* pstNewHistoryData)
{
    HistoryData* pstTempHistoryData = NULL;
    HistoryData* pstHistoryData  = pstHistoryCache->pstHead;
    while (NULL != pstHistoryData)
    {
        /* �Ѵ��ڹ��� */
        if (ISEQUAL_SEQUENCENUMBER(pstHistoryData->seqNum, pstNewHistoryData->seqNum))
        {
            return FALSE;
        }

        /* ���ȿ���β�����룬��߲���Ч�� */
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
