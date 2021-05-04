/*
 * hostprotocol.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "project.h"
#include "scanner.h"
#include "switch.h"

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
    SCANNER_CTX* scanner;
    volatile SBUFSTATE sstate;
    int sending_len;
} HostProtocolCtx;

BOOL hostprotocol_init(HostProtocolCtx* ctx, SCANNER_CTX* scanner);
VSWITCH_CTX* hostprotocol_schedule(HostProtocolCtx* ctx, int now);

void hostprotocol_inform_sending_completion();

#ifdef __cplusplus
}
#endif
