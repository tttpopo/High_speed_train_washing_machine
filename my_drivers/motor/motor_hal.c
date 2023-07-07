#include "usart.h"
#include "elog.h"
#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor_hal.h"
#include "stdio.h"
#include <string.h>

///////////////////////////// OUT /////////////////////////////////
// breake
void MOTOR_FB1_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_1_GPIO_Port, MOTO_BK_1_Pin, GPIO_PIN_SET);
}
void MOTOR_FB2_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_2_GPIO_Port, MOTO_BK_2_Pin, GPIO_PIN_SET);
}
void MOTOR_FB3_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_3_GPIO_Port, MOTO_BK_3_Pin, GPIO_PIN_SET);
}
void MOTOR_UD1_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_4_GPIO_Port, MOTO_BK_4_Pin, GPIO_PIN_SET);
}
void MOTOR_UD2_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_5_GPIO_Port, MOTO_BK_5_Pin, GPIO_PIN_SET);
}
void MOTOR_UD3_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_6_GPIO_Port, MOTO_BK_6_Pin, GPIO_PIN_SET);
}
void MOTOR_BARM_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_7_GPIO_Port, MOTO_BK_7_Pin, GPIO_PIN_SET);
}
void MOTOR_SARM_BK_ON()
{
    HAL_GPIO_WritePin(MOTO_BK_8_GPIO_Port, MOTO_BK_8_Pin, GPIO_PIN_SET);
}

void MOTOR_FB1_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_1_GPIO_Port, MOTO_BK_1_Pin, GPIO_PIN_RESET);
}
void MOTOR_FB2_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_2_GPIO_Port, MOTO_BK_2_Pin, GPIO_PIN_RESET);
}
void MOTOR_FB3_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_3_GPIO_Port, MOTO_BK_3_Pin, GPIO_PIN_RESET);
}
void MOTOR_UD1_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_4_GPIO_Port, MOTO_BK_4_Pin, GPIO_PIN_RESET);
}
void MOTOR_UD2_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_5_GPIO_Port, MOTO_BK_5_Pin, GPIO_PIN_RESET);
}
void MOTOR_UD3_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_6_GPIO_Port, MOTO_BK_6_Pin, GPIO_PIN_RESET);
}
void MOTOR_BARM_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_7_GPIO_Port, MOTO_BK_7_Pin, GPIO_PIN_RESET);
}
void MOTOR_SARM_BK_OFF()
{

    HAL_GPIO_WritePin(MOTO_BK_8_GPIO_Port, MOTO_BK_8_Pin, GPIO_PIN_RESET);
}

GPIO_PinState MOTOR_FB1_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_1_GPIO_Port, MOTO_BK_1_Pin);
}
GPIO_PinState MOTOR_UD1_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_2_GPIO_Port, MOTO_BK_2_Pin);
}
GPIO_PinState MOTOR_FB2_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_3_GPIO_Port, MOTO_BK_3_Pin);
}
GPIO_PinState MOTOR_UD2_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_4_GPIO_Port, MOTO_BK_4_Pin);
}
GPIO_PinState MOTOR_FB3_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_5_GPIO_Port, MOTO_BK_5_Pin);
}
GPIO_PinState MOTOR_UD3_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_6_GPIO_Port, MOTO_BK_6_Pin);
}
GPIO_PinState MOTOR_BARM_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_7_GPIO_Port, MOTO_BK_7_Pin);
}
GPIO_PinState MOTOR_SARM_BK_GET()
{
    return HAL_GPIO_ReadPin(MOTO_BK_8_GPIO_Port, MOTO_BK_8_Pin);
}

// pump

/*
PUMP1 - water pump
PUMP2 - Detergent pump
PUMP3 - spray pump
PUMP4 - brush water pump
PUMP5 - Detergent suction pump
PUMP6 - vacuum pump(sewage pump)
PUMP7 - solenoid valve
*/

