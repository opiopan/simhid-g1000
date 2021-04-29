/*
 * mcp23s18.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 * 
 *  Note: Only bank 0 register configuration is supported
 */

#pragma once
#include "project.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MCP23S18_WRITEOP        0x40
#define MCP23S18_READOP         0x41

#define MCP23S18_REG_IODIRA     0x00
#define MCP23S18_REG_IODIRB     0x01
#define MCP23S18_REG_IPOLA      0x02
#define MCP23S18_REG_IPOLB      0x03
#define MCP23S18_REG_GPINTENA   0x04
#define MCP23S18_REG_GPINTENB   0x05
#define MCP23S18_REG_DEFVALA    0x06
#define MCP23S18_REG_DEFVALB    0x07
#define MCP23S18_REG_INTCONA    0x08
#define MCP23S18_REG_INTCONB    0x09
#define MCP23S18_REG_IOCON      0x0a
#define MCP23S18_REG_GPPUA      0x0c
#define MCP23S18_REG_GPPUB      0x0d
#define MCP23S18_REG_INTFA      0x0e
#define MCP23S18_REG_INTFB      0x0f
#define MCP23S18_REG_INTCAPA    0x10
#define MCP23S18_REG_INTCAPB    0x11
#define MCP23S18_REG_GPIOA      0x12
#define MCP23S18_REG_GPIOB      0x13
#define MCP23S18_REG_OLATA      0x14
#define MCP23S18_REG_OLATB      0x15

#define MCP23S18_CONF_BANK      0x80
#define MCP23S18_CONF_MIRROR    0x40
#define MCP23S18_CONF_SEQOP     0x20
#define MCP23S18_CONF_ODR       0x04
#define MCP23S18_CONF_INTPOL    0x02
#define MCP23S18_CONF_INTCC     0x01

typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *cs_port;
    int cs_pin;
    int inputmask;
    uint8_t sbuf[4];
    uint8_t rbuf[4];
}MCP23S18_CTX;

BOOL mcp23s18_init(MCP23S18_CTX *ctx, SPI_HandleTypeDef *spi,
                   GPIO_TypeDef *cs_port, int cs_pin, int inputmask);

BOOL mcp23s18_send8bits(MCP23S18_CTX *ctx, int addr, int data);
BOOL mcp23s18_send16bits(MCP23S18_CTX *ctx, int addr, int data);
BOOL mcp23s18_recv8bits(MCP23S18_CTX *ctx, int addr, int* data);
BOOL mcp23s18_recv16bits(MCP23S18_CTX *ctx, int addr, int* data);

#define MCP23S18_GETGPIO(ctx, data) mcp23s18_recv16bits(ctx, MCP23S18_REG_GPIOA, data)
#define MCP23S18_PUTGPIO(ctx, data) mcp23s18_send16bits(ctx, MCP23S18_REG_OLATA, data)

#ifdef __cplusplus
}
#endif
