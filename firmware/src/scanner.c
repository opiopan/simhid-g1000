/*
 * scanner.c
 *  Author: Hiroshi Murayama <opiopan@gmail.com>
 */

#include "project.h"
#include "switch.h"
#include "switchgroup.h"
#include "scanner.h"

#define LPF 10000
#define HP_PERIOD 300   /* usec */
#define LP_PERIOD 5000  /* usec */

/*========================================================
 Switch Group Definition: Right Rotaly Encoders
========================================================*/
static DIRECTSWG_GPIO RRE_GPIOS[] = {
    DEFDSWGGPIO(EC5A, 0),
    DEFDSWGGPIO(EC5B, 1),
    DEFDSWGGPIO(EC6XA, 2),
    DEFDSWGGPIO(EC6XB, 3),
    DEFDSWGGPIO(EC6YA, 4),
    DEFDSWGGPIO(EC6YB, 5),
    DEFDSWGGPIO(EC7XA, 6),
    DEFDSWGGPIO(EC7XB, 7),
    DEFDSWGGPIO(EC7YA, 8),
    DEFDSWGGPIO(EC7YB, 9),
    DEFDSWGGPIO(EC8A, 10),
    DEFDSWGGPIO(EC8B, 11),
    DEFDSWGGPIO(EC9XA, 12),
    DEFDSWGGPIO(EC9XB, 13),
    DEFDSWGGPIO(EC9YA, 14),
    DEFDSWGGPIO(EC9YB, 15),
    DEFDSWGGPIO(AUX2U, 16),
    DEFDSWGGPIO(AUX2P, 17),
    DEFDSWGGPIO(AUX2D, 18),
};

DEF_ROTARYENC(EC5, "EC5", 1<<0, 1<<1);
DEF_ROTARYENC(EC6X, "EC6X", 1<<2, 1<<3);
DEF_ROTARYENC(EC6Y, "EC6Y", 1 << 4, 1 << 5);
DEF_ROTARYENC(EC7X, "EC7X", 1 << 6, 1 << 7);
DEF_ROTARYENC(EC7Y, "EC7Y", 1 << 8, 1 << 9);
DEF_ROTARYENC(EC8, "EC8", 1 << 10, 1 << 11);
DEF_ROTARYENC(EC9X, "EC9X", 1 << 12, 1 << 13);
DEF_ROTARYENC(EC9Y, "EC9Y", 1 << 14, 1 << 15);
DEF_SIMPLESW(AUX2U, "AUX2-Up", 1 << 16, FALSE, LPF);
DEF_SIMPLESW(AUX2P, "AUX2-Push", 1 << 17, FALSE, LPF);
DEF_SIMPLESW(AUX2D, "AUX2-Down", 1 << 18, FALSE, LPF);

static VSWITCH_CTX *RRE_SWS[] = {
    (VSWITCH_CTX *)&EC5,
    (VSWITCH_CTX *)&EC6X,
    (VSWITCH_CTX *)&EC6Y,
    (VSWITCH_CTX *)&EC7X,
    (VSWITCH_CTX *)&EC7Y,
    (VSWITCH_CTX *)&EC8,
    (VSWITCH_CTX *)&EC9X,
    (VSWITCH_CTX *)&EC9Y,
    (VSWITCH_CTX *)&AUX2U,
    (VSWITCH_CTX *)&AUX2P,
    (VSWITCH_CTX *)&AUX2D,
};

static DIRECTSWG_CTX SWG_RRE = DEFDIRECTSWG(
    "Right Rotary Encoders",
    RRE_SWS, sizeof(RRE_SWS) / sizeof(RRE_SWS[0]),
    RRE_GPIOS, sizeof(RRE_GPIOS)/ sizeof(RRE_GPIOS[0]), NULL);

