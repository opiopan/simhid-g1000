/*
 * scanner.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "switch.h"
#include "switchgroup.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SWITCHSTATE_MAXLEN (5 + 1 + 4)

typedef enum {
    SWG_LEFT_RE = 0,
    SWG_LEFT_SW,
    SWG_BOTTOM_SW,
    SWG_RIGHT_RE,
    SWG_RIGHT_SW,
} SWG_INDEX;


typedef struct {
    int lastupdate_hp;
    int lastupdate_lp;
    VSWITCH_DIRTY_BUF dbuf;
} SCANNER_CTX;

BOOL scanner_init(SCANNER_CTX *ctx, SPI_HandleTypeDef *spi, int now);
BOOL scanner_schedule(SCANNER_CTX* ctx, int now);

int scanner_getswgnum(SCANNER_CTX* ctx);
VSWG_CTX* scanner_getswg(SCANNER_CTX* ctx, int index);

inline int scanner_updatedSwitchNum(SCANNER_CTX* ctx)
{
    return VSBUF_LENGTH(&ctx->dbuf);
};

inline VSWITCH_CTX* scanner_getUpdatedSwitch(SCANNER_CTX* ctx)
{
    return vsbuf_pop(&ctx->dbuf);
}

#ifdef __cplusplus
}
#endif