void PUMP_1_ON()
{
    HAL_GPIO_WritePin(PUMP_1_GPIO_Port, PUMP_1_Pin, GPIO_PIN_SET);
}
void PUMP_2_ON()
{
    HAL_GPIO_WritePin(PUMP_2_GPIO_Port, PUMP_2_Pin, GPIO_PIN_SET);
}
void PUMP_3_ON()
{
    HAL_GPIO_WritePin(PUMP_3_GPIO_Port, PUMP_3_Pin, GPIO_PIN_SET);
}
void PUMP_4_ON()
{
    HAL_GPIO_WritePin(PUMP_4_GPIO_Port, PUMP_4_Pin, GPIO_PIN_SET);
}
void PUMP_5_ON()
{
    HAL_GPIO_WritePin(PUMP_5_GPIO_Port, PUMP_5_Pin, GPIO_PIN_SET);
}
void PUMP_6_ON()
{
    HAL_GPIO_WritePin(PUMP_6_GPIO_Port, PUMP_6_Pin, GPIO_PIN_SET);
}
void PUMP_7_ON()
{
    HAL_GPIO_WritePin(PUMP_7_GPIO_Port, PUMP_7_Pin, GPIO_PIN_SET);
}

void PUMP_1_OFF()
{
    HAL_GPIO_WritePin(PUMP_1_GPIO_Port, PUMP_1_Pin, GPIO_PIN_RESET);
}
void PUMP_2_OFF()
{
    HAL_GPIO_WritePin(PUMP_2_GPIO_Port, PUMP_2_Pin, GPIO_PIN_RESET);
}
void PUMP_3_OFF()
{
    HAL_GPIO_WritePin(PUMP_3_GPIO_Port, PUMP_3_Pin, GPIO_PIN_RESET);
}
void PUMP_4_OFF()
{
    HAL_GPIO_WritePin(PUMP_4_GPIO_Port, PUMP_4_Pin, GPIO_PIN_RESET);
}
void PUMP_5_OFF()
{
    HAL_GPIO_WritePin(PUMP_5_GPIO_Port, PUMP_5_Pin, GPIO_PIN_RESET);
}
void PUMP_6_OFF()
{
    HAL_GPIO_WritePin(PUMP_6_GPIO_Port, PUMP_6_Pin, GPIO_PIN_RESET);
}
void PUMP_7_OFF()
{
    HAL_GPIO_WritePin(PUMP_7_GPIO_Port, PUMP_7_Pin, GPIO_PIN_RESET);
}

GPIO_PinState PUMP_1_GET()
{
    return HAL_GPIO_ReadPin(PUMP_1_GPIO_Port, PUMP_1_Pin);
}
GPIO_PinState PUMP_2_GET()
{
    return HAL_GPIO_ReadPin(PUMP_2_GPIO_Port, PUMP_2_Pin);
}
GPIO_PinState PUMP_3_GET()
{
    return HAL_GPIO_ReadPin(PUMP_3_GPIO_Port, PUMP_3_Pin);
}
GPIO_PinState PUMP_4_GET()
{
    return HAL_GPIO_ReadPin(PUMP_4_GPIO_Port, PUMP_4_Pin);
}
GPIO_PinState PUMP_5_GET()
{
    return HAL_GPIO_ReadPin(PUMP_5_GPIO_Port, PUMP_5_Pin);
}
GPIO_PinState PUMP_6_GET()
{
    return HAL_GPIO_ReadPin(PUMP_6_GPIO_Port, PUMP_6_Pin);
}
GPIO_PinState PUMP_7_GET()
{
    return HAL_GPIO_ReadPin(PUMP_7_GPIO_Port, PUMP_7_Pin);
}

// drum