/*========================================================
 Switch Group Definition: Right Switch Cluster (Expander 4)
========================================================*/
DEF_SIMPLESW(SW30, "SW30", 1 << 0, TRUE, LPF);
DEF_SIMPLESW(SW32, "SW32", 1 << 1, TRUE, LPF);
DEF_SIMPLESW(SW34, "SW34", 1 << 2, TRUE, LPF);
DEF_SIMPLESW(EC8L, "EC8-Left", 1 << 3, TRUE, LPF);
DEF_SIMPLESW(EC8P, "EC8-Push", 1 << 4, TRUE, LPF);
DEF_SIMPLESW(EC8U, "EC8-Up", 1 << 5, TRUE, LPF);
DEF_SIMPLESW(EC8D, "EC8-Down", 1 << 6, TRUE, LPF);
DEF_SIMPLESW(EC8R, "EC8-Right", 1 << 7, TRUE, LPF);
DEF_SIMPLESW(EC7P, "EC7-Push", 1 << 8, TRUE, LPF);
DEF_SIMPLESW(EC6P, "EC6-Push", 1 << 9, TRUE, LPF);
DEF_SIMPLESW(SW28, "SW28", 1 << 10, TRUE, LPF);
DEF_SIMPLESW(EC5P, "EC5-Push", 1 << 11, TRUE, LPF);
DEF_SIMPLESW(SW29, "SW29", 1 << 12, TRUE, LPF);
DEF_SIMPLESW(SW31, "SW31", 1 << 13, TRUE, LPF);
DEF_SIMPLESW(SW33, "SW33", 1 << 14, TRUE, LPF);
DEF_SIMPLESW(EC9P, "EC9-Push", 1 << 15, TRUE, LPF);

static VSWITCH_CTX *RSW_SWS[] = {
    (VSWITCH_CTX *)&EC5P,
    (VSWITCH_CTX *)&EC6P,
    (VSWITCH_CTX *)&EC7P,
    (VSWITCH_CTX *)&EC8P,
    (VSWITCH_CTX *)&EC8U,
    (VSWITCH_CTX *)&EC8D,
    (VSWITCH_CTX *)&EC8R,
    (VSWITCH_CTX *)&EC8L,
    (VSWITCH_CTX *)&EC9P,
    (VSWITCH_CTX *)&SW28,
    (VSWITCH_CTX *)&SW29,
    (VSWITCH_CTX *)&SW30,
    (VSWITCH_CTX *)&SW31,
    (VSWITCH_CTX *)&SW32,
    (VSWITCH_CTX *)&SW33,
    (VSWITCH_CTX *)&SW34,
};

static uint32_t SWG_RSW_Filter(VSWG_CTX* ctx, uint32_t data)
{
    // Note: 
    //  All switch except aux2 are pulled up to VDD.
    //  That means filter funtion must handle data as negative logic.
    uint32_t EC8_DIRS = 1 << 3 | 1 << 5 | 1 << 6 | 1 << 7;
    uint32_t EC8_PUSH = 1 << 4;
    #define DIRON(bits) (EC8_DIRS & ~(1<<(bits)))

    uint32_t dirs = data & EC8_DIRS;
    uint32_t mask = EC8_DIRS | EC8_PUSH;

    if (dirs == 0){
        mask = EC8_DIRS;
    }else if (dirs == DIRON(3) || dirs == DIRON(5) || 
              dirs == DIRON(6) || dirs == DIRON(7)){
        mask = EC8_PUSH;
    }

    return data | mask;
}

static EXTSWG_CTX SWG_RSW = 
    DEFEXTSWG("Right Buttons", RSW_SWS, 
              sizeof(RSW_SWS)/ sizeof(RSW_SWS[0]), EXT4CS, SWG_RSW_Filter);

/*========================================================
 Switch Group Definition: Bottom Switch Cluster (Expander 3)
========================================================*/
DEF_SIMPLESW(SW21, "SW21", 1 << 0, TRUE, LPF);
DEF_SIMPLESW(SW19, "SW19", 1 << 1, TRUE, LPF);
DEF_SIMPLESW(SW20, "SW20", 1 << 2, TRUE, LPF);
DEF_SIMPLESW(SW18, "SW18", 1 << 3, TRUE, LPF);
DEF_SIMPLESW(SW17, "SW17", 1 << 4, TRUE, LPF);
DEF_SIMPLESW(SW16, "SW16", 1 << 5, TRUE, LPF);
DEF_SIMPLESW(SW15, "SW15", 1 << 6, TRUE, LPF);
/* skip */
/* skip */
/* skip */
DEF_SIMPLESW(SW26, "SW26", 1 << 10, TRUE, LPF);
DEF_SIMPLESW(SW25, "SW25", 1 << 11, TRUE, LPF);
DEF_SIMPLESW(SW24, "SW24", 1 << 12, TRUE, LPF);
DEF_SIMPLESW(SW23, "SW23", 1 << 13, TRUE, LPF);
DEF_SIMPLESW(SW22, "SW22", 1 << 14, TRUE, LPF);
/* skip */

