/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#define ADXL37x_INT2_Pin GPIO_PIN_0
#define ADXL37x_INT2_GPIO_Port GPIOA
#define ADXL37x_NSS_Pin GPIO_PIN_1
#define ADXL37x_NSS_GPIO_Port GPIOA
#define IIS3DWB_INT2_Pin GPIO_PIN_3
#define IIS3DWB_INT2_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define ADXL37x_INT1_Pin GPIO_PIN_4
#define ADXL37x_INT1_GPIO_Port GPIOC
#define ADXL37x_INT1_EXTI_IRQn EXTI4_IRQn
#define LSM6DSx_INT2_Pin GPIO_PIN_5
#define LSM6DSx_INT2_GPIO_Port GPIOC
#define LSM6DSx_INT2_EXTI_IRQn EXTI9_5_IRQn
#define LSM6DSx_INT1_Pin GPIO_PIN_12
#define LSM6DSx_INT1_GPIO_Port GPIOB
#define LSM6DSx_INT1_EXTI_IRQn EXTI15_10_IRQn
#define IIS3DWB_NSS_Pin GPIO_PIN_14
#define IIS3DWB_NSS_GPIO_Port GPIOB
#define IIS3DWB_INT1_Pin GPIO_PIN_15
#define IIS3DWB_INT1_GPIO_Port GPIOB
#define IIS3DWB_INT1_EXTI_IRQn EXTI15_10_IRQn
#define LED_STATUS_Pin GPIO_PIN_9
#define LED_STATUS_GPIO_Port GPIOA
#define SD_CARD_DETECT_Pin GPIO_PIN_10
#define SD_CARD_DETECT_GPIO_Port GPIOA
#define BUTTON_Pin GPIO_PIN_4
#define BUTTON_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
