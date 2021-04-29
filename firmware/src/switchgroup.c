/*
 * switchgroup.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include "project.h"
#include "olog.h"
#include "switchgroup.h"

/*========================================================
 Switch group accessing via MCP23S18
========================================================*/
static SPI_HandleTypeDef *extswg_spi;

void extswg_initenv(SPI_HandleTypeDef *spi)
{
    extswg_spi = spi;
}

static BOOL extswg_init(VSWG_CTX *ctx)
{
    EXTSWG_CTX *rctx = (EXTSWG_CTX *)ctx;

    int mask = 0;
    for (int i = 0; i < rctx->common.swnum; i++){
        VSWITCH_CTX* sw = rctx->common.sws[i];
        mask |= sw->ops->getmask(sw);
    }

    BOOL rc = mcp23s18_init(&rctx->exp, extswg_spi, rctx->cs_port, rctx->cs_pin, mask);
    if (rc){
        OLOG_LOGI("switchgroup: MCP23S13 has been initialized for %s", rctx->common.name);
    }else{
        OLOG_LOGE("switchgroup: Initializing MCP23S13 for %s failed", rctx->common.name);
    }

    return rc;
}

static BOOL extswg_scan(VSWG_CTX *ctx, VSWITCH_DIRTY_BUF *dbuf, int now)
{
    EXTSWG_CTX *rctx = (EXTSWG_CTX *)ctx;

    int state;
    if (!MCP23S18_GETGPIO(&rctx->exp, (int*)&state)){
        return FALSE;
    }
    if (rctx->common.filter){
        state = rctx->common.filter(ctx, state);
    }

    for (int i = 0; i < rctx->common.swnum; i++){
        VSWITCH_CTX *sw = rctx->common.sws[i];
        if (sw->ops->update(sw, state, now)){
            vsbuf_push(dbuf, sw);
        }
    }

    rctx->common.scantime = now;
    return TRUE;
}

const VSWG_OPS EXTSWG_OPS = {
    .init = extswg_init,
    .scan = extswg_scan,
};

/*========================================================
 Switch group direct connected with MCU
========================================================*/
static BOOL directswg_init(VSWG_CTX *ctx)
{
    return TRUE;
}

static BOOL directswg_scan(VSWG_CTX *ctx, VSWITCH_DIRTY_BUF *dbuf, int now)
{
    DIRECTSWG_CTX *rctx = (DIRECTSWG_CTX *)ctx;

    int state = 0;
    for (int i = 0; i < rctx->gpionum; i++){
        DIRECTSWG_GPIO* gpio = &rctx->gpios[i];
        state |= HAL_GPIO_ReadPin(gpio->port, gpio->pin) << gpio->bitpos;
    }
    if (rctx->common.filter){
        state = rctx->common.filter(ctx, state);
    }

    for (int i = 0; i < rctx->common.swnum; i++){
        VSWITCH_CTX *sw = rctx->common.sws[i];
        if (sw->ops->update(sw, state, now)){
            vsbuf_push(dbuf, sw);
        }
    }

    rctx->common.scantime = now;
    return TRUE;
}

const VSWG_OPS DIRECTSWG_OPS = {
    .init = directswg_init,
    .scan = directswg_scan,
};