static VSWITCH_CTX *BSW_SWS[] = {
    (VSWITCH_CTX *)&SW15,
    (VSWITCH_CTX *)&SW16,
    (VSWITCH_CTX *)&SW17,
    (VSWITCH_CTX *)&SW18,
    (VSWITCH_CTX *)&SW19,
    (VSWITCH_CTX *)&SW20,
    (VSWITCH_CTX *)&SW21,
    (VSWITCH_CTX *)&SW22,
    (VSWITCH_CTX *)&SW23,
    (VSWITCH_CTX *)&SW24,
    (VSWITCH_CTX *)&SW25,
    (VSWITCH_CTX *)&SW26,
};

static EXTSWG_CTX SWG_BSW =
    DEFEXTSWG("Bottom Buttons", BSW_SWS, 
              sizeof(BSW_SWS) / sizeof(BSW_SWS[0]), EXT3CS, NULL);

/*========================================================
 Switch Group Definition: Left Rotary Encoders  (Expander 1)
========================================================*/
DEF_ROTARYENC(EC1, "EC1", 1 << 11, 1 << 12);
DEF_ROTARYENC(EC2X, "EC2X", 1 << 9, 1 << 10);
DEF_ROTARYENC(EC2Y, "EC2Y", 1 << 7, 1 << 8);
DEF_ROTARYENC(EC3, "EC3", 1 << 14, 1 << 13);
DEF_ROTARYENC(EC4X, "EC4X", 1 << 3, 1 << 2);
DEF_ROTARYENC(EC4Y, "EC4Y", 1 << 1, 1 << 0);
DEF_SIMPLESW(EC1P, "EC1-Push", 1 << 4, TRUE, LPF);
DEF_SIMPLESW(EC2P, "EC2-Push", 1 << 6, TRUE, LPF);
DEF_SIMPLESW(EC3P, "EC3-Push", 1 << 15, TRUE, LPF);
DEF_SIMPLESW(SW1, "SW1", 1 << 5, TRUE, LPF);

static VSWITCH_CTX *LRE_SWS[] = {
    (VSWITCH_CTX *)&EC1,
    (VSWITCH_CTX *)&EC2X,
    (VSWITCH_CTX *)&EC2Y,
    (VSWITCH_CTX *)&EC3,
    (VSWITCH_CTX *)&EC4X,
    (VSWITCH_CTX *)&EC4Y,
    (VSWITCH_CTX *)&EC1P,
    (VSWITCH_CTX *)&EC2P,
    (VSWITCH_CTX *)&EC3P,
    (VSWITCH_CTX *)&SW1,
};

static EXTSWG_CTX SWG_LRE = 
    DEFEXTSWG("Left Rotary Encoders", LRE_SWS, 
              sizeof(LRE_SWS) / sizeof(LRE_SWS[0]), EXT1CS, NULL);

/*========================================================
 Switch Group Definition: Left Switch Cluster  (Expander 2)
========================================================*/
DEF_SIMPLESW(SW7, "SW7", 1 << 0, TRUE, LPF);
DEF_SIMPLESW(SW9, "SW9", 1 << 1, TRUE, LPF);
DEF_SIMPLESW(SW11, "SW11", 1 << 2, TRUE, LPF);
DEF_SIMPLESW(SW13, "SW13", 1 << 3, TRUE, LPF);
DEF_SIMPLESW(SW5, "SW5", 1 << 4, TRUE, LPF);
DEF_SIMPLESW(SW3, "SW3", 1 << 5, TRUE, LPF);
DEF_SIMPLESW(SW2, "SW2", 1 << 6, TRUE, LPF);
DEF_SIMPLESW(SW4, "SW4", 1 << 7, TRUE, LPF);
DEF_SIMPLESW(AUX1U, "AUX1-Up", 1 << 8, TRUE, LPF);
DEF_SIMPLESW(AUX1P, "AUX1-Push", 1 << 9, TRUE, LPF);
DEF_SIMPLESW(AUX1D, "AUX1-Down", 1 << 10, TRUE, LPF);
DEF_SIMPLESW(SW12, "SW12", 1 << 11, TRUE, LPF);
DEF_SIMPLESW(SW10, "SW10", 1 << 12, TRUE, LPF);
DEF_SIMPLESW(SW8, "SW8", 1 << 13, TRUE, LPF);
DEF_SIMPLESW(SW6, "SW6", 1 << 14, TRUE, LPF);
DEF_SIMPLESW(EC4P, "EC4-Push", 1 << 15, TRUE, LPF);

