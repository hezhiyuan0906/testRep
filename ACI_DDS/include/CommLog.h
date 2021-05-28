#ifndef HIPERD_COMMLOG_H
#define HIPERD_COMMLOG_H

#define COMMLOG_INFO    0
#define COMMLOG_WARN    1
#define COMMLOG_ERROR   2

#define DDS_TRACEDEBUG(format,...) printf("FILE: "__FILE__"(%s\t%s), LINE: %d ====> "format"\n",__DATE__,__TIME__,__LINE__,##__VA_ARGS__)
#define DDS_DEBUG(format,...) fprintf(g_logptr, "[%s:%d] " format "/n", __FILE__, __LINE__, ##__VA_ARGS__);
#define DDS_TRACEFUNCDEBUG(fortmat,...) printf("FUNCTION: "__FUNCTION__"(%s\t%s), LINE: %d ====> "format"\n",__DATE__,__TIME__,__LINE__,##__VA_ARGS__)

extern DDS_DLL void TimeToStr(time_t timep, const char* format, char* timeStr);
extern VOID InitCommLog();
extern BOOL CreateFolder(const char *path);
extern VOID CrtLogFile();
extern VOID DelLogFile(long deltime);
extern VOID PrintLog(int level, const char* vargformat, ...);
#endif