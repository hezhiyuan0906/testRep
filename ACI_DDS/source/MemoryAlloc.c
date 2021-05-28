#include "../Include/GlobalDefine.h"

static DDS_MUTEX hMutex;

#ifdef USE_STATIC_MALLOC

static int malloc_num = 0;

/* ¾²Ì¬ÄÚ´æÉêÇë */
static MEMBLOCK_16B    s_memBlock_16B[MEMBLOCK_NUM_SIZE_16B];
static MEMBLOCK_32B    s_memBlock_32B[MEMBLOCK_NUM_SIZE_32B];
static MEMBLOCK_64B    s_memBlock_64B[MEMBLOCK_NUM_SIZE_64B];
static MEMBLOCK_128B   s_memBlock_128B[MEMBLOCK_NUM_SIZE_128B];
static MEMBLOCK_256B   s_memBlock_256B[MEMBLOCK_NUM_SIZE_256B];
static MEMBLOCK_512B   s_memBlock_512B[MEMBLOCK_NUM_SIZE_512B];
static MEMBLOCK_1K     s_memBlock_1K[MEMBLOCK_NUM_SIZE_1K];
static MEMBLOCK_2K     s_memBlock_2K[MEMBLOCK_NUM_SIZE_2K];
static MEMBLOCK_4K     s_memBlock_4K[MEMBLOCK_NUM_SIZE_4K];
static MEMBLOCK_8K     s_memBlock_8K[MEMBLOCK_NUM_SIZE_8K];
static MEMBLOCK_16K    s_memBlock_16K[MEMBLOCK_NUM_SIZE_16K];
static MEMBLOCK_32K    s_memBlock_32K[MEMBLOCK_NUM_SIZE_32K];
static MEMBLOCK_64K    s_memBlock_64K[MEMBLOCK_NUM_SIZE_64K];

/* 静态指针初始化 */
static MEMBLOCK_16B*   s_pMemBlock_16B   = NULL;
static MEMBLOCK_32B*   s_pMemBlock_32B   = NULL;
static MEMBLOCK_64B*   s_pMemBlock_64B   = NULL;
static MEMBLOCK_128B*  s_pMemBlock_128B  = NULL;
static MEMBLOCK_256B*  s_pMemBlock_256B  = NULL;
static MEMBLOCK_512B*  s_pMemBlock_512B  = NULL;
static MEMBLOCK_1K*    s_pMemBlock_1K    = NULL;
static MEMBLOCK_2K*    s_pMemBlock_2K    = NULL;
static MEMBLOCK_4K*    s_pMemBlock_4K    = NULL;
static MEMBLOCK_8K*    s_pMemBlock_8K    = NULL;
static MEMBLOCK_16K*   s_pMemBlock_16K   = NULL;
static MEMBLOCK_32K*   s_pMemBlock_32K   = NULL;
static MEMBLOCK_64K*   s_pMemBlock_64K   = NULL;

