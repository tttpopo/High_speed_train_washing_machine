/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f103xe.h"
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
#define MOTO_BK_4_Pin GPIO_PIN_2
#define MOTO_BK_4_GPIO_Port GPIOE
#define MOTO_BK_5_Pin GPIO_PIN_3
#define MOTO_BK_5_GPIO_Port GPIOE
#define MOTO_BK_6_Pin GPIO_PIN_4
#define MOTO_BK_6_GPIO_Port GPIOE
#define MOTO_BK_7_Pin GPIO_PIN_5
#define MOTO_BK_7_GPIO_Port GPIOE
#define MOTO_BK_8_Pin GPIO_PIN_6
#define MOTO_BK_8_GPIO_Port GPIOE
#define MOTO_BK_3_Pin GPIO_PIN_0
#define MOTO_BK_3_GPIO_Port GPIOC
#define MOTO_BK_2_Pin GPIO_PIN_1
#define MOTO_BK_2_GPIO_Port GPIOC
#define MOTO_BK_1_Pin GPIO_PIN_2
#define MOTO_BK_1_GPIO_Port GPIOC
#define SIGN_LIGHT_4_Pin GPIO_PIN_3
#define SIGN_LIGHT_4_GPIO_Port GPIOC
#define SIGN_LIGHT_3_Pin GPIO_PIN_0
#define SIGN_LIGHT_3_GPIO_Port GPIOA
#define SIGN_LIGHT_2_Pin GPIO_PIN_1
#define SIGN_LIGHT_2_GPIO_Port GPIOA
#define SIGN_LIGHT_1_Pin GPIO_PIN_2
#define SIGN_LIGHT_1_GPIO_Port GPIOA
#define ADC_1_Pin GPIO_PIN_4
#define ADC_1_GPIO_Port GPIOA
#define ADC_2_Pin GPIO_PIN_5
#define ADC_2_GPIO_Port GPIOA
#define ADC_3_Pin GPIO_PIN_6
#define ADC_3_GPIO_Port GPIOA
#define ADC_4_Pin GPIO_PIN_7
#define ADC_4_GPIO_Port GPIOA
#define ADC_5_Pin GPIO_PIN_4
#define ADC_5_GPIO_Port GPIOC
#define ADC_6_Pin GPIO_PIN_5
#define ADC_6_GPIO_Port GPIOC
#define ADC_7_Pin GPIO_PIN_0
#define ADC_7_GPIO_Port GPIOB
#define ADC_8_Pin GPIO_PIN_1
#define ADC_8_GPIO_Port GPIOB
#define MOTO_FB1_LIMIT_F_Pin GPIO_PIN_2
#define MOTO_FB1_LIMIT_F_GPIO_Port GPIOB
#define MOTO_FB1_LIMIT_B_Pin GPIO_PIN_7
#define MOTO_FB1_LIMIT_B_GPIO_Port GPIOE
#define MOTO_FB2_LIMIT_F_Pin GPIO_PIN_8
#define MOTO_FB2_LIMIT_F_GPIO_Port GPIOE
#define MOTO_FB2_LIMIT_B_Pin GPIO_PIN_9
#define MOTO_FB2_LIMIT_B_GPIO_Port GPIOE
#define MOTO_FB3_LIMIT_F_Pin GPIO_PIN_10
#define MOTO_FB3_LIMIT_F_GPIO_Port GPIOE
#define MOTO_FB3_LIMIT_B_Pin GPIO_PIN_11
#define MOTO_FB3_LIMIT_B_GPIO_Port GPIOE
#define MOTO_UD1_LIMIT_U_Pin GPIO_PIN_12
#define MOTO_UD1_LIMIT_U_GPIO_Port GPIOE
#define MOTO_UD1_LIMIT_D_Pin GPIO_PIN_13
#define MOTO_UD1_LIMIT_D_GPIO_Port GPIOE
#define MOTO_UD2_LIMIT_U_Pin GPIO_PIN_14
#define MOTO_UD2_LIMIT_U_GPIO_Port GPIOE
#define MOTO_UD2_LIMIT_D_Pin GPIO_PIN_15
#define MOTO_UD2_LIMIT_D_GPIO_Port GPIOE
#define MOTO_UD3_LIMIT_D_Pin GPIO_PIN_12
#define MOTO_UD3_LIMIT_D_GPIO_Port GPIOB
#define MOTO_UD3_LIMIT_U_Pin GPIO_PIN_13
#define MOTO_UD3_LIMIT_U_GPIO_Port GPIOB
#define MOTO_BARM_LIMIT_F_Pin GPIO_PIN_14
#define MOTO_BARM_LIMIT_F_GPIO_Port GPIOB
#define MOTO_BARM_LIMIT_B_Pin GPIO_PIN_15
#define MOTO_BARM_LIMIT_B_GPIO_Port GPIOB
#define MOTO_SARM_LIMIT_U_Pin GPIO_PIN_8
#define MOTO_SARM_LIMIT_U_GPIO_Port GPIOD
#define MOTO_SARM_LIMIT_D_Pin GPIO_PIN_9
#define MOTO_SARM_LIMIT_D_GPIO_Port GPIOD
#define EMERG_KEY_Pin GPIO_PIN_8
#define EMERG_KEY_GPIO_Port GPIOA
#define OU_RES_5_Pin GPIO_PIN_0
#define OU_RES_5_GPIO_Port GPIOD
#define SPOT_LIGHT_2_Pin GPIO_PIN_3
#define SPOT_LIGHT_2_GPIO_Port GPIOD
#define SPOT_LIGHT_1_Pin GPIO_PIN_7
#define SPOT_LIGHT_1_GPIO_Port GPIOD
#define DRUM_1_Pin GPIO_PIN_3
#define DRUM_1_GPIO_Port GPIOB
#define DRUM_2_Pin GPIO_PIN_4
#define DRUM_2_GPIO_Port GPIOB
#define DRUM_3_Pin GPIO_PIN_5
#define DRUM_3_GPIO_Port GPIOB
#define OU_RES_4_Pin GPIO_PIN_6
#define OU_RES_4_GPIO_Port GPIOB
#define PUMP_1_Pin GPIO_PIN_8
#define PUMP_1_GPIO_Port GPIOB
#define PUMP_2_Pin GPIO_PIN_9
#define PUMP_2_GPIO_Port GPIOB
#define PUMP_3_Pin GPIO_PIN_0
#define PUMP_3_GPIO_Port GPIOE
#define PUMP_4_Pin GPIO_PIN_1
#define PUMP_4_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define BRUSH_START_DELAY_TIME 500 // brush one-button start interval
#define FB_SPEED 20000               // Translation brush group running speed
#define UD_SPEED 12000               // Lifting brush group running speed
#define B_ARM_TARG_PULSE -50000      // Big arm end point pulse value
#define B_AEM_SPEED 500            // Big arm running speed
#define S_ARM_TARG_PULSE 50000      // Small arm end point pulse value
#define S_ARM_SPEED 500            // Small arm running speed

#define MOTOR_BK_DELAY 110          // Brake delay time when starting the motor

#define MOTOR_ERR_MAX 1000          // Number of motor failure retries

#define MOTOR_CAN_OUT_TIME 100      // ms
#define MOTOR_CAN_RETRY_COUNT 0     // retry count

#define SET 0                       // Define proximity sensor in place status

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
