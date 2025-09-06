/*
 * mcp23s18.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 * 
 *  Note: Only bank 0 register configuration is supported
 */

#include "project.h" 
#include "mcp23s18.h"

#define SPITIMEOUT 500

static inline void enableChip(MCP23S18_CTX *ctx)
{
    HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_RESET);
}

static inline void disableChip(MCP23S18_CTX *ctx)
{
    HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_SET);
}

BOOL mcp23s18_init(MCP23S18_CTX *ctx, SPI_HandleTypeDef *spi,
                   GPIO_TypeDef *cs_port, int cs_pin, int inputmask)
{
    ctx->spi = spi;
    ctx->cs_port = cs_port;
    ctx->cs_pin = cs_pin;
    ctx->inputmask = inputmask;

    BOOL failed = FALSE;

    int conf = 0;
    failed |= !mcp23s18_send8bits(ctx, MCP23S18_REG_IOCON, conf);
    int check;
    failed |= !mcp23s18_recv8bits(ctx, MCP23S18_REG_IOCON, &check);
    failed |= (conf != check);
    failed |= !mcp23s18_send16bits(ctx, MCP23S18_REG_IODIRA, ctx->inputmask);

    return !failed;
}

BOOL mcp23s18_send8bits(MCP23S18_CTX *ctx, int addr, int data)
{
    ctx->sbuf[0] = MCP23S18_WRITEOP;
    ctx->sbuf[1] = addr & 0xff;
    ctx->sbuf[2] = data & 0xff;
    
    enableChip(ctx);
    if (HAL_SPI_TransmitReceive(ctx->spi, ctx->sbuf, ctx->rbuf, 3, SPITIMEOUT)){
        disableChip(ctx);
        return FALSE;
    }
    disableChip(ctx);
    return TRUE;
}

BOOL mcp23s18_send16bits(MCP23S18_CTX *ctx, int addr, int data)
{
    ctx->sbuf[0] = MCP23S18_WRITEOP;
    ctx->sbuf[1] = addr & 0xff;
    ctx->sbuf[2] = data & 0xff;
    ctx->sbuf[3] = (data & 0xff00) >> 8;

    enableChip(ctx);
    if (HAL_SPI_TransmitReceive(ctx->spi, ctx->sbuf, ctx->rbuf, 4, SPITIMEOUT)){
        disableChip(ctx);
        return FALSE;
    }
    disableChip(ctx);
    return TRUE;
}

BOOL mcp23s18_recv8bits(MCP23S18_CTX *ctx, int addr, int *data)
{
    ctx->sbuf[0] = MCP23S18_READOP;
    ctx->sbuf[1] = addr & 0xff;
    ctx->sbuf[2] = 0;
    
    enableChip(ctx);
    if (HAL_SPI_TransmitReceive(ctx->spi, ctx->sbuf, ctx->rbuf, 3, SPITIMEOUT)){
        disableChip(ctx);
        return FALSE;
    }
    disableChip(ctx);
    *data = ctx->rbuf[2];
    return TRUE;
}

BOOL mcp23s18_recv16bits(MCP23S18_CTX *ctx, int addr, int *data)
{
    ctx->sbuf[0] = MCP23S18_READOP;
    ctx->sbuf[1] = addr & 0xff;
    ctx->sbuf[2] = 0;
    ctx->sbuf[3] = 0;

    enableChip(ctx);
    if (HAL_SPI_TransmitReceive(ctx->spi, ctx->sbuf, ctx->rbuf, 4, SPITIMEOUT)){
        disableChip(ctx);
        return FALSE;
    }
    disableChip(ctx);
    *data = ctx->rbuf[2] | (ctx->rbuf[3] << 8);
    return TRUE;
}
