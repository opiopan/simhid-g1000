/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EC6YA_Pin GPIO_PIN_13
#define EC6YA_GPIO_Port GPIOC
#define EC6XB_Pin GPIO_PIN_14
#define EC6XB_GPIO_Port GPIOC
#define EC6XA_Pin GPIO_PIN_15
#define EC6XA_GPIO_Port GPIOC
#define EC7YA_Pin GPIO_PIN_2
#define EC7YA_GPIO_Port GPIOA
#define EXT4CS_Pin GPIO_PIN_3
#define EXT4CS_GPIO_Port GPIOA
#define EC7YB_Pin GPIO_PIN_4
#define EC7YB_GPIO_Port GPIOA
#define EXT3CS_Pin GPIO_PIN_0
#define EXT3CS_GPIO_Port GPIOB
#define EXT2CS_Pin GPIO_PIN_1
#define EXT2CS_GPIO_Port GPIOB
#define EXT1CS_Pin GPIO_PIN_2
#define EXT1CS_GPIO_Port GPIOB
#define EC7XA_Pin GPIO_PIN_10
#define EC7XA_GPIO_Port GPIOB
#define EC7XB_Pin GPIO_PIN_12
#define EC7XB_GPIO_Port GPIOB
#define EC8A_Pin GPIO_PIN_13
#define EC8A_GPIO_Port GPIOB
#define EC8B_Pin GPIO_PIN_14
#define EC8B_GPIO_Port GPIOB
#define EC9XA_Pin GPIO_PIN_15
#define EC9XA_GPIO_Port GPIOB
#define EC9XB_Pin GPIO_PIN_8
#define EC9XB_GPIO_Port GPIOA
#define EC9YA_Pin GPIO_PIN_9
#define EC9YA_GPIO_Port GPIOA
#define EC9YB_Pin GPIO_PIN_10
#define EC9YB_GPIO_Port GPIOA
#define AUX2D_Pin GPIO_PIN_4
#define AUX2D_GPIO_Port GPIOB
#define AUX2P_Pin GPIO_PIN_5
#define AUX2P_GPIO_Port GPIOB
#define AUX2U_Pin GPIO_PIN_6
#define AUX2U_GPIO_Port GPIOB
#define EC5B_Pin GPIO_PIN_7
#define EC5B_GPIO_Port GPIOB
#define EC5A_Pin GPIO_PIN_8
#define EC5A_GPIO_Port GPIOB
#define EC6YB_Pin GPIO_PIN_9
#define EC6YB_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