void DRUM_1_ON(void)
{
    HAL_GPIO_WritePin(DRUM_1_GPIO_Port, DRUM_1_Pin, GPIO_PIN_SET);
}
void DRUM_2_ON(void)
{
    HAL_GPIO_WritePin(DRUM_2_GPIO_Port, DRUM_2_Pin, GPIO_PIN_SET);
}
void DRUM_3_ON(void)
{
    HAL_GPIO_WritePin(DRUM_3_GPIO_Port, DRUM_3_Pin, GPIO_PIN_SET);
}
void DRUM_1_OFF(void)
{
    HAL_GPIO_WritePin(DRUM_1_GPIO_Port, DRUM_1_Pin, GPIO_PIN_RESET);
}
void DRUM_2_OFF(void)
{
    HAL_GPIO_WritePin(DRUM_2_GPIO_Port, DRUM_2_Pin, GPIO_PIN_RESET);
}
void DRUM_3_OFF(void)
{
    HAL_GPIO_WritePin(DRUM_3_GPIO_Port, DRUM_3_Pin, GPIO_PIN_RESET);
}
GPIO_PinState DRUM_1_GET(void)
{
    return HAL_GPIO_ReadPin(DRUM_1_GPIO_Port, DRUM_1_Pin);
}
GPIO_PinState DRUM_2_GET(void)
{
    return HAL_GPIO_ReadPin(DRUM_2_GPIO_Port, DRUM_2_Pin);
}
GPIO_PinState DRUM_3_GET(void)
{
    return HAL_GPIO_ReadPin(DRUM_3_GPIO_Port, DRUM_3_Pin);
}
///////////////////////////// IN  /////////////////////////////////
// BRUSH 1 Forward and backward, up and down,Proximity sensor (limit switch)

GPIO_PinState MOTOR_FB1_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_F_GPIO_Port, MOTO_FB1_LIMIT_F_Pin);
}
GPIO_PinState MOTOR_FB1_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_B_GPIO_Port, MOTO_FB1_LIMIT_B_Pin);
}
GPIO_PinState MOTOR_FB2_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_F_GPIO_Port, MOTO_FB2_LIMIT_F_Pin);
}
GPIO_PinState MOTOR_FB2_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_B_GPIO_Port, MOTO_FB2_LIMIT_B_Pin);
}
GPIO_PinState MOTOR_FB3_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_F_GPIO_Port, MOTO_FB3_LIMIT_F_Pin);
}
GPIO_PinState MOTOR_FB3_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_B_GPIO_Port, MOTO_FB3_LIMIT_B_Pin);
}
GPIO_PinState MOTOR_UD1_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_U_GPIO_Port, MOTO_UD1_LIMIT_U_Pin);
}
GPIO_PinState MOTOR_UD1_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_D_GPIO_Port, MOTO_UD1_LIMIT_D_Pin);
}
GPIO_PinState MOTOR_UD2_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_U_GPIO_Port, MOTO_UD2_LIMIT_U_Pin);
}
GPIO_PinState MOTOR_UD2_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_D_GPIO_Port, MOTO_UD2_LIMIT_D_Pin);
}
GPIO_PinState MOTOR_UD3_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_U_GPIO_Port, MOTO_UD3_LIMIT_U_Pin);
}
GPIO_PinState MOTOR_UD3_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_D_GPIO_Port, MOTO_UD3_LIMIT_D_Pin);
}
GPIO_PinState MOTOR_BARM_F_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_F_GPIO_Port, MOTO_BARM_LIMIT_F_Pin);
}
GPIO_PinState MOTOR_BARM_B_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_B_GPIO_Port, MOTO_BARM_LIMIT_B_Pin);
}
GPIO_PinState MOTOR_SARM_U_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_U_GPIO_Port, MOTO_SARM_LIMIT_U_Pin);
}
GPIO_PinState MOTOR_SARM_D_FLAG()
{
    return HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_D_GPIO_Port, MOTO_SARM_LIMIT_D_Pin);
}

// Emergency stop button
GPIO_PinState EMERGENCY_KEY_FLAG()
{
    return HAL_GPIO_ReadPin(IN_RES_4_GPIO_Port, IN_RES_4_Pin);
}

GPIO_PinState ANTI_COLLISION_FLAG()
{
    return HAL_GPIO_ReadPin(IN_RES_1_GPIO_Port, IN_RES_1_Pin);
}

/*
 ===============================================================================
                 ##### communication functions #####
 ===============================================================================
 */

