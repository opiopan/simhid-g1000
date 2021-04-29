/*
 * scanswitch.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include "project.h"
#include "appmain.h"
#include "olog.h"
#include "hrtimer.h"
#include "scanner.h"

APPCONF* appconf;

/*===============================================================
 * main logic
 *==============================================================*/
void runApp(APPCONF *conf)
{
    appconf = conf;
    olog_init();
    olog_printf("\n\n=============================================\n");
    olog_printf("  SimHID G1000\n");
    olog_printf("=============================================\n");

    hrtimer_init(conf->hrtimer);
    int now = HRTIMER_GETTIME();
    SCANNER_CTX scanner;
    scanner_init(&scanner, appconf->spi, now);

    // main loop
    int lastprint = now;
    while (1)
    {
        now = HRTIMER_GETTIME();
        scanner_schedule(&scanner, now);
        if (now - lastprint >= 50 * 1000){
            lastprint = now;
            VSWITCH_CTX *ctx;
            while ((ctx = scanner_getUpdatedSwitch(&scanner)) != NULL){
                ctx->ops->printlog(ctx);
            }
        }
    }
}
