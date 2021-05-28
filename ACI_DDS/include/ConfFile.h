/*********************************************************************************
*  OpenST Basic tool library
*  Copyright (C) 2019 QiYuan
*
*
*  This program is security software
*  without permission you cannot use or distribute the software
*  otherwise, legal liability will be investigated
*
*  If you want to use the software, please contact the authorities to obtain a license
*  < http://www.iamqy.com.cn >.
*
*  @file     ConfFile.h
*  @brief    ConfFile
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2020/04/28
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/08 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef _CONF_FILE_H_
#define _CONF_FILE_H_

#define INI_LINE_LEN     1024

typedef struct _KeyValue
{
    CHAR key[32];
    CHAR value[1024];
    struct _KeyValue* pNext;

}KeyValue;

typedef struct _ConfData
{
    CHAR label[32];

    KeyValue* pstKeyValue;

    struct _ConfData* pNext;

}ConfData;

extern BOOL ReadIniFile(const CHAR* filename);

extern VOID DeleteIniFile();

extern DDS_DLL BOOL GetStrValue(const CHAR*label, const CHAR* key, CHAR* value);

extern DDS_DLL BOOL GetIntValue(const CHAR*label, const CHAR* key, INT* value);

extern DDS_DLL BOOL GetLongValue(const CHAR*label, const CHAR* key, LONG* value);

extern DDS_DLL BOOL GetIntListValue(const CHAR*label, const CHAR* key, int * rArrValue, int *rSize);

#endif