static VSWITCH_CTX *LSW_SWS[] = {
    (VSWITCH_CTX *)&SW2,
    (VSWITCH_CTX *)&SW3,
    (VSWITCH_CTX *)&SW4,
    (VSWITCH_CTX *)&SW5,
    (VSWITCH_CTX *)&SW6,
    (VSWITCH_CTX *)&SW7,
    (VSWITCH_CTX *)&SW8,
    (VSWITCH_CTX *)&SW9,
    (VSWITCH_CTX *)&SW10,
    (VSWITCH_CTX *)&SW11,
    (VSWITCH_CTX *)&SW12,
    (VSWITCH_CTX *)&SW13,
    (VSWITCH_CTX *)&AUX1U,
    (VSWITCH_CTX *)&AUX1P,
    (VSWITCH_CTX *)&AUX1D,
    (VSWITCH_CTX *)&EC4P,
};

static EXTSWG_CTX SWG_LSW =
    DEFEXTSWG("Left Buttons", LSW_SWS, 
              sizeof(LSW_SWS) / sizeof(LSW_SWS[0]), EXT2CS, NULL);

/*========================================================
 Switch Group Prioritizing
========================================================*/
static VSWG_CTX* HP_SWGS[] = {
    (VSWG_CTX*)&SWG_RRE,
    (VSWG_CTX*)&SWG_LRE,
};
#define HP_SWGS_NUM (sizeof(HP_SWGS) / sizeof(HP_SWGS[0]))
#define HP_SWGS_PERIOD  (HP_PERIOD / HP_SWGS_NUM)

static VSWG_CTX* LP_SWGS[] = {
    (VSWG_CTX*)&SWG_RSW,
    (VSWG_CTX*)&SWG_LSW,
    (VSWG_CTX*)&SWG_BSW,
};
#define LP_SWGS_NUM (sizeof(LP_SWGS) / sizeof(LP_SWGS[0]))
#define LP_SWGS_PERIOD  (LP_PERIOD / LP_SWGS_NUM)

/*========================================================
 Switces to enter log mode
========================================================*/
static VSWITCH_CTX* logmode_sws[] = {
    (VSWITCH_CTX*)&AUX1P,
};

/*========================================================
 Ring buffer to indicate switches in dirty state
========================================================*/
static VSWITCH_CTX* dirtyBuf[64];

/*========================================================
 scanner operations
========================================================*/
BOOL scanner_init(SCANNER_CTX *ctx, SPI_HandleTypeDef *spi, int now)
{
    vsbuf_init(&ctx->dbuf, dirtyBuf, sizeof(dirtyBuf) / sizeof(dirtyBuf[0]));
    extswg_initenv(spi);

    BOOL failed = FALSE;

    for (int i = 0; i < HP_SWGS_NUM; i++){
        VSWG_CTX* swg = HP_SWGS[i];
        failed |= swg->ops->init(swg);
        swg->ops->scan(swg, &ctx->dbuf, now);
    }
    for (int i = 0; i < LP_SWGS_NUM; i++){
        VSWG_CTX* swg = LP_SWGS[i];
        failed |= swg->ops->init(swg);
        swg->ops->scan(swg, &ctx->dbuf, now);
    }

    BOOL logmode = TRUE;
    for (int i = 0; i < sizeof(logmode_sws) / sizeof(logmode_sws[0]); i++){
        VSWITCH_CTX* sw = logmode_sws[i];
        logmode = logmode && sw->ops->getvalue(sw);
    }
    if (logmode){
        ctx->bootmode = BOOTMODE_LOG;
    }

    return !failed;
}

BOOL scanner_schedule(SCANNER_CTX *ctx, int now)
{
    for (int i = 0; i < HP_SWGS_NUM; i++){
        VSWG_CTX* swg = HP_SWGS[i];
        if (now - swg->scantime >= HP_SWGS_PERIOD){
            return swg->ops->scan(swg, &ctx->dbuf, now);
        }
    }
    for (int i = 0; i < LP_SWGS_NUM; i++){
        VSWG_CTX* swg = LP_SWGS[i];
        if (now - swg->scantime >= LP_SWGS_PERIOD){
            return swg->ops->scan(swg, &ctx->dbuf, now);
        }
    }
    return TRUE;
}
