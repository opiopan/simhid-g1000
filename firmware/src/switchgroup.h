/*
 * switchgroup.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#pragma once
#include "main.h"
#include "project.h"
#include "switch.h"
#include "mcp23s18.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*========================================================
 Virutalized Switch Group Operations
========================================================*/
typedef struct vswgroup_ctx_t VSWG_CTX;

typedef struct {
    BOOL (*init)(VSWG_CTX* ctx);
    BOOL (*scan)(VSWG_CTX* ctx, VSWITCH_DIRTY_BUF* dbuf, int now);
} VSWG_OPS;

typedef struct vswgroup_ctx_t{
    const VSWG_OPS* ops;
    const char* name;
    int swnum;
    VSWITCH_CTX** sws;
    uint32_t (*filter)(VSWG_CTX* ctx, uint32_t data);
    BOOL is_valid;
    int lastdata;
    int scantime;
}VSWG_CTX;

#define SWG_GET_SWITCHNUM(swg) (swg)->swnum
#define SWG_GET_SWITCH(swg, ix) (swg)->sws[ix]

/*========================================================
 Switch group accessing via MCP23S18
========================================================*/
typedef struct{
    VSWG_CTX common;
    GPIO_TypeDef* cs_port;
    int cs_pin;
    MCP23S18_CTX exp;
}EXTSWG_CTX;

void extswg_initenv(SPI_HandleTypeDef *spi);

extern const VSWG_OPS EXTSWG_OPS;

#define DEFEXTSWG(NM, SWS, NUM, CS, FILTER) {\
    .common = (VSWG_CTX){\
        .ops = &EXTSWG_OPS,\
        .name = NM,\
        .swnum = (NUM),\
        .sws = (SWS), \
        .filter = (FILTER)},\
    .cs_port = CS##_GPIO_Port,\
    .cs_pin = CS##_Pin}

/*========================================================
 Switch group direct connected with MCU
========================================================*/
typedef struct{
    GPIO_TypeDef* port;
    int pin;
    int bitpos;
}DIRECTSWG_GPIO;

#define DEFDSWGGPIO(GPIO, BITS) (DIRECTSWG_GPIO){\
    .port = GPIO##_GPIO_Port,\
    .pin = GPIO##_Pin,\
    .bitpos = BITS}

typedef struct{
    VSWG_CTX common;
    DIRECTSWG_GPIO* gpios;
    int gpionum;
}DIRECTSWG_CTX;

extern const VSWG_OPS DIRECTSWG_OPS;

#define DEFDIRECTSWG(NM, SWS, SWNUM, GPIOS, GPIONUM, FILTER) {\
    .common = (VSWG_CTX){\
        .ops = &DIRECTSWG_OPS,\
        .name = NM,\
        .swnum = (SWNUM),\
        .sws = (SWS),\
        .filter = (FILTER)},\
    .gpios = (GPIOS),\
    .gpionum = (GPIONUM)}

#ifdef __cplusplus
}
#endif
