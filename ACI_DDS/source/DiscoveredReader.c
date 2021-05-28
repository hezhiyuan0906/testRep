#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DiscoveredReader_Init
-   ��������: ��ʼ��Զ���Ķ���
-   ��    ��: Զ���Ķ���
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ʼ��Զ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: DDS_DiscoveredReader_Uninit
-   ��������: ȥʼ��Զ���Ķ���
-   ��    ��: Զ���Ķ���
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ȥʼ��Զ���Ķ���
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
        ///* ����������ַ */
        //LIST_DELETE_ALL(pstDiscReader->unicastLocator, pstTempNode);

        ///* �����鲥��ַ */
        //LIST_DELETE_ALL(pstDiscReader->multicastLocator, pstTempNode);
    }
}

/*---------------------------------------------------------------------------------
-   �� �� ��: DiscoveredReaderAddLocalWriter
-   ��������: ��ӱ���д����
-   ��    ��: Զ���Ķ�����guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ӱ���д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: DiscoveredReaderAddLocalWriter
-   ��������: ��ӱ���д����
-   ��    ��: Զ���Ķ�����guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ӱ���д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: TwinDiscoveredReaderChangePosition
-   ��������: �ֵ�д�����ö�
-   ��    ��: Զ���Ķ���
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: �ֵ�д�����ö�
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: GetDiscoveredReaderLocalWriterByGuid
-   ��������: ��ȡԶ���Ķ����ı���д����
-   ��    ��: Զ���Ķ�����guid
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ȡԶ���Ķ����ı���д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
