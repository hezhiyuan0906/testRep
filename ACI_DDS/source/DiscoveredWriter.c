#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredWriter_Init
-   功能描述: 初始化远端写入器
-   输    入: 发现写入器
-   返    回:
-   全局变量:
-   注    释: 初始化远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredWriter_Init(DDS_DiscoveredWriter * pstDiscWriter)
{
    if (NULL != pstDiscWriter)
    {
        pstDiscWriter->pstUnicastLocator = NULL;
        pstDiscWriter->pNext = NULL;
        pstDiscWriter->stAckNack.count = 0;
        pstDiscWriter->stAckNack.readerSNState.base = SEQUENCENUMBER_START;
        pstDiscWriter->stHeartbeat.count = 0;
		pstDiscWriter->durabilityFlag = FALSE;
        InitHistoryCache(&pstDiscWriter->stHistoryCache);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredWriter_Uninit
-   功能描述: 去始化远端写入器
-   输    入: 发现写入器
-   返    回:
-   全局变量:
-   注    释: 去始化远端写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredWriter_Uninit(DDS_DiscoveredWriter * pstDiscWriter)
{
    if (NULL != pstDiscWriter)
    {
        /* 先析构缓存器*/
        UninitHistoryCache(&pstDiscWriter->stHistoryCache);

        //Locator_t* pstTempNode = NULL;
        ///* 析构单播地址 */
        //LIST_DELETE_ALL(pstDiscWriter->unicastLocator, pstTempNode);

        ///* 析构组播地址 */
        //LIST_DELETE_ALL(pstDiscWriter->multicastLocator, pstTempNode);

    }
}
