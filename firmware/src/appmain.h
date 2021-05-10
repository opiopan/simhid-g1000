/*
 * scanswitch.h
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#ifndef APPMAIN__
#define APPMAIN__

#include "main.h"
#include "project.h"
#include "scanner.h"
#include "hostprotocol.h"

typedef struct {
    TIM_HandleTypeDef* hrtimer;
    SPI_HandleTypeDef* spi;
} APPCONF;

void runApp(APPCONF* conf);

SCANNER_CTX *app_getScannerCtx();
HostProtocolCtx *app_getProtocolCtx();

#endif