char MOTOR_REC_FLAG_BUF[10] = {0};
unsigned char MOTOR_REC_FLAG = 0;
// unsigned int MOTOR_ERR_ID = 0;
uint32_t maillbox = CAN_TX_MAILBOX0;
unsigned char can_rx_buf[10] = {0};
CAN_RxHeaderTypeDef can_rx_head;
CAN_TxHeaderTypeDef can_tx_head = {
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA};

/// @brief init can
/// @return NONE
HAL_StatusTypeDef motor_hal_can_init()
{
    HAL_StatusTypeDef ret;

    ret = HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    if (ret == HAL_OK)
    {
        elog_d("CAN-INIT", "Enable interrupts ok!");
    }
    else
    {
        elog_e("CAN-INIT", "Enable interrupts error!");
        return HAL_ERROR;
    }

    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

    ret = HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
    if (ret == HAL_OK)
    {
        elog_d("CAN-INIT", "configfilter ok!");
    }
    else
    {
        elog_e("CAN-INIT", "configfilter error!");
        return HAL_ERROR;
    }

    ret = HAL_CAN_Start(&hcan);
    if (ret == HAL_OK)
    {
        elog_d("CAN-INIT", "can start ok!");
    }
    else
    {
        elog_e("CAN-INIT", "can start error!");
        return HAL_ERROR;
    }

    return HAL_OK;
}

/// @brief Wait for the motor to be powered on,default timeout is five seconds
/// @return
HAL_StatusTypeDef motor_wait_en()
{
    // int out_time = 0;
    // MOTOR_REC_FLAG = 0;
    // while (out_time < 500)
    // {
    //     out_time++;
    //     if (MOTOR_REC_FLAG)
    //     {
    //         elog_d("MOTOR", "motor BOOTUP!");
    //         return HAL_OK;
    //     }
    //     vTaskDelay(10 / portTICK_RATE_MS);
    // }
    // elog_d("MOTOR", "motor BOOTUP time out!");
    // return HAL_ERROR;

    vTaskDelay(4000 / portTICK_RATE_MS);
    return HAL_OK;
}

/// @brief Send data to the motor
/// @param s_id id
/// @param s_buf data
/// @param s_size data length
/// @return
HAL_StatusTypeDef motor_send_data(unsigned int s_id, unsigned char *s_buf, unsigned char s_size)
{
    unsigned int out_time = 0;
    unsigned char retry_count = 0;
    HAL_StatusTypeDef ret;
    can_tx_head.StdId = s_id;
    can_tx_head.DLC = s_size;
    MOTOR_REC_FLAG = 0;

RETRY:
    out_time = 0;
    if (retry_count > MOTOR_CAN_RETRY_COUNT)
    {
        // elog_e("MOTOR", "moto %d time out!", s_id - 0x600);
        return HAL_ERROR;
    }

    ret = HAL_CAN_AddTxMessage(&hcan, &can_tx_head, s_buf, (uint32_t *)&maillbox);
    if (ret != HAL_OK)
    {
        elog_e("MOTOR", "can send error!");
        retry_count++;
        goto RETRY;
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    while (out_time < MOTOR_CAN_OUT_TIME)
    {
        out_time++;
        // for (char i = 0; i < 10; i++)
        // {
        //     if (MOTOR_REC_FLAG_BUF[i] == s_id - 0x600)
        //     {
        //         memset(MOTOR_REC_FLAG_BUF,0,sizeof(MOTOR_REC_FLAG_BUF));
        //         MOTOR_REC_FLAG_BUF[i] = 0;
        //         return HAL_OK;
        //     }
        // }
        if (MOTOR_REC_FLAG)
        {
            return HAL_OK;
        }
        vTaskDelay(1 / portTICK_RATE_MS);
    }
    retry_count++;
    goto RETRY;
}

/// @brief rx callback
/// @param hcan
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    //static unsigned char cnt_flag = 0;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can_rx_head, can_rx_buf) == HAL_OK)
    {
        // memset(MOTOR_REC_FLAG_BUF, 0, sizeof(MOTOR_REC_FLAG_BUF));
        // MOTOR_REC_FLAG_BUF[cnt_flag++] = can_rx_head.StdId - 0x580;
        // cnt_flag %= 10;
        MOTOR_REC_FLAG = 1;
        // if ((can_rx_head.StdId >= (129 + MOTOR_FB_1)) && (can_rx_head.StdId <= (129 + MOTOR_S_ARM)))
        // {
        //     MOTOR_ERR_ID = can_rx_head.StdId;
        // }
        // printf("in---");
        // printf("%x-->", can_rx_head.StdId);
        // for (int i = 0; i < can_rx_head.DLC; i++)
        // {
        //     printf("%x-", can_rx_buf[i]);
        // }
        // printf("\r\n");
    }
}

