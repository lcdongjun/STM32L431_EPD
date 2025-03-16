/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
//#include "arm_math.h"
#include "delay.h"
#include "sys.h"
#include "malloc.h"	 

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
#define T_SQW_Pin GPIO_PIN_13
#define T_SQW_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_1
#define KEY2_GPIO_Port GPIOH
#define KEY1_WKUP_Pin GPIO_PIN_0
#define KEY1_WKUP_GPIO_Port GPIOA
#define BEEP_Pin GPIO_PIN_1
#define BEEP_GPIO_Port GPIOA
#define CHK_VU_Pin GPIO_PIN_4
#define CHK_VU_GPIO_Port GPIOA
#define EPD_SCL_Pin GPIO_PIN_5
#define EPD_SCL_GPIO_Port GPIOA
#define EPD_BUSY_Pin GPIO_PIN_6
#define EPD_BUSY_GPIO_Port GPIOA
#define EPD_SDI_Pin GPIO_PIN_7
#define EPD_SDI_GPIO_Port GPIOA
#define EPD_RES_Pin GPIO_PIN_0
#define EPD_RES_GPIO_Port GPIOB
#define EPD_DC_Pin GPIO_PIN_1
#define EPD_DC_GPIO_Port GPIOB
#define EPD_CS_Pin GPIO_PIN_2
#define EPD_CS_GPIO_Port GPIOB
#define EN_CHK_VU_Pin GPIO_PIN_12
#define EN_CHK_VU_GPIO_Port GPIOB
#define F_CLK_Pin GPIO_PIN_13
#define F_CLK_GPIO_Port GPIOB
#define F_SO_Pin GPIO_PIN_14
#define F_SO_GPIO_Port GPIOB
#define F_SI_Pin GPIO_PIN_15
#define F_SI_GPIO_Port GPIOB
#define F_CS_Pin GPIO_PIN_8
#define F_CS_GPIO_Port GPIOA
#define KEY_OK_Pin GPIO_PIN_11
#define KEY_OK_GPIO_Port GPIOA
#define KEY_R_Pin GPIO_PIN_12
#define KEY_R_GPIO_Port GPIOA
#define KEY_L_Pin GPIO_PIN_15
#define KEY_L_GPIO_Port GPIOA
#define BT_CDS_Pin GPIO_PIN_3
#define BT_CDS_GPIO_Port GPIOB
#define BT_LINK_Pin GPIO_PIN_4
#define BT_LINK_GPIO_Port GPIOB
#define BT_BRTS_Pin GPIO_PIN_5
#define BT_BRTS_GPIO_Port GPIOB
#define AHT_SCL_Pin GPIO_PIN_6
#define AHT_SCL_GPIO_Port GPIOB
#define AHT_SDA_Pin GPIO_PIN_7
#define AHT_SDA_GPIO_Port GPIOB
#define T_SCL_Pin GPIO_PIN_8
#define T_SCL_GPIO_Port GPIOB
#define T_SDA_Pin GPIO_PIN_9
#define T_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
