/*
 * command.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "project.h"
#include "scanner.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*========================================================
 Command parser
========================================================*/
#define COMMAND_MAX_PARAM 10

typedef enum{
        PARSE_INIT = 0,
        PARSE_POSTCOMMAND,
        PARSE_SEPARATOR,
        PARSE_MINUSNUMBER,
        PARSE_NUMBER,
        PARSE_STRING,
        PARSE_SKIP,
        PARSE_EOL,
        PARSE_END,
    } CMDPARSE_PHASE;

typedef struct{
    BOOL isNumber;
    const char *strvalue;
    int len;
    int numvalue;
}CommandParam;

typedef struct{
    char* linebuf;
    int linebuflen;
    int parsedlen;
    CMDPARSE_PHASE phase;
    int command;
    int paramnum;
    CommandParam params[COMMAND_MAX_PARAM];
    const char* err;
} CommandParserCtx;

void command_parser_init(CommandParserCtx* ctx, char* buf, int len);
BOOL command_parser_parse(CommandParserCtx* ctx, int c);


/*========================================================
 Command Executor
========================================================*/
#define CMDCONTEXT_MAXSIZE  32

typedef struct{
    void (*init)(void* ctx, CommandParserCtx* cmd);
    int (*schedule)(void* ctx, char* respbuf, int len);
    BOOL (*isfinished)(void* ctx);
}CMDOPS;

typedef enum{
    CMD_INIT,
    CMD_EXECUTING,
    CMD_COMPLETE,
} CMDSTATUS;

typedef struct{
    CMDSTATUS status;
    char cmdcontext[CMDCONTEXT_MAXSIZE];
    CMDOPS* ops;
}CommandExecutorCtx;

#define COMMAND_EXECUTOR_EXECUTING(ctx) ((ctx)->status == CMD_EXECUTING)

void command_executor_init(CommandExecutorCtx* ctx, CommandParserCtx* command);
int command_exectutor_schedule(CommandExecutorCtx* ctx, char* respbuf, int len);

#ifdef __cplusplus
}
#endif
