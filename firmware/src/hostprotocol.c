/*
 * hostprotocol.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include <string.h>
#include "usbd_cdc_if.h"
#include "project.h"
#include "switch.h"
#include "scanner.h"
#include "hostprotocol.h"

/*========================================================
 Send buffer operations
========================================================*/
#define SENDBUF_MASK 0xff
static struct {
    char buf[SENDBUF_MASK + 1];
    int top;
    int bottom;
}SENDBUF;

static inline int sendbuf_len()
{
    return SENDBUF.top - SENDBUF.bottom;
}

static inline int sendbuf_freelen()
{
    return sizeof(SENDBUF.buf) - sendbuf_len();
}

static int sendbuf_add(char* data, int len)
{
    len = MIN(len, sendbuf_freelen());
    int copied = 0;
    if (copied < len){
        int begin = SENDBUF.top & SENDBUF_MASK;
        copied = MIN(len, sizeof(SENDBUF.buf) - begin);
        memcpy(SENDBUF.buf + begin, data, copied);
        if (copied < len){
            memcpy(SENDBUF.buf, data + copied, len - copied);
        }
        SENDBUF.top += len;
    }
    return len;
}

static inline char* sendbuf_datablock()
{
    return SENDBUF.buf + (SENDBUF.bottom & SENDBUF_MASK);
}

static inline int sendbuf_datablocklen()
{
    int block_start = SENDBUF.bottom & SENDBUF_MASK;
    int block_end = SENDBUF.top & SENDBUF_MASK;
    return block_end < block_start ? sizeof(SENDBUF.buf) - block_start : 
                                     block_end - block_start;
}

void sendbuf_remove(int len)
{
    SENDBUF.bottom += len;
    if (SENDBUF.bottom == SENDBUF.top){
        SENDBUF.top = 0;
        SENDBUF.bottom = 0;
    }
}

/*========================================================
 Host communication scheduler
========================================================*/
static HostProtocolCtx* saved_ctx;
BOOL hostprotocol_init(HostProtocolCtx *ctx, SCANNER_CTX *scanner)
{
    ctx->scanner = scanner;
    ctx->sstate = SBUF_FREE;
    saved_ctx = ctx;
    return TRUE;
}

VSWITCH_CTX *hostprotocol_schedule(HostProtocolCtx *ctx, int now)
{
    if (ctx->sstate == SBUF_SENT){
        sendbuf_remove(ctx->sending_len);
        ctx->sstate = SBUF_FREE;
    }

    VSWITCH_CTX* sw = NULL;
    static char evresp[2 + SWITCHSTATE_MAXLEN + 2] = "S ";
    while (scanner_updatedSwitchNum(ctx->scanner) && 
           sendbuf_freelen() >= sizeof(evresp)){
        sw = scanner_getUpdatedSwitch(ctx->scanner);
        int len = 2;
        len += sw->ops->printstate(sw, evresp + len, sizeof(evresp) - len - 2);
        evresp[len++] = '\r';
        evresp[len++] = '\n';
        sendbuf_add(evresp, len);
        sw->ops->commit(sw);
    }

    if (ctx->sstate == SBUF_FREE && sendbuf_len() > 0){
        char* addr = sendbuf_datablock();
        int len = sendbuf_datablocklen();
        ctx->sstate = SBUF_SENDING;
        ctx->sending_len = len;
        CDC_Transmit_FS((uint8_t *)addr, len);
    }

    return sw;
}

void hostprotocol_inform_sending_completion()
{
    saved_ctx->sstate = SBUF_SENT;
}
