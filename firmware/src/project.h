/*
 * project.h
 *
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef TRUE
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#endif /* PROJECT_H_ */
