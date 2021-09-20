/*
 * switch.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "project.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*========================================================
 Virutalized Switch Operations
========================================================*/
typedef struct vswitch_ctx_t VSWITCH_CTX;

typedef struct {
    BOOL (*update)(VSWITCH_CTX* ctx, uint32_t data, int now);
    int32_t (*getmask)(VSWITCH_CTX* ctx);
    int32_t (*getvalue)(VSWITCH_CTX *ctx);
    int32_t (*getrawvalue)(VSWITCH_CTX *ctx);
    void (*commit)(VSWITCH_CTX *ctx);
    int (*printstate)(VSWITCH_CTX *ctx, char *buf, int len);
    int (*printdef)(VSWITCH_CTX *ctx, char *buf, int len);
    int (*printlog)(VSWITCH_CTX *ctx, char *buf, int len);
} VSWITCH_OPS;

typedef struct vswitch_ctx_t{
    const VSWITCH_OPS* ops;
    const char* name;
    BOOL isDirty;
}VSWITCH_CTX;

/*========================================================
 Dirty ring buffer
========================================================*/
typedef struct {
    int mask;
    int top;
    int bottom;
    VSWITCH_CTX** buf;
}VSWITCH_DIRTY_BUF;

void vsbuf_init(VSWITCH_DIRTY_BUF* ctx, VSWITCH_CTX** buf, int bufsize); // bufsize must be power of 2
void vsbuf_push(VSWITCH_DIRTY_BUF *ctx, VSWITCH_CTX *data);
VSWITCH_CTX* vsbuf_pop(VSWITCH_DIRTY_BUF *ctx);
#define VSBUF_LENGTH(C) ((C)->bottom - (C)->top)

/*========================================================
Simple switch 
========================================================*/
typedef struct {
    VSWITCH_CTX common;
    const char* description;
    int mask;
    BOOL state;
    BOOL indoubt;
    int changeTime;
    BOOL reversePolarity;
    int  filterPeriod;
}SimpleSwitchCtx;

const VSWITCH_OPS SimpleSwitchOps;

#define DEF_SIMPLESW(N, D, M, R, P) static SimpleSwitchCtx N = {\
    .common = (VSWITCH_CTX){\
        .ops = &SimpleSwitchOps,\
        .name = #N,\
    },\
    .description = (D),\
    .mask = (M),\
    .reversePolarity = (R),\
    .filterPeriod = (P)}

/*========================================================
Rotary Encoder 
========================================================*/
typedef enum{
    RE_UNKNOWN = 0,
    RE_STOP1,
    RE_CW1,
    RE_CCW1,
    RE_STOP2,
    RE_CW2,
    RE_CCW2,
} RE_STATE;

typedef struct {
    VSWITCH_CTX common;
    const char* name;
    const char* description;
    int amask;
    int bmask;
    RE_STATE state;
    int  counter;
    int last;
}RotaryEncoderCtx;

const VSWITCH_OPS RotaryEncoderOps;

#define DEF_ROTARYENC(N, D, A, B) static RotaryEncoderCtx N = {\
    .common = (VSWITCH_CTX){\
        .ops = &RotaryEncoderOps,\
        .name = #N,\
    },\
    .description = (D),\
    .amask = (A),\
    .bmask = (B)}

#ifdef __cplusplus
}
#endif
