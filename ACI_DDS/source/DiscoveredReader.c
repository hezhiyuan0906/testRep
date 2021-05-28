#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredReader_Init
-   功能描述: 初始化远端阅读器
-   输    入: 远端阅读器
-   返    回:
-   全局变量:
-   注    释: 初始化远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID  DDS_DiscoveredReader_Init(DDS_DiscoveredReader* pstDiscReader)
{
    if (NULL != pstDiscReader)
    {
        pstDiscReader->pstLocalWriter = NULL;
        pstDiscReader->pstTwinDiscReader = NULL;
        pstDiscReader->pstUnicastLocator = NULL;
        pstDiscReader->pNext = NULL;
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DDS_DiscoveredReader_Uninit
-   功能描述: 去始化远端阅读器
-   输    入: 远端阅读器
-   返    回:
-   全局变量:
-   注    释: 去始化远端阅读器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredReader_Uninit(DDS_DiscoveredReader* pstDiscReader)
{
    if (NULL != pstDiscReader)
    {
        DDS_LocalWriter*  pstLocalWriter = NULL;
        LIST_DELETE_ALL(pstDiscReader->pstLocalWriter, pstLocalWriter);

        DDS_DiscoveredReader* pstTwinDiscReader = NULL;
        LIST_DELETE_ALL(pstDiscReader->pstTwinDiscReader, pstTwinDiscReader);
        //Locator_t* pstTempNode = NULL;
        ///* 析构单播地址 */
        //LIST_DELETE_ALL(pstDiscReader->unicastLocator, pstTempNode);

        ///* 析构组播地址 */
        //LIST_DELETE_ALL(pstDiscReader->multicastLocator, pstTempNode);
    }
}

/*---------------------------------------------------------------------------------
-   函 数 名: DiscoveredReaderAddLocalWriter
-   功能描述: 添加本地写入器
-   输    入: 远端阅读器、guid
-   返    回:
-   全局变量:
-   注    释: 添加本地写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL DiscoveredReaderAddLocalWriter(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid)
{
    DDS_LocalWriter* pstTempLocalWriter = NULL;

    if (NULL == pstDiscReader)
    {
        return FALSE;
    }

    DDS_LocalWriter* pstLocalWriter = (DDS_LocalWriter*)DDS_STATIC_MALLOC(sizeof(DDS_LocalWriter));
    if (NULL == pstLocalWriter)
    {
        return FALSE;
    }
    
    pstLocalWriter->uiSendHeartBeatNum = 0;    
    pstLocalWriter->guid = *pstGuid;
    pstLocalWriter->stAckNack.count = 0;

    LIST_INSERT_TAIL(pstDiscReader->pstLocalWriter, pstLocalWriter, pstTempLocalWriter);

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: DiscoveredReaderAddLocalWriter
-   功能描述: 添加本地写入器
-   输    入: 远端阅读器、guid
-   返    回:
-   全局变量:
-   注    释: 添加本地写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL DiscoveredReaderDelLocalWriter(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid)
{

    DDS_LocalWriter*  pstTempLocalWriter = NULL;
    DDS_LocalWriter*  pstLocalWriter = pstDiscReader->pstLocalWriter;

    while (NULL != pstLocalWriter)
    {
        if (Guid_Is_Equal(&pstLocalWriter->guid, pstGuid))
        {
            if (!pstTempLocalWriter)
            {
                pstLocalWriter = pstLocalWriter->pNext;
                DDS_STATIC_FREE(pstDiscReader->pstLocalWriter);
                pstDiscReader->pstLocalWriter = pstLocalWriter;
            }
            else
            {
                pstTempLocalWriter->pNext = pstLocalWriter->pNext;
                DDS_STATIC_FREE(pstLocalWriter);
                pstLocalWriter = pstTempLocalWriter->pNext;
            }
        }
        else
        {
            pstTempLocalWriter = pstLocalWriter;
            pstLocalWriter = pstLocalWriter->pNext;
        }
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: TwinDiscoveredReaderChangePosition
-   功能描述: 兄弟写入器置顶
-   输    入: 远端阅读器
-   返    回:
-   全局变量:
-   注    释: 兄弟写入器置顶
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID TwinDiscoveredReaderChangePosition(DDS_DiscoveredReader* pstDiscReader)
{
    DDS_DiscoveredReader* pstTwinDiscReader = pstDiscReader->pstTwinDiscReader;
    if (NULL != pstTwinDiscReader)
    {
        pstDiscReader->guid = pstTwinDiscReader->guid;
        pstDiscReader->stDataReaderQos = pstTwinDiscReader->stDataReaderQos;
        pstDiscReader->pstTwinDiscReader = pstTwinDiscReader->pNext;

        DDS_DiscoveredReader_Uninit(pstTwinDiscReader);
        DDS_STATIC_FREE(pstTwinDiscReader);
    }
}
/*---------------------------------------------------------------------------------
-   函 数 名: GetDiscoveredReaderLocalWriterByGuid
-   功能描述: 获取远端阅读器的本地写入器
-   输    入: 远端阅读器、guid
-   返    回:
-   全局变量:
-   注    释: 获取远端阅读器的本地写入器
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_LocalWriter*  GetDiscoveredReaderLocalWriterByGuid(DDS_DiscoveredReader* pstDiscReader, GUID_t* pstGuid)
{
    DDS_LocalWriter* pstLocalWriter = pstDiscReader->pstLocalWriter;
    while (NULL != pstLocalWriter)
    {
        if (Guid_Is_Equal(&pstLocalWriter->guid, pstGuid))
        {
            return pstLocalWriter;
        }
        pstLocalWriter = pstLocalWriter->pNext;
    }

    return NULL;
}
