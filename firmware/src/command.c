/*
 * command.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include "project.h"
#include "appmain.h"
#include "command.h"

static const char* PRODUCTNAME = "SimHID G1000";

/*========================================================
 Error message
========================================================*/
static const char* ERR_SYNTAX = "syntax error";
static const char* ERR_TOOLONG = "too long line";
static const char* ERR_TOOMANYPARAM = "parameters of command must be less than 10";
static const char* ERR_NOCMD = "not supported command";

/*========================================================
 Command parser implementation
========================================================*/
void command_parser_init(CommandParserCtx *ctx, char *buf, int len)
{
    ctx->phase = PARSE_INIT;
    ctx->linebuf = buf;
    ctx->linebuflen = len;
    ctx->parsedlen = 0;
    ctx->paramnum = 0;
    ctx->command = -1;
    ctx->err = NULL;
}

static inline BOOL isSeparator(int c)
{
    return c == ' ' || c == '\t';
}

static inline BOOL isCommand(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static inline BOOL isNumeric(int c)
{
    return c >= '0' && c <= '9';
}

BOOL command_parser_parse(CommandParserCtx *ctx, int c)
{
    BOOL rc = FALSE;

    if (ctx->phase == PARSE_END){
        ctx->phase = PARSE_INIT;
        ctx->parsedlen = 0;
        ctx->paramnum = 0;
        ctx->command = -1;
        ctx->err = NULL;
    }

    if (ctx->parsedlen == ctx->linebuflen && ctx->phase != PARSE_SKIP){
        ctx->phase = PARSE_SKIP;
        ctx->err = ERR_TOOLONG;
    }
    if (ctx->phase != PARSE_SKIP || ctx->phase != PARSE_EOL){
        ctx->linebuf[ctx->parsedlen++] = c;
    }

    switch (ctx->phase){
    case PARSE_INIT:{
        if (isSeparator(c)){
            /* nothing to do */
        }else if (isCommand(c)){
            ctx->phase = PARSE_POSTCOMMAND;
            ctx->command = c;
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }else{
            ctx->phase = PARSE_SKIP;
            ctx->err = ERR_SYNTAX;
        }
        break;
    }
    case PARSE_POSTCOMMAND:{
        if (isSeparator(c)){
            ctx->phase = PARSE_SEPARATOR;
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }else{
            ctx->phase = PARSE_SKIP;
            ctx->err = ERR_SYNTAX;
        }
        break;
    }
    case PARSE_SEPARATOR:{
        if (isSeparator(c)){
            /* nothing to do */
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }else if (ctx->paramnum >= COMMAND_MAX_PARAM){
            ctx->phase = PARSE_SKIP;
            ctx->err = ERR_TOOMANYPARAM;
        }else{
            CommandParam* param = ctx->params + ctx->paramnum++;
            param->strvalue = ctx->linebuf + ctx->parsedlen - 1;
            param->isNumber = FALSE;
            param->len = 1;
            if (c == '-'){
                ctx->phase = PARSE_MINUSNUMBER;
            }else if (isNumeric(c)){
                ctx->phase = PARSE_NUMBER;
                param->numvalue = c - '0';
            }else{
                ctx->phase = PARSE_STRING;
            }
        }
        break;
    }
    case PARSE_MINUSNUMBER:{
        if (isSeparator(c)){
            ctx->phase = PARSE_SEPARATOR;
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }else{
            CommandParam *param = ctx->params + ctx->paramnum - 1;
            param->len++;
            if (isNumeric(c)){
                ctx->phase = PARSE_NUMBER;
                param->numvalue = -(c - '0');
            }else{
                ctx->phase = PARSE_STRING;
            }
        }
        break;
    }
    case PARSE_NUMBER:{
        CommandParam *param = ctx->params + ctx->paramnum - 1;
        if (isSeparator(c)){
            ctx->phase = PARSE_SEPARATOR;
            param->isNumber = TRUE;
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
            param->isNumber = TRUE;
        }else{
            param->len++;
            if (isNumeric(c)){
                param->numvalue *= 10;
                param->numvalue += param->numvalue > 0 ? 
                                   c - '0' : -(c - '0');
            }else{
                ctx->phase = PARSE_STRING;
            }
        }
        break;
    }
    case PARSE_STRING:{
        CommandParam *param = ctx->params + ctx->paramnum - 1;
        if (isSeparator(c)){
            ctx->phase = PARSE_SEPARATOR;
        }else if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }else{
            param->len++;
        }
        break;
    }
    case PARSE_SKIP:{
        if (c == '\r'){
            ctx->phase = PARSE_EOL;
        }
        break;
    }
    case PARSE_EOL:{
        if (c == '\n'){
            ctx->phase = PARSE_END;
            rc = TRUE;
        }else{
            ctx->phase = PARSE_SKIP;
            ctx->err = ERR_SYNTAX;
        }
        break;
    }
    case PARSE_END:{
        /* no condition reach here */
        break;
    }
    }

    return rc;
}

