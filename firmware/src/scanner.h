/*
 * scanner.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "switch.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BOOTMODE_LOG    1

typedef struct {
    int bootmode;
    int lastupdate_hp;
    int lastupdate_lp;
    VSWITCH_DIRTY_BUF dbuf;
} SCANNER_CTX;

BOOL scanner_init(SCANNER_CTX *ctx, SPI_HandleTypeDef *spi, int now);
BOOL scanner_schedule(SCANNER_CTX* ctx, int now);

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
