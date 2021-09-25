/*
 * hostprotocol.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "project.h"
#include "scanner.h"
#include "switch.h"
#include "command.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    SBUF_FREE = 0,
    SBUF_SENDING,
    SBUF_SENT,
} SBUFSTATE;

typedef struct {
    enum {
        PARSING,
        EXECUTING,
    }cmdstatus;
    CommandParserCtx parser;
    CommandExecutorCtx executor;
    char* respbuf;
    int respbuf_len;
    int respbuf_used;
    SCANNER_CTX* scanner;
    volatile SBUFSTATE sstate;
    int sending_len;
    int send_time;
    int send_flush_time;
} HostProtocolCtx;

BOOL hostprotocol_init(HostProtocolCtx* ctx, SCANNER_CTX* scanner);
VSWITCH_CTX* hostprotocol_schedule(HostProtocolCtx* ctx, int now);

void hostprotocol_inform_sending_completion();
void hostprotocol_recvbuf_sync_it();
int hostprotocol_recvbuf_add_it(char *buf, int len);

#ifdef __cplusplus
}
#endif
