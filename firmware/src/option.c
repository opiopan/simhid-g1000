/*
 * option.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include <string.h>
#include "project.h"
#include "option.h"

static struct OPTION{
    OPTION_DEF def;
    OPTION_VALDATA data;
}options[] = {
    {{.name = "DEBUG", .type = OPTVAL_BOOL, .maxlen = sizeof(BOOL)}, {.boolval = FALSE}},
    {{.name = "EVLOG", .type = OPTVAL_BOOL, .maxlen = sizeof(BOOL)}, {.boolval = FALSE}},
};

int option_getOptNum()
{
    return sizeof(options) / sizeof(options[0]);
}

OPTION_DEF *option_getOptDef(int ix)
{
    return &options[ix].def;
}

BOOL option_setValue(int ix, OPTION_VALUE value)
{
    struct OPTION *option = options + ix;
    if (value.type != option->def.type){
        return FALSE;
    }
    if (value.type == OPTVAL_STRING ){
        int len = strlen(value.data.strval);
        if (len >= option->def.maxlen){
            return FALSE;
        }
        strcpy((char*)option->data.strval, value.data.strval);
    }else if (value.type == OPTVAL_BOOL){
        option->data.boolval = (value.data.boolval != FALSE);
    }else if (value.type == OPTVAL_INTEGER){
        option->data.intval = value.data.intval;
    }
    return FALSE;
}

OPTION_VALUE option_getValue(int ix)
{
    struct OPTION *option = options + ix;
    OPTION_VALUE val = {
        .type = option->def.type,
        .data = option->data,
    };
    return val;
}
