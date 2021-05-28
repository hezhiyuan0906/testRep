#include "../Include/GlobalDefine.h"

static FILE* g_logfile = NULL;
static CHAR g_logName[64] = "QY_DDS.log";
static CHAR g_logPath[256] = ".";
static INT g_logLevel = 0;
static INT g_logSaveTime = 0;
static CHAR g_fileName[256] = "";

DDS_MUTEX g_logMutex; //线程加锁

/*---------------------------------------------------------------------------------
-   函 数 名: TimeToStr
-   功能描述: 时间格式转换
-   输    入: 整形时间、输出格式、输出字符串
-   返    回: 无
-   全局变量:
-   注    释: 时间格式转换
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
DDS_DLL VOID TimeToStr(time_t timep, const char* format, char* timeStr)
{
    strftime(timeStr, 32, format, localtime(&timep));
}

/*---------------------------------------------------------------------------------
-   函 数 名: GetFormatTime
-   功能描述: 时间格式转换
-   输    入: 整形时间、输出格式、输出字符串
-   返    回: 无
-   全局变量:
-   注    释: 时间格式转换
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID GetFormatTime(char*nowtime, char* format)
{
    time_t tmp_time;
    struct tm p;
    time(&tmp_time);
    localtime_s(&p, &tmp_time);
    sprintf_s(nowtime, 32, format,
        p.tm_year + 1900,
        p.tm_mon + 1,
        p.tm_mday,
        p.tm_hour,
        p.tm_min,
        p.tm_sec);
}

/*---------------------------------------------------------------------------------
-   函 数 名: CreateFolder
-   功能描述: 创建文件夹
-   输    入: 路径
-   返    回: 无
-   全局变量:
-   注    释: 创建文件夹
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
BOOL CreateFolder(const char *path)
{
    int i = 0;
    char attr[256];
    char folder[64];
    assert(path != NULL);
	strncpy(attr, path, strlen(path));

    char *tmp = (char *)attr;
    while (*tmp != '\0')
    {
        folder[i++] = *tmp;
        if (*tmp == '/' || *tmp == '\\')
        {
            folder[i - 1] = '\0';
            if (strcmp(folder, ".."))
            {
                *tmp = '\0';
                if (_access(attr, 0))
                {
                    if (_mkdir(attr))
                    {
                        fprintf(stderr, "Failed to create directory %s:%s\n", attr, strerror(errno));
                        return FALSE;
                    }
                }
                *tmp = '/';
            }
            i = 0;
        }
        ++tmp;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------------
-   函 数 名: InitCommLog
-   功能描述: 初始化日志模块
-   输    入: 无
-   返    回: 无
-   全局变量:
-   注    释: 初始化日志模块
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID InitCommLog()
{
#ifdef LOG_PRINT
    DDS_MUTEX_INIT(g_logMutex);
    ReadIniFile("sys.ini");

    GetStrValue("log", "path", g_logPath);
    GetIntValue("log", "level", &g_logLevel);
    GetIntValue("log", "saveTime", &g_logSaveTime);

    CreateFolder(g_logPath);
#endif

}

/*---------------------------------------------------------------------------------
-   函 数 名: CrtLogFile
-   功能描述: 创建日志文件
-   输    入: 无
-   返    回: 无
-   全局变量:
-   注    释: 创建日志文件
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID CrtLogFile()
{
    CHAR nowtime[32];

    GetStrValue("log", "name", g_logName);
    CHAR* prefix = strtok(g_logName, ".");
    CHAR* suffix = strtok(NULL, ".");

    if (NULL == prefix || NULL == suffix)
    {
        return;
    }

    if (g_logfile != NULL)
    {
        fclose(g_logfile);
    }

    GetFormatTime(nowtime, "%04d%02d%02d");
    sprintf_s(g_fileName, 256, "%s/%s_%s.%s", g_logPath, prefix, nowtime, suffix);
    fopen_s(&g_logfile, g_fileName, "a+");

    printf("create log file: %s\n", g_fileName);
}

/*---------------------------------------------------------------------------------
-   函 数 名: DelLogFile
-   功能描述: 删除日志文件
-   输    入: 时间长度
-   返    回: 无
-   全局变量:
-   注    释: 删除日志文件，距当前时间长度外的文件删除
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID DelLogFile(long deltime)
{
    if (deltime <= 0)
    {
        return;
    }

    GetStrValue("log", "name", g_logName);
    CHAR* prefix = strtok(g_logName, ".");
    CHAR* suffix = strtok(NULL, ".");

    if (NULL == prefix || NULL == suffix)
    {
        return;
    }

    time_t curtime;
    time(&curtime);

    struct _finddata_t fa;
    intptr_t handle;
    CHAR path[256];

    sprintf_s(path, 256, "%s\\%s", g_logPath, "*");

    if ((handle = _findfirst(path, &fa)) == -1L)
    {
        printf("The Path %s is wrong!\n", path);
        return;
    }

    do
    {
        if (strstr(fa.name, suffix) && abs((long)(fa.time_create - curtime)) >= deltime)
        {
            sprintf_s(path, 256, "%s\\%s", g_logPath, fa.name);
            if (remove(path) == 0)
            {
                printf("delete %s successful!\n", path);
            }
            else
            {
                printf("delete %s fail!\n", path);
            }
        }

    } while (_findnext(handle, &fa) == 0); /* 成功找到时返回0*/

    printf("\n");
    _findclose(handle);
}

/*---------------------------------------------------------------------------------
-   函 数 名: PrintLog
-   功能描述: 打印日志
-   输    入: 日志级别、日志内容
-   返    回: 无
-   全局变量:
-   注    释: 打印日志
-   修改记录:
-   修改日期        版本        修改人      修改原因及内容
==================================================================================
-   2020\06\11       1.0         Hunter      Created
----------------------------------------------------------------------------------*/
VOID PrintLog(int level, const char* vargformat, ...)
{
#ifdef LOG_PRINT
	DDS_MUTEX_LOCK(g_logMutex);

    CHAR nowtime[32];
    va_list args;
    va_start(args, vargformat);

    GetFormatTime(nowtime, "%04d%02d%02d");

    if (NULL == g_logfile || !strstr(g_fileName, nowtime))
    {
        CrtLogFile();
    }

    if (NULL == g_logfile)
    {
        va_end(args);
        return;
    }

    GetFormatTime(nowtime, "%04d-%02d-%02d %02d:%02d:%02d");
    switch (level)
    {
        case COMMLOG_INFO:
            if ((g_logLevel >> 0) & 0x01)
            {
                fprintf(g_logfile, "%s Info:", nowtime);
            }
            break;
        case COMMLOG_WARN:
            if ((g_logLevel >> 1) & 0x01)
            {
                fprintf(g_logfile, "%s Warning:", nowtime);
            }
            break;
        case COMMLOG_ERROR:
            if ((g_logLevel >> 2) & 0x01)
            {
                fprintf(g_logfile, "%s Error:", nowtime);
            }
            break;
        default:
            DDS_TRACEDEBUG("CCommLog inputpara <level> no distinguished!");
            break;
    }

    vfprintf(g_logfile, vargformat, args);
    va_end(args);
    fflush(g_logfile);
    
    DelLogFile(24 * 60 * 60 * g_logSaveTime);

    DDS_MUTEX_UNLOCK(g_logMutex);
#endif
}