/*---------------------------------------------------------------------------------
-   函 数 名: InitMemBlock
-   功能描述: 静态内存初始话
-   输    入: 
-   返    回:
-   全局变量: 各内存块首地址
-   注    释: 全局静态内存初始话
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitMemBlock()
{
    DDS_MUTEX_INIT(hMutex);

    int i = 0;

	for (i = 0; i < MEMBLOCK_NUM_SIZE_16B; i++)
	{
		s_memBlock_16B[i].uiSeqNum = i + 1;
		s_memBlock_16B[i].memType = MEMBLOCK_SIZE_16B;
		s_memBlock_16B[i].eumUse = UNSE;
		s_memBlock_16B[i].pNext = s_pMemBlock_16B;
		s_pMemBlock_16B = &s_memBlock_16B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_32B; i++)
	{
		s_memBlock_32B[i].uiSeqNum = i + 1;
		s_memBlock_32B[i].memType = MEMBLOCK_SIZE_32B;
		s_memBlock_32B[i].eumUse = UNSE;
		s_memBlock_32B[i].pNext = s_pMemBlock_32B;
		s_pMemBlock_32B = &s_memBlock_32B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_64B; i++)
	{
		s_memBlock_64B[i].uiSeqNum = i + 1;
		s_memBlock_64B[i].memType = MEMBLOCK_SIZE_64B;
		s_memBlock_64B[i].eumUse = UNSE;
		s_memBlock_64B[i].pNext = s_pMemBlock_64B;
		s_pMemBlock_64B = &s_memBlock_64B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_128B; i++)
	{
		s_memBlock_128B[i].uiSeqNum = i + 1;
		s_memBlock_128B[i].memType = MEMBLOCK_SIZE_128B;
		s_memBlock_128B[i].eumUse = UNSE;
		s_memBlock_128B[i].pNext = s_pMemBlock_128B;
		s_pMemBlock_128B = &s_memBlock_128B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_256B; i++)
	{
		s_memBlock_256B[i].uiSeqNum = i + 1;
		s_memBlock_256B[i].memType = MEMBLOCK_SIZE_256B;
		s_memBlock_256B[i].eumUse = UNSE;
		s_memBlock_256B[i].pNext = s_pMemBlock_256B;
		s_pMemBlock_256B = &s_memBlock_256B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_512B; i++)
	{
		s_memBlock_512B[i].uiSeqNum = i + 1;
		s_memBlock_512B[i].memType = MEMBLOCK_SIZE_512B;
		s_memBlock_512B[i].eumUse = UNSE;
		s_memBlock_512B[i].pNext = s_pMemBlock_512B;
		s_pMemBlock_512B = &s_memBlock_512B[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_1K; i++)
	{
		s_memBlock_1K[i].uiSeqNum = i + 1;
		s_memBlock_1K[i].memType = MEMBLOCK_SIZE_1K;
		s_memBlock_1K[i].eumUse = UNSE;
		s_memBlock_1K[i].pNext = s_pMemBlock_1K;
		s_pMemBlock_1K = &s_memBlock_1K[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_2K; i++)
	{
		s_memBlock_2K[i].uiSeqNum = i + 1;
		s_memBlock_2K[i].memType = MEMBLOCK_SIZE_2K;
		s_memBlock_2K[i].eumUse = UNSE;
		s_memBlock_2K[i].pNext = s_pMemBlock_2K;
		s_pMemBlock_2K = &s_memBlock_2K[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_4K; i++)
	{
		s_memBlock_4K[i].uiSeqNum = i + 1;
		s_memBlock_4K[i].memType = MEMBLOCK_SIZE_4K;
		s_memBlock_4K[i].eumUse = UNSE;
		s_memBlock_4K[i].pNext = s_pMemBlock_4K;
		s_pMemBlock_4K = &s_memBlock_4K[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_8K; i++)
	{
		s_memBlock_8K[i].uiSeqNum = i + 1;
		s_memBlock_8K[i].memType = MEMBLOCK_SIZE_8K;
		s_memBlock_8K[i].eumUse = UNSE;
		s_memBlock_8K[i].pNext = s_pMemBlock_8K;
		s_pMemBlock_8K = &s_memBlock_8K[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_16K; i++)
	{
		s_memBlock_16K[i].uiSeqNum = i + 1;
		s_memBlock_16K[i].memType = MEMBLOCK_SIZE_16K;
		s_memBlock_16K[i].eumUse = UNSE;
		s_memBlock_16K[i].pNext = s_pMemBlock_16K;
		s_pMemBlock_16K = &s_memBlock_16K[i];
	}

	for (i = 0; i < MEMBLOCK_NUM_SIZE_32K; i++)
	{
		s_memBlock_32K[i].uiSeqNum = i + 1;
		s_memBlock_32K[i].memType = MEMBLOCK_SIZE_32K;
		s_memBlock_32K[i].eumUse = UNSE;
		s_memBlock_32K[i].pNext = s_pMemBlock_32K;
		s_pMemBlock_32K = &s_memBlock_32K[i];
	}

    for (i = 0; i < MEMBLOCK_NUM_SIZE_64K; i++)
    {
        s_memBlock_64K[i].uiSeqNum = i + 1;
        s_memBlock_64K[i].memType = MEMBLOCK_SIZE_64K;
        s_memBlock_64K[i].eumUse = UNSE;
        s_memBlock_64K[i].pNext = s_pMemBlock_64K;
        s_pMemBlock_64K = &s_memBlock_64K[i];
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_STATIC_MALLOC
-   功能描述: 静态内存分配
-   输    入:
-   返    回:
-   全局变量: 各内存块首地址
-   注    释: 静态内存分配，最大可能得分配内存, 关键数据使用
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID* DDS_STATIC_MALLOC(UINT32 uiLen)
{
    DDS_MUTEX_LOCK(hMutex);

    if (uiLen <= MEMBLOCK_SIZE_16B)
    {
        if (NULL != s_pMemBlock_16B)
        {
            MEMBLOCK_16B* pMemBlock_16B = s_pMemBlock_16B;
            s_pMemBlock_16B = pMemBlock_16B->pNext;
            pMemBlock_16B->eumUse = USED;
            pMemBlock_16B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_16B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_32B)
    {
        if (NULL != s_pMemBlock_32B)
        {
            MEMBLOCK_32B* pMemBlock_32B = s_pMemBlock_32B;
            s_pMemBlock_32B = pMemBlock_32B->pNext;
            pMemBlock_32B->eumUse = USED;
            pMemBlock_32B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_32B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_64B)
    {
        if (NULL != s_pMemBlock_64B)
        {
            MEMBLOCK_64B* pMemBlock_64B = s_pMemBlock_64B;
            s_pMemBlock_64B = pMemBlock_64B->pNext;
            pMemBlock_64B->eumUse = USED;
            pMemBlock_64B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_64B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_128B)
    {
        if (NULL != s_pMemBlock_128B)
        {
            MEMBLOCK_128B* pMemBlock_128B = s_pMemBlock_128B;
            s_pMemBlock_128B = pMemBlock_128B->pNext;
            pMemBlock_128B->eumUse = USED;
            pMemBlock_128B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_128B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_256B)
    {
        if (NULL != s_pMemBlock_256B)
        {
            MEMBLOCK_256B* pMemBlock_256B = s_pMemBlock_256B;
            s_pMemBlock_256B = pMemBlock_256B->pNext;
            pMemBlock_256B->eumUse = USED;
            pMemBlock_256B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_256B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_512B)
    {
        if (NULL != s_pMemBlock_512B)
        {
            MEMBLOCK_512B* pMemBlock_512B = s_pMemBlock_512B;
            s_pMemBlock_512B = pMemBlock_512B->pNext;
            pMemBlock_512B->eumUse = USED;
            pMemBlock_512B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_512B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_1K)
    {
        if (NULL != s_pMemBlock_1K)
        {
            MEMBLOCK_1K* pMemBlock_1K = s_pMemBlock_1K;
            s_pMemBlock_1K = pMemBlock_1K->pNext;
            pMemBlock_1K->eumUse = USED;
            pMemBlock_1K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_1K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_2K)
    {
        if (NULL != s_pMemBlock_2K)
        {
            MEMBLOCK_2K* pMemBlock_2K = s_pMemBlock_2K;
            s_pMemBlock_2K = pMemBlock_2K->pNext;
            pMemBlock_2K->eumUse = USED;
            pMemBlock_2K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_2K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_4K)
    {
        if (NULL != s_pMemBlock_4K)
        {
            MEMBLOCK_4K* pMemBlock_4K = s_pMemBlock_4K;
            s_pMemBlock_4K = pMemBlock_4K->pNext;
            pMemBlock_4K->eumUse = USED;
            pMemBlock_4K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_4K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_8K)
    {
        if (NULL != s_pMemBlock_8K)
        {
            MEMBLOCK_8K* pMemBlock_8K = s_pMemBlock_8K;
            s_pMemBlock_8K = pMemBlock_8K->pNext;
            pMemBlock_8K->eumUse = USED;
            pMemBlock_8K->pNext = pMemBlock_8K;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_8K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_16K)
    {
        if (NULL != s_pMemBlock_16K)
        {
            MEMBLOCK_16K* pMemBlock_16K = s_pMemBlock_16K;
            s_pMemBlock_16K = pMemBlock_16K->pNext;
            pMemBlock_16K->eumUse = USED;
            pMemBlock_16K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_16K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_32K)
    {
        if (NULL != s_pMemBlock_32K)
        {
            MEMBLOCK_32K* pMemBlock_32K = s_pMemBlock_32K;
            s_pMemBlock_32K = pMemBlock_32K->pNext;
            pMemBlock_32K->eumUse = USED;
            pMemBlock_32K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_32K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_64K)
    {
        if (NULL != s_pMemBlock_64K)
        {
            MEMBLOCK_64K* pMemBlock_64K = s_pMemBlock_64K;
            s_pMemBlock_64K = pMemBlock_64K->pNext;
            pMemBlock_64K->eumUse = USED;
            pMemBlock_64K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_64K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    DDS_MUTEX_UNLOCK(hMutex);

    return NULL;
}


/*---------------------------------------------------------------------------------
-   函 数 名: DDS_STATIC_MALLOC_T
-   功能描述: 静态内存分配
-   输    入:
-   返    回:
-   全局变量: 各内存块首地址
-   注    释: 静态内存分配，保留部分内存块
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID* DDS_STATIC_MALLOC_T(UINT32 uiLen)
{
    DDS_MUTEX_LOCK(hMutex);

    if (uiLen <= MEMBLOCK_SIZE_16B)
    {
        if (NULL != s_pMemBlock_16B && s_pMemBlock_16B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_16B* pMemBlock_16B = s_pMemBlock_16B;
            s_pMemBlock_16B = pMemBlock_16B->pNext;
            pMemBlock_16B->eumUse = USED;
            pMemBlock_16B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_16B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_32B)
    {
        if (NULL != s_pMemBlock_32B && s_pMemBlock_32B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_32B* pMemBlock_32B = s_pMemBlock_32B;
            s_pMemBlock_32B = pMemBlock_32B->pNext;
            pMemBlock_32B->eumUse = USED;
            pMemBlock_32B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_32B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_64B)
    {
        if (NULL != s_pMemBlock_64B && s_pMemBlock_64B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_64B* pMemBlock_64B = s_pMemBlock_64B;
            s_pMemBlock_64B = pMemBlock_64B->pNext;
            pMemBlock_64B->eumUse = USED;
            pMemBlock_64B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_64B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_128B)
    {
        if (NULL != s_pMemBlock_128B && s_pMemBlock_128B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_128B* pMemBlock_128B = s_pMemBlock_128B;
            s_pMemBlock_128B = pMemBlock_128B->pNext;
            pMemBlock_128B->eumUse = USED;
            pMemBlock_128B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_128B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_256B)
    {
        if (NULL != s_pMemBlock_256B && s_pMemBlock_256B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_256B* pMemBlock_256B = s_pMemBlock_256B;
            s_pMemBlock_256B = pMemBlock_256B->pNext;
            pMemBlock_256B->eumUse = USED;
            pMemBlock_256B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_256B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_512B)
    {
        if (NULL != s_pMemBlock_512B && s_pMemBlock_512B->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_512B* pMemBlock_512B = s_pMemBlock_512B;
            s_pMemBlock_512B = pMemBlock_512B->pNext;
            pMemBlock_512B->eumUse = USED;
            pMemBlock_512B->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_512B + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_1K)
    {
        if (NULL != s_pMemBlock_1K && s_pMemBlock_1K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_1K* pMemBlock_1K = s_pMemBlock_1K;
            s_pMemBlock_1K = pMemBlock_1K->pNext;
            pMemBlock_1K->eumUse = USED;
            pMemBlock_1K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_1K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_2K)
    {
        if (NULL != s_pMemBlock_2K && s_pMemBlock_2K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_2K* pMemBlock_2K = s_pMemBlock_2K;
            s_pMemBlock_2K = pMemBlock_2K->pNext;
            pMemBlock_2K->eumUse = USED;
            pMemBlock_2K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_2K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_4K)
    {
        if (NULL != s_pMemBlock_4K && s_pMemBlock_4K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_4K* pMemBlock_4K = s_pMemBlock_4K;
            s_pMemBlock_4K = pMemBlock_4K->pNext;
            pMemBlock_4K->eumUse = USED;
            pMemBlock_4K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_4K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_8K)
    {
        if (NULL != s_pMemBlock_8K && s_pMemBlock_8K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_8K* pMemBlock_8K = s_pMemBlock_8K;
            s_pMemBlock_8K = pMemBlock_8K->pNext;
            pMemBlock_8K->eumUse = USED;
            pMemBlock_8K->pNext = pMemBlock_8K;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_8K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_16K)
    {
        if (NULL != s_pMemBlock_16K && s_pMemBlock_16K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_16K* pMemBlock_16K = s_pMemBlock_16K;
            s_pMemBlock_16K = pMemBlock_16K->pNext;
            pMemBlock_16K->eumUse = USED;
            pMemBlock_16K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_16K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_32K)
    {
        if (NULL != s_pMemBlock_32K && s_pMemBlock_32K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_32K* pMemBlock_32K = s_pMemBlock_32K;
            s_pMemBlock_32K = pMemBlock_32K->pNext;
            pMemBlock_32K->eumUse = USED;
            pMemBlock_32K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_32K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    if (uiLen <= MEMBLOCK_SIZE_64K)
    {
        if (NULL != s_pMemBlock_64K && s_pMemBlock_64K->uiSeqNum > RESERVE_NUM)
        {
            MEMBLOCK_64K* pMemBlock_64K = s_pMemBlock_64K;
            s_pMemBlock_64K = pMemBlock_64K->pNext;
            pMemBlock_64K->eumUse = USED;
            pMemBlock_64K->pNext = NULL;
            DDS_MUTEX_UNLOCK(hMutex);
            return (VOID*)((CHAR*)pMemBlock_64K + sizeof(VOID*) + sizeof(UINT32) + sizeof(MEMUSE) + sizeof(MemBlockType));
        }
    }

    DDS_MUTEX_UNLOCK(hMutex);

    return NULL;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_STATIC_FREE
-   功能描述: 静态内存释放
-   输    入:
-   返    回:
-   全局变量: 各内存块首地址
-   注    释: 静态内存释放
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_STATIC_FREE(VOID* pMemBlock)
{
    if (NULL == pMemBlock)
    {
        return;
    }

    DDS_MUTEX_LOCK(hMutex);

    MemBlockType* pMemBlockType = (MemBlockType*)((CHAR*)pMemBlock - sizeof(MemBlockType) - sizeof(MEMUSE));

    MEMUSE* pMemUse = (MEMUSE*)((CHAR*)pMemBlock - sizeof(MEMUSE));

    if (UNSE == *pMemUse)
    {
        DDS_MUTEX_UNLOCK(hMutex);
        return;
    }

    switch (*pMemBlockType)
    {
        case MEMBLOCK_SIZE_16B:
        {
            MEMBLOCK_16B* pMemBlock_16B = (MEMBLOCK_16B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_16B->eumUse = UNSE;
            if (NULL == s_pMemBlock_16B)
            {
                pMemBlock_16B->uiSeqNum = 1;
                s_pMemBlock_16B = pMemBlock_16B;
            }
            else
            {
                pMemBlock_16B->uiSeqNum = s_pMemBlock_16B->uiSeqNum + 1;
                pMemBlock_16B->pNext = s_pMemBlock_16B;
                s_pMemBlock_16B = pMemBlock_16B;
            }
            break;
        }

        case MEMBLOCK_SIZE_32B:
        {
            MEMBLOCK_32B* pMemBlock_32B = (MEMBLOCK_32B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_32B->eumUse = UNSE;
            if (NULL == s_pMemBlock_32B)
            {
                pMemBlock_32B->uiSeqNum = 1;
                s_pMemBlock_32B = pMemBlock_32B;
            }
            else
            {
                pMemBlock_32B->uiSeqNum = s_pMemBlock_32B->uiSeqNum + 1;
                pMemBlock_32B->pNext = s_pMemBlock_32B;
                s_pMemBlock_32B = pMemBlock_32B;
            }
            break;
        }

        case MEMBLOCK_SIZE_64B:
        {
            MEMBLOCK_64B* pMemBlock_64B = (MEMBLOCK_64B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_64B->eumUse = UNSE;
            if (NULL == s_pMemBlock_64B)
            {
                pMemBlock_64B->uiSeqNum = 1;
                s_pMemBlock_64B = pMemBlock_64B;
            }
            else
            {
                pMemBlock_64B->uiSeqNum = s_pMemBlock_64B->uiSeqNum + 1;
                pMemBlock_64B->pNext = s_pMemBlock_64B;
                s_pMemBlock_64B = pMemBlock_64B;
            }
            break;
        }

        case MEMBLOCK_SIZE_128B:
        {
            MEMBLOCK_128B* pMemBlock_128B = (MEMBLOCK_128B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_128B->eumUse = UNSE;
            if (NULL == s_pMemBlock_128B)
            {
                pMemBlock_128B->uiSeqNum = 1;
                s_pMemBlock_128B = pMemBlock_128B;
            }
            else
            {
                pMemBlock_128B->uiSeqNum = s_pMemBlock_128B->uiSeqNum + 1;
                pMemBlock_128B->pNext = s_pMemBlock_128B;
                s_pMemBlock_128B = pMemBlock_128B;
            }
            break;
        }

        case MEMBLOCK_SIZE_256B:
        {
            MEMBLOCK_256B* pMemBlock_256B = (MEMBLOCK_256B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_256B->eumUse = UNSE;
            if (NULL == s_pMemBlock_256B)
            {
                pMemBlock_256B->uiSeqNum = 1;
                s_pMemBlock_256B = pMemBlock_256B;
            }
            else
            {
                pMemBlock_256B->uiSeqNum = s_pMemBlock_256B->uiSeqNum + 1;
                pMemBlock_256B->pNext = s_pMemBlock_256B;
                s_pMemBlock_256B = pMemBlock_256B;
            }
            break;
        }
        case MEMBLOCK_SIZE_512B:
        {
            MEMBLOCK_512B* pMemBlock_512B = (MEMBLOCK_512B*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_512B->eumUse = UNSE;
            if (NULL == s_pMemBlock_512B)
            {
                pMemBlock_512B->uiSeqNum = 1;
                s_pMemBlock_512B = pMemBlock_512B;
            }
            else
            {
                pMemBlock_512B->uiSeqNum = s_pMemBlock_512B->uiSeqNum + 1;
                pMemBlock_512B->pNext = s_pMemBlock_512B;
                s_pMemBlock_512B = pMemBlock_512B;
            }
            break;
        }

        case MEMBLOCK_SIZE_1K:
        {
            MEMBLOCK_1K* pMemBlock_1K = (MEMBLOCK_1K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_1K->eumUse = UNSE;
            if (NULL == s_pMemBlock_1K)
            {
                pMemBlock_1K->uiSeqNum = 1;
                s_pMemBlock_1K = pMemBlock_1K;
            }
            else
            {
                pMemBlock_1K->uiSeqNum = s_pMemBlock_1K->uiSeqNum + 1;
                pMemBlock_1K->pNext = s_pMemBlock_1K;
                s_pMemBlock_1K = pMemBlock_1K;
            }
            break;
        }

        case MEMBLOCK_SIZE_2K:
        {
            MEMBLOCK_2K* pMemBlock_2K = (MEMBLOCK_2K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_2K->eumUse = UNSE;
            if (NULL == s_pMemBlock_2K)
            {
                pMemBlock_2K->uiSeqNum = 1;
                s_pMemBlock_2K = pMemBlock_2K;
            }
            else
            {
                pMemBlock_2K->uiSeqNum = s_pMemBlock_2K->uiSeqNum + 1;
                pMemBlock_2K->pNext = s_pMemBlock_2K;
                s_pMemBlock_2K = pMemBlock_2K;
            }
            break;
        }

        case MEMBLOCK_SIZE_4K:
        {
            MEMBLOCK_4K* pMemBlock_4K = (MEMBLOCK_4K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_4K->eumUse = UNSE;
            if (NULL == s_pMemBlock_4K)
            {
                pMemBlock_4K->uiSeqNum = 1;
                s_pMemBlock_4K = pMemBlock_4K;
            }
            else
            {
                pMemBlock_4K->uiSeqNum = s_pMemBlock_4K->uiSeqNum + 1;
                pMemBlock_4K->pNext = s_pMemBlock_4K;
                s_pMemBlock_4K = pMemBlock_4K;
            }
            break;
        }

        case MEMBLOCK_SIZE_8K:
        {
            MEMBLOCK_8K* pMemBlock_8K = (MEMBLOCK_8K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_8K->eumUse = UNSE;
            if (NULL == s_pMemBlock_8K)
            {
                pMemBlock_8K->uiSeqNum = 1;
                s_pMemBlock_8K = pMemBlock_8K;
            }
            else
            {
                pMemBlock_8K->uiSeqNum = s_pMemBlock_8K->uiSeqNum + 1;
                pMemBlock_8K->pNext = s_pMemBlock_8K;
                s_pMemBlock_8K = pMemBlock_8K;
            }
            break;
        }

        case MEMBLOCK_SIZE_16K:
        {
            MEMBLOCK_16K* pMemBlock_16K = (MEMBLOCK_16K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_16K->eumUse = UNSE;
            if (NULL == s_pMemBlock_16K)
            {
                pMemBlock_16K->uiSeqNum = 1;
                s_pMemBlock_16K = pMemBlock_16K;
            }
            else
            {
                pMemBlock_16K->uiSeqNum = s_pMemBlock_16K->uiSeqNum + 1;
                pMemBlock_16K->pNext = s_pMemBlock_16K;
                s_pMemBlock_16K = pMemBlock_16K;
            }
            break;
        }

        case MEMBLOCK_SIZE_32K:
        {
            MEMBLOCK_32K* pMemBlock_32K = (MEMBLOCK_32K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_32K->eumUse = UNSE;
            if (NULL == s_pMemBlock_32K)
            {
                pMemBlock_32K->uiSeqNum = 1;
                s_pMemBlock_32K = pMemBlock_32K;
            }
            else
            {
                pMemBlock_32K->uiSeqNum = s_pMemBlock_32K->uiSeqNum + 1;
                pMemBlock_32K->pNext = s_pMemBlock_32K;
                s_pMemBlock_32K = pMemBlock_32K;
            }
            break;
        }

        case MEMBLOCK_SIZE_64K:
        {
            MEMBLOCK_64K* pMemBlock_64K = (MEMBLOCK_64K*)((CHAR*)pMemBlock - sizeof(VOID*) - sizeof(UINT32) - sizeof(MEMUSE) - sizeof(MemBlockType));
            pMemBlock_64K->eumUse = UNSE;
            if (NULL == s_pMemBlock_64K)
            {
                pMemBlock_64K->uiSeqNum = 1;
                s_pMemBlock_64K = pMemBlock_64K;
            }
            else
            {
                pMemBlock_64K->uiSeqNum = s_pMemBlock_64K->uiSeqNum + 1;
                pMemBlock_64K->pNext = s_pMemBlock_64K;
                s_pMemBlock_64K = pMemBlock_64K;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    DDS_MUTEX_UNLOCK(hMutex);

}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_MemBlockInit
-   功能描述: 静态内存初始话
-   输    入:
-   返    回:
-   全局变量: 各内存块首地址
-   注    释: 全局静态内存初始话
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
=======================================================
-   2019\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID DDS_MemoryUsage()
{
    printf("**********************************Memory Usage***************************************\n");

    if (s_pMemBlock_16B)
    {
        printf("*    16B    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_16B, MEMBLOCK_NUM_SIZE_16B - s_pMemBlock_16B->uiSeqNum, s_pMemBlock_16B->uiSeqNum);
    }
    else
    {
        printf("*    16B    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_16B, 0, MEMBLOCK_NUM_SIZE_16B);
    }

    if (s_pMemBlock_32B)
    {
        printf("*    32B    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_32B, MEMBLOCK_NUM_SIZE_32B - s_pMemBlock_32B->uiSeqNum, s_pMemBlock_32B->uiSeqNum);
    }
    else
    {
        printf("*    32B    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_32B, 0, MEMBLOCK_NUM_SIZE_32B);
    }

    if (s_pMemBlock_64B)
    {
        printf("*    64B    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_64B, MEMBLOCK_NUM_SIZE_64B - s_pMemBlock_64B->uiSeqNum, s_pMemBlock_64B->uiSeqNum);
    }
    else
    {
        printf("*    64B    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_64B, 0, MEMBLOCK_NUM_SIZE_64B);
    }

    if (s_pMemBlock_128B)
    {
        printf("*    128B   |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_128B, MEMBLOCK_NUM_SIZE_128B - s_pMemBlock_128B->uiSeqNum, s_pMemBlock_128B->uiSeqNum);
    }
    else
    {
        printf("*    128B   |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_128B, 0, MEMBLOCK_NUM_SIZE_128B);
    }

    if (s_pMemBlock_256B)
    {
        printf("*    256B   |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_256B, MEMBLOCK_NUM_SIZE_256B - s_pMemBlock_256B->uiSeqNum, s_pMemBlock_256B->uiSeqNum);
    }
    else
    {
        printf("*    256B   |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_256B, 0, MEMBLOCK_NUM_SIZE_256B);
    }

    if (s_pMemBlock_512B)
    {
        printf("*    512B   |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_512B, MEMBLOCK_NUM_SIZE_512B - s_pMemBlock_512B->uiSeqNum, s_pMemBlock_512B->uiSeqNum);
    }
    else
    {
        printf("*    512B   |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_512B, 0, MEMBLOCK_NUM_SIZE_512B);
    }

    if (s_pMemBlock_1K)
    {
        printf("*    1K     |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_1K, MEMBLOCK_NUM_SIZE_1K - s_pMemBlock_1K->uiSeqNum, s_pMemBlock_1K->uiSeqNum);
    }
    else
    {
        printf("*    1K     |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_1K, 0, MEMBLOCK_NUM_SIZE_1K);
    }

    if (s_pMemBlock_2K)
    {
        printf("*    2K     |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_2K, MEMBLOCK_NUM_SIZE_2K - s_pMemBlock_2K->uiSeqNum, s_pMemBlock_2K->uiSeqNum);
    }
    else
    {
        printf("*    2K     |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_2K, 0, MEMBLOCK_NUM_SIZE_2K);
    }

    if (s_pMemBlock_4K)
    {
        printf("*    4K     |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_4K, MEMBLOCK_NUM_SIZE_4K - s_pMemBlock_4K->uiSeqNum, s_pMemBlock_4K->uiSeqNum);
    }
    else
    {
        printf("*    4K     |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_4K, 0, MEMBLOCK_NUM_SIZE_4K);
    }

    if (s_pMemBlock_8K)
    {
        printf("*    8K     |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_8K, MEMBLOCK_NUM_SIZE_8K - s_pMemBlock_8K->uiSeqNum, s_pMemBlock_8K->uiSeqNum);
    }
    else
    {
        printf("*    8K     |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_8K, 0, MEMBLOCK_NUM_SIZE_8K);
    }

    if (s_pMemBlock_16K)
    {
        printf("*    16K    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_16K, MEMBLOCK_NUM_SIZE_16K - s_pMemBlock_16K->uiSeqNum, s_pMemBlock_16K->uiSeqNum);
    }
    else
    {
        printf("*    16K    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_16K, 0, MEMBLOCK_NUM_SIZE_16K);
    }

    if (s_pMemBlock_32K)
    {
        printf("*    32K    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_32K, MEMBLOCK_NUM_SIZE_32K - s_pMemBlock_32K->uiSeqNum, s_pMemBlock_32K->uiSeqNum);
    }
    else
    {
        printf("*    32K    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_32K, 0, MEMBLOCK_NUM_SIZE_32K);
    }

    if (s_pMemBlock_64K)
    {
        printf("*    64K    |    All:%d    Used:%u     Residue:%u\n", MEMBLOCK_NUM_SIZE_64K, MEMBLOCK_NUM_SIZE_64K - s_pMemBlock_64K->uiSeqNum, s_pMemBlock_64K->uiSeqNum);
    }
    else
    {
        printf("*    64K    |    All:%d    Used:%d     Residue:%d\n", MEMBLOCK_NUM_SIZE_64K, 0, MEMBLOCK_NUM_SIZE_64K);
    }
    printf("****************************************************************************************\n");

}

#elif defined(USE_STATIC_MALLOC_T)

static MemBlock* gs_pstMemBlock = NULL;
static MemBlockInfo gs_memBlockInfo = {NULL, NULL, 0, 0};
/*---------------------------------------------------------------------------------
-   º¯ Êý Ãû: InitMemBlock
-   ¹¦ÄÜÃèÊö: ¾²Ì¬ÄÚ´æ³õÊ¼»°
-   Êä    Èë:
-   ·µ    »Ø:
-   È«¾Ö±äÁ¿: ¸÷ÄÚ´æ¿éÊ×µØÖ·
-   ×¢    ÊÍ: È«¾Ö¾²Ì¬ÄÚ´æ³õÊ¼»°
-   ÐÞ¸Ä¼ÇÂ¼:
-   ÐÞ¸ÄÈÕÆÚ        °æ±¾        ÐÞ¸ÄÈË      ÐÞ¸ÄÔ­Òò¼°ÄÚÈÝ
=======================================================
-   2020\7\30       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitMemBlock()
{
	gs_pstMemBlock = (MemBlock*)malloc(STATIC_MEM_LEN);
	if (NULL == gs_pstMemBlock)
	{
		printf("Init memory fail!");
		Sleep(5000);
		exit(0);
	}

	gs_memBlockInfo.base = (CHAR*)gs_pstMemBlock;
	gs_memBlockInfo.end = (CHAR*)gs_pstMemBlock;;

	gs_pstMemBlock->end = (CHAR*)gs_pstMemBlock;
	gs_pstMemBlock->pNext = NULL;
}

/*---------------------------------------------------------------------------------
-   º¯ Êý Ãû: DDS_STATIC_MALLOC
-   ¹¦ÄÜÃèÊö: ¾²Ì¬ÄÚ´æ·ÖÅä
-   Êä    Èë:
-   ·µ    »Ø:
-   È«¾Ö±äÁ¿: ¸÷ÄÚ´æ¿éÊ×µØÖ·
-   ×¢    ÊÍ: ¾²Ì¬ÄÚ´æ·ÖÅä£¬×î´ó¿ÉÄÜµÃ·ÖÅäÄÚ´æ, ¹Ø¼üÊý¾ÝÊ¹ÓÃ
-   ÐÞ¸Ä¼ÇÂ¼:
-   ÐÞ¸ÄÈÕÆÚ        °æ±¾        ÐÞ¸ÄÈË      ÐÞ¸ÄÔ­Òò¼°ÄÚÈÝ
=======================================================
-   2020\7\28       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID* DDS_STATIC_MALLOC(UINT32 uiLen)
{
	DDS_MUTEX_LOCK(hMutex);
	gs_memBlockInfo.usBlockNum++;
	gs_memBlockInfo.uiBlockTotal += (uiLen + sizeof(MemBlock));

	MemBlock* pstPreMemBlock = NULL;
	MemBlock* pstMemBlock = gs_pstMemBlock;

	while (NULL != pstMemBlock)
	{
		if (NULL == pstPreMemBlock)
		{
			if ((CHAR*)pstMemBlock == pstMemBlock->end)
			{
				pstMemBlock->end = (CHAR*)pstMemBlock + uiLen + sizeof(MemBlock);
				/* Î²µØÖ· */
				gs_memBlockInfo.end = pstMemBlock->end;
				DDS_MUTEX_UNLOCK(hMutex);
				return (VOID*)((CHAR*)pstMemBlock + sizeof(MemBlock));
			}
		}
		else
		{
			if (uiLen <= (CHAR*)pstMemBlock - pstPreMemBlock->end)
			{
				MemBlock* pstNewMemBlock = (MemBlock*)pstPreMemBlock->end;
				pstNewMemBlock->end = pstPreMemBlock->end + uiLen + sizeof(MemBlock);
				pstNewMemBlock->pNext = pstMemBlock;
				pstPreMemBlock->pNext = pstNewMemBlock;
				DDS_MUTEX_UNLOCK(hMutex);
				return (VOID*)(pstPreMemBlock->end + sizeof(MemBlock));
			}
		}
		pstPreMemBlock = pstMemBlock;
		pstMemBlock = pstMemBlock->pNext;
	}

	if (uiLen <= (CHAR*)gs_pstMemBlock + STATIC_MEM_LEN - pstPreMemBlock->end)
	{
		MemBlock* pstNewMemBlock = (MemBlock*)pstPreMemBlock->end;
		pstNewMemBlock->end = pstPreMemBlock->end + uiLen + sizeof(MemBlock);
		pstNewMemBlock->pNext = pstMemBlock;
		pstPreMemBlock->pNext = pstNewMemBlock;
		/* Î²µØÖ· */
		gs_memBlockInfo.end = pstNewMemBlock->end;
		DDS_MUTEX_UNLOCK(hMutex);
		return (VOID*)(pstPreMemBlock->end + sizeof(MemBlock));
	}

	return NULL;
}


