/*
 * hrtimer.c
 *  Author: opiopan@gmail.com
 */

#include "hrtimer.h"

TIM_HandleTypeDef *hrtimer_handler;

void hrtimer_init(TIM_HandleTypeDef *handler)
{
    hrtimer_handler = handler;
    HAL_TIM_Base_Start(hrtimer_handler);
}
