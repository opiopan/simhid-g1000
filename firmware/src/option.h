/*
 * option.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "project.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    OPT_DEBUG = 0,
    OPT_EVLOG,
}OPTION;

typedef enum{
    OPTVAL_BOOL,
    OPTVAL_INTEGER,
    OPTVAL_STRING,
}OPTION_VALTYPE;

typedef struct{
    const char* name;
    OPTION_VALTYPE type;
    int maxlen;
}OPTION_DEF;

typedef union{
    BOOL boolval;
    int intval;
    const char* strval;
}OPTION_VALDATA;

typedef struct{
    OPTION_VALTYPE type;
    OPTION_VALDATA data;
}OPTION_VALUE;

int option_getOptNum();
OPTION_DEF* option_getOptDef(int ix);
BOOL option_setValue(int ix, OPTION_VALUE value);
OPTION_VALUE option_getValue(int ix);

#ifdef __cplusplus
}
#endif