#include "main.h"
#include "gpio.h"
#include "can.h"

#ifndef _MOTOR_HAL_H_
#define _MOTOR_HAL_H_

typedef enum
{
    MOTOR_FB_1 = 0,
    MOTOR_FB_2,
    MOTOR_FB_3,
    MOTOR_UD_1,
    MOTOR_UD_2,
    MOTOR_UD_3,
    MOTOR_B_ARM,
    MOTOR_S_ARM,
    MOTOR_MAX_NUM
} MOTOR_NUM;

extern unsigned int MOTOR_STATION[];
extern void (*MOTOR_BK_ON[])(void);
extern void (*MOTOR_BK_OFF[])(void);
extern GPIO_PinState (*MOTOR_BK_STAT[])(void);
extern GPIO_PinState (*MOTOR_LIMIT_FLAG[])(void);
extern void (*PUMP_ON[])(void);
extern void (*PUMP_OFF[])(void);
extern GPIO_PinState (*PUMP_STAT[])(void);
extern void (*DRUM_ON[])(void);
extern void (*DRUM_OFF[])(void);
extern GPIO_PinState (*DRUM_STAT[])(void);

/*
 ===============================================================================
                 ##### communication functions #####
 ===============================================================================
 */
HAL_StatusTypeDef motor_hal_can_init(void);
HAL_StatusTypeDef motor_wait_en(void);
HAL_StatusTypeDef motor_send_data(unsigned int s_id, unsigned char *s_buf, unsigned char s_size);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

///////////////////////////// OUT /////////////////////////////////
void PUMP_1_ON(void);
void PUMP_2_ON(void);
void PUMP_3_ON(void);
void PUMP_4_ON(void);
void PUMP_1_OFF(void);
void PUMP_2_OFF(void);
void PUMP_3_OFF(void);
void PUMP_4_OFF(void);
// GPIO_PinState PUMP_1_GET(void);
// GPIO_PinState PUMP_2_GET(void);
// GPIO_PinState PUMP_3_GET(void);
// GPIO_PinState PUMP_4_GET(void);
// drum
// void DRUM_1_ON(void);HAL_GPIO_WritePin(PUMP_1_GPIO_Port,PUMP_1_Pin,GPIO_PIN_RESET);
// void DRUM_2_ON(void);HAL_GPIO_WritePin(PUMP_2_GPIO_Port,PUMP_2_Pin,GPIO_PIN_RESET);
// void DRUM_3_ON(void);HAL_GPIO_WritePin(PUMP_3_GPIO_Port,PUMP_3_Pin,GPIO_PIN_RESET);
// void DRUM_1_OFF(void);HAL_GPIO_WritePin(PUMP_1_GPIO_Port,PUMP_1_Pin,GPIO_PIN_SET);
// void DRUM_2_OFF(void);HAL_GPIO_WritePin(PUMP_2_GPIO_Port,PUMP_2_Pin,GPIO_PIN_SET);
// void DRUM_3_OFF(void);HAL_GPIO_WritePin(PUMP_3_GPIO_Port,PUMP_3_Pin,GPIO_PIN_SET);
// void DRUM_1_GET(void);HAL_GPIO_ReadPin(PUMP_1_GPIO_Port,PUMP_1_Pin);
// void DRUM_2_GET(void);HAL_GPIO_ReadPin(PUMP_2_GPIO_Port,PUMP_2_Pin);
// void DRUM_3_GET(void);HAL_GPIO_ReadPin(PUMP_3_GPIO_Port,PUMP_3_Pin);
///////////////////////////// IN  /////////////////////////////////

GPIO_PinState MOTOR_FB1_F_FLAG(void);
GPIO_PinState MOTOR_FB1_B_FLAG(void);
GPIO_PinState MOTOR_FB2_F_FLAG(void);
GPIO_PinState MOTOR_FB2_B_FLAG(void);
GPIO_PinState MOTOR_FB3_F_FLAG(void);
GPIO_PinState MOTOR_FB3_B_FLAG(void);
GPIO_PinState MOTOR_UD1_F_FLAG(void);
GPIO_PinState MOTOR_UD1_B_FLAG(void);
GPIO_PinState MOTOR_UD2_F_FLAG(void);
GPIO_PinState MOTOR_UD2_B_FLAG(void);
GPIO_PinState MOTOR_UD3_F_FLAG(void);
GPIO_PinState MOTOR_UD3_B_FLAG(void);
GPIO_PinState MOTOR_BARM_F_FLAG(void);
GPIO_PinState MOTOR_BARM_B_FLAG(void);
GPIO_PinState MOTOR_SARM_U_FLAG(void);
GPIO_PinState MOTOR_SARM_D_FLAG(void);

// Emergency stop button
GPIO_PinState EMERGENCY_KEY_FLAG(void);
// Anti-collision strip
GPIO_PinState ANTI_COLLISION_FLAG(void);

#endif
