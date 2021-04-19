/*
 * scanswitch.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include "project.h"
#include "appmain.h"
#include "olog.h"
#include "hrtimer.h"
#include "switch.h"

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

    // main loop
    while(1){
        int now = HRTIMER_GETTIME();
    }
}