VOID DDS_STATIC_FREE(VOID* pMemBlock)
{
	DDS_MUTEX_LOCK(hMutex);

	MemBlock* pstPreMemBlock = NULL;
	MemBlock* pstMemBlock = gs_pstMemBlock;
	while (NULL != pstMemBlock)
	{
		MemBlock* pstCurMemBlock = (MemBlock*)((CHAR*)pMemBlock - sizeof(MemBlock));
		if (pstCurMemBlock == pstMemBlock)
		{
			if (NULL == pstPreMemBlock)
			{
				pstMemBlock->end = (CHAR*)pstMemBlock;
				gs_memBlockInfo.usBlockNum--;
				gs_memBlockInfo.uiBlockTotal -= (pstMemBlock->end - (CHAR*)pstMemBlock);
				if (!pstMemBlock->pNext)
				{
					gs_memBlockInfo.end = (CHAR*)pstMemBlock;
				}
				DDS_MUTEX_UNLOCK(hMutex);
				return;
			}
			else
			{
				pstPreMemBlock->pNext = pstMemBlock->pNext;
				gs_memBlockInfo.usBlockNum--;
				gs_memBlockInfo.uiBlockTotal -= (pstMemBlock->end - (CHAR*)pstMemBlock);
				if (!pstMemBlock->pNext)
				{
					gs_memBlockInfo.end = (CHAR*)pstMemBlock;
				}
				DDS_MUTEX_UNLOCK(hMutex);
				return;
			}
		}

		pstPreMemBlock = pstMemBlock;
		pstMemBlock = pstMemBlock->pNext;
	}
}

VOID PrintMemBlockInfo()
{
	printf("num:%u use total:%u total:%d free:%d\n", gs_memBlockInfo.usBlockNum, gs_memBlockInfo.uiBlockTotal, gs_memBlockInfo.end - gs_memBlockInfo.base, gs_memBlockInfo.end - gs_memBlockInfo.base - gs_memBlockInfo.uiBlockTotal);
}

#else

DDS_DLL VOID DDS_MemoryUsage()
{
    return;
}

VOID* DDS_STATIC_MALLOC(UINT32 uiLen)
{
    return malloc(uiLen);
}

VOID* DDS_STATIC_MALLOC_T(UINT32 uiLen)
{
    return malloc(uiLen);
}

VOID DDS_STATIC_FREE(VOID* pMemBlock)
{
     free(pMemBlock);
}

#endif
