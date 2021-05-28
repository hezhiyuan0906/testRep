#include "../Include/GlobalDefine.h"

void getStrFromLong(EX_UINT32 val,
    unsigned char * v3,
    unsigned char * v2,
    unsigned char * v1,
    unsigned char * v0)
{
    *v3 = (char)(val >> 24 & 0xff);
    *v2 = (char)(val >> 16 & 0xff);
    *v1 = (char)(val >> 8 & 0xff);
    *v0 = (char)(val & 0xff);
}