unsigned int MOTOR_STATION[] = {0X601, 0X602, 0X603, 0X604, 0X605, 0X606, 0X607, 0X608};

void (*MOTOR_BK_ON[])(void) = {
    MOTOR_FB1_BK_ON,
    MOTOR_FB2_BK_ON,
    MOTOR_FB3_BK_ON,
    MOTOR_UD1_BK_ON,
    MOTOR_UD2_BK_ON,
    MOTOR_UD3_BK_ON,
    MOTOR_BARM_BK_ON,
    MOTOR_SARM_BK_ON};

void (*MOTOR_BK_OFF[])(void) = {
    MOTOR_FB1_BK_OFF,
    MOTOR_FB2_BK_OFF,
    MOTOR_FB3_BK_OFF,
    MOTOR_UD1_BK_OFF,
    MOTOR_UD2_BK_OFF,
    MOTOR_UD3_BK_OFF,
    MOTOR_BARM_BK_OFF,
    MOTOR_SARM_BK_OFF};

GPIO_PinState (*MOTOR_BK_STAT[])(void) = {
    MOTOR_FB1_BK_GET,
    MOTOR_FB2_BK_GET,
    MOTOR_FB3_BK_GET,
    MOTOR_UD1_BK_GET,
    MOTOR_UD2_BK_GET,
    MOTOR_UD3_BK_GET,
    MOTOR_BARM_BK_GET,
    MOTOR_SARM_BK_GET};

GPIO_PinState (*MOTOR_LIMIT_FLAG[])(void) = {
    MOTOR_FB1_F_FLAG,
    MOTOR_FB1_B_FLAG,
    MOTOR_FB2_F_FLAG,
    MOTOR_FB2_B_FLAG,
    MOTOR_FB3_F_FLAG,
    MOTOR_FB3_B_FLAG,
    MOTOR_UD1_F_FLAG,
    MOTOR_UD1_B_FLAG,
    MOTOR_UD2_F_FLAG,
    MOTOR_UD2_B_FLAG,
    MOTOR_UD3_F_FLAG,
    MOTOR_UD3_B_FLAG,
    MOTOR_BARM_F_FLAG,
    MOTOR_BARM_B_FLAG,
    MOTOR_SARM_U_FLAG,
    MOTOR_SARM_D_FLAG};

void (*PUMP_ON[])(void) = {PUMP_1_ON, PUMP_2_ON, PUMP_3_ON, PUMP_4_ON, PUMP_5_ON, PUMP_6_ON, PUMP_7_ON};

void (*PUMP_OFF[])(void) = {PUMP_1_OFF, PUMP_2_OFF, PUMP_3_OFF, PUMP_4_OFF, PUMP_5_OFF, PUMP_6_OFF, PUMP_7_OFF};

GPIO_PinState (*PUMP_STAT[])(void) = {PUMP_1_GET, PUMP_2_GET, PUMP_3_GET, PUMP_4_GET, PUMP_5_GET, PUMP_6_GET, PUMP_7_GET};

void (*DRUM_ON[])(void) = {DRUM_1_ON, DRUM_2_ON, DRUM_3_ON};

void (*DRUM_OFF[])(void) = {DRUM_1_OFF, DRUM_2_OFF, DRUM_3_OFF};

GPIO_PinState (*DRUM_STAT[])(void) = {DRUM_1_GET, DRUM_2_GET, DRUM_3_GET};
