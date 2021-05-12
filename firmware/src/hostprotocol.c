/*
 * hostprotocol.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include <string.h>
#include "usbd_cdc_if.h"
#include "project.h"
#include "switch.h"
#include "scanner.h"
#include "command.h"
#include "option.h"
#include "hostprotocol.h"

#define LINEBUFLEN 128
#define RESPBUFLEN 100

/*========================================================
 Send buffer operations
========================================================*/
#define SENDBUF_MASK 0x7f
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
 Receive buffer operations
========================================================*/
#define RECVBUF_MASK 0x7f
static struct
{
    char buf[RECVBUF_MASK + 1];
    struct {
        int top;
        int bottom;
    }it;
    struct {
        int top;
        int bottom;
    }normal;
} RECVBUF;

// functions for interruption handler
void hostprotocol_recvbuf_sync_it()
{
    RECVBUF.it.bottom = RECVBUF.normal.bottom;
};


static inline int recvbuf_len_it()
{
    return RECVBUF.it.top - RECVBUF.it.bottom;
}

static inline int recvbuf_freelen_it()
{
    return sizeof(RECVBUF.buf) - recvbuf_len_it();
}

int hostprotocol_recvbuf_add_it(char* buf, int len)
{
    len = MIN(len, recvbuf_freelen_it());
    int copied = 0;
    if (copied < len){
        int begin = RECVBUF.it.top & RECVBUF_MASK;
        copied = MIN(len, sizeof(RECVBUF.buf) - begin);
        memcpy(RECVBUF.buf + begin, buf, copied);
        if (copied < len){
            memcpy(RECVBUF.buf, buf + copied, len - copied);
        }
        RECVBUF.it.top += len;
    }
    return len;
}

// functions for normal contxet
static inline void recvbuf_sync()
{
    RECVBUF.normal.top = RECVBUF.it.top;
};

static inline int recvbuf_len()
{
    return RECVBUF.normal.top - RECVBUF.normal.bottom;
}

static inline int recvbuf_freelen()
{
    return sizeof(RECVBUF.buf) - recvbuf_len();
}

static int rcvbuf_getc()
{
    if (RECVBUF.normal.top == RECVBUF.normal.bottom){
        return -1;
    }
    int rc = RECVBUF.buf[RECVBUF.normal.bottom & RECVBUF_MASK];
    RECVBUF.normal.bottom++;
    return rc;
}

/*========================================================
 Host communication scheduler
========================================================*/
static HostProtocolCtx *saved_ctx;
static char linebuf[LINEBUFLEN];
static char respbuf[RESPBUFLEN];

BOOL hostprotocol_init(HostProtocolCtx *ctx, SCANNER_CTX *scanner)
{
    ctx->cmdstatus = PARSING;
    command_parser_init(&ctx->parser, linebuf, sizeof(linebuf));
    ctx->respbuf = respbuf;
    ctx->respbuf_len = sizeof(respbuf);
    ctx->respbuf_used = 0;
    ctx->scanner = scanner;
    ctx->sstate = SBUF_FREE;
    saved_ctx = ctx;
    return TRUE;
}

VSWITCH_CTX *hostprotocol_schedule(HostProtocolCtx *ctx, int now)
{
    /*
     * maintain sending buffer
     */
    if (ctx->sstate == SBUF_SENT){
        sendbuf_remove(ctx->sending_len);
        ctx->sstate = SBUF_FREE;
    }

    /*
     * mainatain receiving buffer
     */
    recvbuf_sync();

    /*
     * parsing input command
     */
    if (ctx->cmdstatus == PARSING){
       int c = rcvbuf_getc();
       if (c >= 0){
           if (command_parser_parse(&ctx->parser, c)){
               ctx->cmdstatus = EXECUTING;
               command_executor_init(&ctx->executor, &ctx->parser);
            }
        }
    }

    /*
    * execute command & fill sending buffer with result
    */
    if (ctx->cmdstatus == EXECUTING){
        if (ctx->respbuf_used == 0){
            ctx->respbuf_used = 
                command_exectutor_schedule(&ctx->executor, ctx->respbuf, ctx->respbuf_len);
        }
        if (sendbuf_freelen() >= ctx->respbuf_used){
            if (ctx->respbuf_used > 0){
                sendbuf_add(ctx->respbuf, ctx->respbuf_used);
                ctx->respbuf_used = 0;
            }
            ctx->cmdstatus = COMMAND_EXECUTOR_EXECUTING(&ctx->executor) ?
                             EXECUTING : PARSING;
        }
    }

    /*
     *  entry switch event to sending buffer
     */
    VSWITCH_CTX *sw = NULL;
    if (ctx->cmdstatus != EXECUTING){
        static char evresp[60] = "S ";
        BOOL inDebugMode = option_getValue(OPT_DEBUG).data.boolval;
        int maxlen = inDebugMode ? sizeof(evresp) : 2 + SWITCHSTATE_MAXLEN + 2;
        while (scanner_updatedSwitchNum(ctx->scanner) &&
               sendbuf_freelen() >= maxlen){
            sw = scanner_getUpdatedSwitch(ctx->scanner);
            int len = 2;
            if (inDebugMode){
                evresp[0] = '#';
                len += sw->ops->printlog(sw, evresp + len, maxlen - len);
                memset(evresp + len, ' ', maxlen - len);
                evresp[maxlen - 1] = '\r';
                len = maxlen;
            }else{
                evresp[0] = 'S';
                len += sw->ops->printstate(sw, evresp + len, maxlen - len - 2);
                evresp[len++] = '\r';
                evresp[len++] = '\n';
            }
            sendbuf_add(evresp, len);
            sw->ops->commit(sw);
            ctx->send_time = now;
        }
    }

    /*
     * flash sending buffer
     */
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
