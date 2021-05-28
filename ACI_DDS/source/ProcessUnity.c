#include "../Include/GlobalDefine.h"

int getProcessId()
{
#if defined (_WIN32)
    int pid = _getpid();
    return pid;
#elif defined(_LINUX)
    int pid = getpid();
    return pid;
#elif defined(_VXWORKS)
    return taskIdSelf();
#endif // defined
}