/*========================================================
 Individual command implementation
========================================================*/

/*--------------------------------------------------------
 NOP command
--------------------------------------------------------*/
static void nop_init(void *ctx, CommandParserCtx *cmd)
{
}

static int nop_schedule(void *ctx, char *respbuf, int len)
{
    return 0;
}

static BOOL nop_isfinished(void *ctx)
{
    return TRUE;
}

static CMDOPS cmd_nop = {
    .init = nop_init,
    .schedule = nop_schedule,
    .isfinished = nop_isfinished,
};

/*--------------------------------------------------------
 Error response command
--------------------------------------------------------*/
typedef struct{
    const char* msg;
}ERRCTX;

static void err_init(void *ctx, CommandParserCtx *cmd)
{
    ERRCTX* rctx = (ERRCTX*)ctx;
    rctx->msg = cmd->err;
}

static int err_schedule(void *ctx, char *respbuf, int len)
{
    ERRCTX *rctx = (ERRCTX *)ctx;
    return snprintf(respbuf, len, "E %s\r\n", rctx->msg);
}

static BOOL err_isfinished(void *ctx)
{
    return TRUE;
}

static CMDOPS cmd_err = {
    .init = err_init,
    .schedule = err_schedule,
    .isfinished = err_isfinished,
};

/*--------------------------------------------------------
 Identifier command
--------------------------------------------------------*/
static void id_init(void *ctx, CommandParserCtx *cmd)
{
}

static int id_schedule(void *ctx, char *respbuf, int len)
{
    return snprintf(respbuf, len, "I %s\r\n", PRODUCTNAME);
}

static BOOL id_isfinished(void *ctx)
{
    return TRUE;
}

static CMDOPS cmd_id = {
    .init = id_init,
    .schedule = id_schedule,
    .isfinished = id_isfinished,
};

/*--------------------------------------------------------
 Retrieving switch definitions command
--------------------------------------------------------*/
typedef struct {
    int swgix;
    int swix;
    BOOL completed;
}SWDEFCTX;

static void swdef_init(void *ctx, CommandParserCtx *cmd)
{
    SWDEFCTX* rctx = (SWDEFCTX*)ctx;
    *rctx = (SWDEFCTX){};
}

static int swdef_schedule(void *ctx, char *respbuf, int len)
{
    SWDEFCTX* rctx = (SWDEFCTX*)ctx;
    if (rctx->swgix < 0){
        rctx->completed = TRUE;
        return strlcpy(respbuf, "D\r\n", len);
    }

    SCANNER_CTX *scanner = app_getScannerCtx();
    VSWG_CTX* swg = scanner_getswg(scanner, rctx->swgix);
    VSWITCH_CTX* sw = SWG_GET_SWITCH(swg, rctx->swix);

    int rc = snprintf(respbuf, len, "D %s ", sw->name);
    rc += sw->ops->printdef(sw, respbuf + rc, len - rc);
    rc += strlcpy(respbuf + rc, "\r\n", len - rc);

    rctx->swix++;
    if (rctx->swix >= SWG_GET_SWITCHNUM(swg)){
        rctx->swgix++;
        rctx->swix = 0;
        if (rctx->swgix > scanner_getswgnum(scanner)){
            rctx->swgix = -1;
            rctx->swix = -1;
        }
    }

    return rc;
}

static BOOL swdef_isfinished(void *ctx)
{
    SWDEFCTX *rctx = (SWDEFCTX *)ctx;
    return rctx->completed;
}

static CMDOPS cmd_swdef = {
    .init = swdef_init,
    .schedule = swdef_schedule,
    .isfinished = swdef_isfinished,
};

/*========================================================
 Command executor implementation
========================================================*/
void command_executor_init(CommandExecutorCtx *ctx, CommandParserCtx *command)
{
    ctx->status = CMD_INIT;
    int cmdchr = command->command;
    if (command->err){
        ctx->ops = &cmd_err;
    }else if (cmdchr < 0){
        ctx->ops = &cmd_nop;
    }else if (cmdchr == 'i' || cmdchr == 'I'){
        ctx->ops = &cmd_id;
    }else if (cmdchr == 'd' || cmdchr == 'D'){
        ctx->ops = &cmd_swdef;
    }else{
        command->err = ERR_NOCMD;
        ctx->ops = &cmd_err;
    }
    ctx->ops->init(ctx->cmdcontext, command);
}

int command_exectutor_schedule(CommandExecutorCtx *ctx, char *respbuf, int len)
{
    int rc = 0;
    if (ctx->status != CMD_COMPLETE){
        rc = ctx->ops->schedule(ctx->cmdcontext, respbuf, len);
        if (ctx->ops->isfinished(ctx->cmdcontext)){
            ctx->status = CMD_COMPLETE;
        }else{
            ctx->status = CMD_EXECUTING;
        }
    }
    return rc;
}
