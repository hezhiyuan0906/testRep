#include "../Include/GlobalDefine.h"

/*---------------------------------------------------------------------------------
-   �� �� ��: DDS_DiscoveredWriter_Init
-   ��������: ��ʼ��Զ��д����
-   ��    ��: ����д����
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ��ʼ��Զ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
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
-   �� �� ��: DDS_DiscoveredWriter_Uninit
-   ��������: ȥʼ��Զ��д����
-   ��    ��: ����д����
-   ��    ��:
-   ȫ�ֱ���:
-   ע    ��: ȥʼ��Զ��д����
-   �޸ļ�¼:
-   �޸�����        �汾        �޸���      �޸�ԭ������
==================================================================================
-   2019\8\8       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DDS_DiscoveredWriter_Uninit(DDS_DiscoveredWriter * pstDiscWriter)
{
    if (NULL != pstDiscWriter)
    {
        /* ������������*/
        UninitHistoryCache(&pstDiscWriter->stHistoryCache);

        //Locator_t* pstTempNode = NULL;
        ///* ����������ַ */
        //LIST_DELETE_ALL(pstDiscWriter->unicastLocator, pstTempNode);

        ///* �����鲥��ַ */
        //LIST_DELETE_ALL(pstDiscWriter->multicastLocator, pstTempNode);

    }
}
