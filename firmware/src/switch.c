/*
 * switch.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include "switch.h"
#include "olog.h"

/*========================================================
 Dirty ring buffer
========================================================*/
void vsbuf_init(VSWITCH_DIRTY_BUF *ctx, VSWITCH_CTX **buf, int bufsize)
{
    ctx->top = 0;
    ctx->bottom = 0;
    ctx->buf = buf;
    ctx->mask = bufsize - 1;
}

void vsbuf_push(VSWITCH_DIRTY_BUF *ctx, VSWITCH_CTX *data)
{
    if (data->isDirty){
        return;
    }
    if (ctx->bottom - ctx->top > ctx->mask){
        VSWITCH_CTX* sw = vsbuf_pop(ctx);
        sw->ops->commit(sw);
    }

    data->isDirty = TRUE;
    ctx->buf[ctx->bottom & ctx->mask] = data;
    ctx->bottom++;
}

VSWITCH_CTX *vsbuf_pop(VSWITCH_DIRTY_BUF *ctx)
{
    if (VSBUF_LENGTH(ctx) <= 0){
        return NULL;
    }
    VSWITCH_CTX* data = ctx->buf[ctx->top & ctx->mask];
    data->isDirty = FALSE;
    ctx->top++;
    return data;
}

/*========================================================
Simple switch
========================================================*/
static BOOL ss_update(VSWITCH_CTX *ctx, uint32_t data, int now)
{
    SimpleSwitchCtx *rctx = (SimpleSwitchCtx *)ctx;
    BOOL current = ((data & rctx->mask) != 0) ^ rctx->reversePolarity;
    BOOL rc = FALSE;
    if (rctx->indoubt){
        if (rctx->state == current){
            rctx->indoubt = FALSE;
        }else if (now - rctx->changeTime >= rctx->filterPeriod){
            rctx->indoubt = FALSE;
            rctx->state = current;
            rc = TRUE;
        }
    }else if (rctx->state != current){
        rctx->indoubt = TRUE;
        rctx->changeTime = now;
    }
    return rc;
}

static int32_t ss_getmask(VSWITCH_CTX* ctx)
{
    SimpleSwitchCtx *rctx = (SimpleSwitchCtx *)ctx;
    return rctx->mask;
}

static int32_t ss_getvalue(VSWITCH_CTX *ctx)
{
    SimpleSwitchCtx *rctx = (SimpleSwitchCtx *)ctx;
    return rctx->state;
}

static void ss_commit(VSWITCH_CTX *ctx)
{
}

static void ss_printlog(VSWITCH_CTX *ctx)
{
    SimpleSwitchCtx *rctx = (SimpleSwitchCtx *)ctx;
    olog_printf(
        "%.8s: %.8s                                   \r", 
        rctx->description, rctx->state ? "ON" : "OFF");
}

const VSWITCH_OPS SimpleSwitchOps = {
    .update = ss_update,
    .getmask = ss_getmask,
    .getvalue = ss_getvalue,
    .commit = ss_commit,
    .printlog = ss_printlog,
};

/*========================================================
Rotary Encoder 
========================================================*/
typedef struct
{
    RE_STATE next;
    int delta;
} RESM;

static RESM RESM_UNKNOWN[] = {
    {RE_STOP1, 0},
    {RE_UNKNOWN, 0},
    {RE_UNKNOWN, 0},
    {RE_STOP2, 0}};

static RESM RESM_STOP1[] = {
    {RE_STOP1, 0},
    {RE_CW1, 0},
    {RE_CCW1, 0},
    {RE_STOP2, 0}};

static RESM RESM_CW1[] = {
    {RE_STOP1, 0},
    {RE_CW1, 0},
    {RE_UNKNOWN, 0},
    {RE_STOP2, 1}};

static RESM RESM_CCW1[] = {
    {RE_STOP1, 0},
    {RE_UNKNOWN, 0},
    {RE_CCW1, 0},
    {RE_STOP2, -1}};

static RESM RESM_STOP2[] = {
    {RE_STOP1, 0},
    {RE_CCW2, 0},
    {RE_CW2, 0},
    {RE_STOP2, 0}};

static RESM RESM_CW2[] = {
    {RE_STOP1, 1},
    {RE_UNKNOWN, 0},
    {RE_CW2, 0},
    {RE_STOP2, 0}};

static RESM RESM_CCW2[] = {
    {RE_STOP1, -1},
    {RE_CCW2, 0},
    {RE_UNKNOWN, 0},
    {RE_STOP2, 0}};

static RESM *resm[] = {
    RESM_UNKNOWN,
    RESM_STOP1,
    RESM_CW1,
    RESM_CCW1,
    RESM_STOP2,
    RESM_CW2,
    RESM_CCW2,
};

static BOOL re_update(VSWITCH_CTX *ctx, uint32_t data, int now)
{
    RotaryEncoderCtx* rctx = (RotaryEncoderCtx*)ctx;
    BOOL a = (data & rctx->amask) != 0;
    BOOL b = (data & rctx->bmask) != 0;
    int pat = a | b << 1;
    int delta = resm[rctx->state][pat].delta;
    rctx->state = resm[rctx->state][pat].next;
    rctx->counter += delta;
    return delta != 0;
}

static int32_t re_getmask(VSWITCH_CTX *ctx)
{
    RotaryEncoderCtx *rctx = (RotaryEncoderCtx *)ctx;
    return rctx->amask | rctx->bmask;
}

static int32_t re_getvalue(VSWITCH_CTX *ctx)
{
    RotaryEncoderCtx *rctx = (RotaryEncoderCtx *)ctx;
    return rctx->counter - rctx->last;
}

static void re_commit(VSWITCH_CTX *ctx)
{
    RotaryEncoderCtx *rctx = (RotaryEncoderCtx *)ctx;
    rctx->last = rctx->counter;
}

static void re_printlog(VSWITCH_CTX *ctx)
{
    RotaryEncoderCtx *rctx = (RotaryEncoderCtx *)ctx;
    static const char *pat[] = {
        "|oooooo                          |",
        "| oooooo                         |",
        "|  oooooo                        |",
        "|   oooooo                       |",
        "|    oooooo                      |",
        "|     oooooo                     |",
        "|      oooooo                    |",
        "|       oooooo                   |",
        "|        oooooo                  |",
        "|         oooooo                 |",
        "|          oooooo                |",
        "|           oooooo               |",
        "|            oooooo              |",
        "|             oooooo             |",
        "|              oooooo            |",
        "|               oooooo           |",
        "|                oooooo          |",
        "|                 oooooo         |",
        "|                  oooooo        |",
        "|                   oooooo       |",
        "|                    oooooo      |",
        "|                     oooooo     |",
        "|                      oooooo    |",
        "|                       oooooo   |",
        "|                        oooooo  |",
        "|                         oooooo |",
        "|                          oooooo|",
        "|o                          ooooo|",
        "|oo                          oooo|",
        "|ooo                          ooo|",
        "|oooo                          oo|",
        "|ooooo                          o|",
    };
    olog_printf(
        "%.8s: %8d %s\r", 
        rctx->description, rctx->counter, pat[rctx->counter & 0x1f]);
}

const VSWITCH_OPS RotaryEncoderOps = {
    .update = re_update,
    .getmask = re_getmask,
    .getvalue = re_getvalue,
    .commit = re_commit,
    .printlog = re_printlog,
};
