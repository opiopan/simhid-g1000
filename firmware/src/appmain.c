/*
 * scanswitch.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include <string.h>
#include "usbd_cdc_if.h"
#include "project.h"
#include "appmain.h"
#include "olog.h"
#include "hrtimer.h"
#include "scanner.h"
#include "hostprotocol.h"
#include "option.h"

APPCONF* appconf;

volatile int appmaintest;

/*===============================================================
 * utility functions
 *==============================================================*/
static SCANNER_CTX scanner;
static HostProtocolCtx protocol;

SCANNER_CTX* app_getScannerCtx()
{
    return &scanner;
}

HostProtocolCtx* app_getProtocolCtx()
{
    return &protocol;
}

/*===============================================================
 * main logic
 *==============================================================*/
void runApp(APPCONF *conf)
{
    appconf = conf;
    olog_init();
    olog_printf("\n\n");
    olog_printf("=============================================\n");
    olog_printf("  SimHID G1000\n");
    olog_printf("=============================================\n");

    HAL_Delay(500);

    hrtimer_init(conf->hrtimer);
    int now = HRTIMER_GETTIME();
    scanner_init(&scanner, appconf->spi, now);
    hostprotocol_init(&protocol, &scanner);

    BOOL inLogmode = option_getValue(OPT_EVLOG).data.boolval;
    OLOG_LOGI("appmain: Log mode [%s]", inLogmode ? "ON" : "OFF");

    // main loop
    int lastprint = now;
    VSWITCH_CTX* updated_sw = NULL;
    while (1){
        now = HRTIMER_GETTIME();
        scanner_schedule(&scanner, now);
        VSWITCH_CTX* sw = hostprotocol_schedule(&protocol, now);
        updated_sw = sw ? sw : updated_sw;
        if (now - lastprint >= 50 * 1000 && sw){
            lastprint = now;
            inLogmode = option_getValue(OPT_EVLOG).data.boolval;
            if (inLogmode){
                static char buf[60];
                int len = sw->ops->printlog(sw, buf, sizeof(buf) - 1);
                memset(buf + len, ' ', sizeof(buf) - 1 - len);
                buf[sizeof(buf) - 2] = '\r';
                buf[sizeof(buf) - 1] = '\0';
                olog_printf("    %s", buf);
                sw = NULL;
            }
        }
    }
}
