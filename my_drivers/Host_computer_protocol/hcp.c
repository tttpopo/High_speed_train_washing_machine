#include "hcp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "stdio.h"
#include "elog.h"
#include "motor_drive.h"
#include "brush.h"
#include "string.h"
#include "dma.h"
#include "rtc.h"

TaskHandle_t hcp_task_handle;
unsigned char hcp_buf[40] = {0};
unsigned char hcp_rec_s = 0;
static unsigned short CRC16_Modbus(unsigned char *pdata, int len)
{
    unsigned short crc = 0xFFFF;
    int i, j;
    for (j = 0; j < len; j++)
    {
        crc = crc ^ pdata[j];
        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x0001) > 0)
            {
                crc = crc >> 1;
                crc = crc ^ 0xa001;
            }
            else
                crc = crc >> 1;
        }
    }
    return crc;
}

/// @brief Collect all information and upload it
extern unsigned char pb_bat_level;
extern unsigned char pb_charge_state;

static void response_beat()
{
    static unsigned char heart_buf[32] = {0x5a, 0x17, 0x10, 0x11};
    brush_get_state(&heart_buf[4]);

    unsigned short crc_code = CRC16_Modbus(heart_buf, sizeof(heart_buf) - 2);
    heart_buf[27] = pb_bat_level;
    heart_buf[28] = pb_charge_state;

    heart_buf[sizeof(heart_buf) - 2] = (crc_code & 0x00ff);
    heart_buf[sizeof(heart_buf) - 1] = (crc_code >> 8);
    // HAL_UART_Transmit_DMA(&huart2, heart_buf, sizeof(heart_buf));
    // HAL_UART_Transmit_IT(&huart2, heart_buf, sizeof(heart_buf));
    HAL_UART_Transmit(&huart2, heart_buf, sizeof(heart_buf), 1000);
}

extern unsigned char pre_cmd;
static void hcp_analyse_callback(unsigned int size)
{
    // for (int i = 0; i < size; i++)
    // {
    //     printf("-%x", hcp_buf[i]);
    // }
    // printf("\r\n");
    if (hcp_buf[0] != 0x5a)
    {
        return;
    }
    unsigned short crc_code = CRC16_Modbus(hcp_buf, size - 2);
    if ((hcp_buf[size - 2] != (crc_code & 0x00ff)) || (hcp_buf[size - 1] != (crc_code >> 8)))
    {
        return;
    }
    // Record the current one click start command
    motor_record_stat(hcp_buf[3]);

    if (EMERGENCY_KEY_FLAG() == 0)
    {
        if (hcp_buf[3] == 0x01)
        {
            elog_d("HCP", "heartbeat");
            response_beat();
        }
        return;
    }

    switch (hcp_buf[3])
    {
    case 0x01:
        elog_d("HCP", "heartbeat");
        // response 0x11
        response_beat();
        // water_pump_set(&hcp_buf[4]);
        break;
    case 0x02:
        elog_d("HCP", "one button start");
        button_start();
        break;
    case 0x03:
        elog_d("HCP", "one button reset");
        button_reset();
        break;
    case 0x04:
        elog_d("HCP", "one button stop");
        button_stop();
        break;
    case 0x05:
        elog_d("HCP", "translation 1 start");
        brush_fb_1(hcp_buf[4]);
        break;
    case 0x06:
        elog_d("HCP", "translation 2 start");
        brush_fb_2(hcp_buf[4]);
        break;
    case 0x07:
        elog_d("HCP", "translation 3 start");
        brush_fb_3(hcp_buf[4]);
        break;
    case 0x08:
        elog_d("HCP", "translation 1-2-3start");
        brush_fb_123(hcp_buf[4]);
        break;
    case 0x09:
        elog_d("HCP", "promotion and demotion 1 start");
        brush_ud_1(hcp_buf[4]);
        break;
    case 0x0a:
        elog_d("HCP", "promotion and demotion 2 start");
        brush_ud_2(hcp_buf[4]);
        break;
    case 0x0b:
        elog_d("HCP", "promotion and demotion 3 start");
        brush_ud_3(hcp_buf[4]);
        break;
    case 0x0c:
        elog_d("HCP", "promotion and demotion 1-2-3 start");
        brush_ud_123(hcp_buf[4]);
        break;
    case 0x0d:
        elog_d("HCP", "position set");
        brush_position_set(&hcp_buf[4]);
        break;
    case 0x0e:
        elog_d("HCP", "Forward and reverse settings");

        break;
    case 0x0f:
        elog_d("HCP", "brush start or stop");
        drum_revolve(hcp_buf[4]);
        break;
    case 0x10:
        elog_d("HCP", "water pump start or stop");
        water_pump_set(&hcp_buf[4]);
        break;
    case 0x16:
        elog_d("HCP", "Spraying rod big arm start");
        big_arm_start(hcp_buf[4]);
        break;
    case 0x17:
        elog_d("HCP", "Spraying rod small arm start");
        small_arm_start(hcp_buf[4]);
        break;
    case 0x18:
        elog_d("HCP", "Folding boom and spray bar small arm start");
        all_arm_start(hcp_buf[4]);
        break;
    case 0xAE:
        elog_d("HCP", "Clear fault");
        err_deal();
        break;
    case 0xA1:
        elog_d("HCP", "ud1 negative pulse");
        brush_ud_1_negative_pulse();
        break;
    case 0x19:
        elog_d("HCP", "ud1 negative pulse");
        change_B_S_arm_state(hcp_buf[4]);
        break;
    }
}

void hcp_task()
{
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, hcp_buf, sizeof(hcp_buf));
    HAL_UART_Receive_IT(&huart2, &hcp_rec_s, 1);
    unsigned int size;
    while (1)
    {
        if (xTaskNotifyWait(0, 0, &size, 1000) == pdFALSE)
        {
            // elog_e("HCP", "Upper computer lost");
            // HAL_UART_Receive_IT(&huart2, &hcp_rec_s, 1);
        }
        else
        {

            hcp_analyse_callback(size);

            // for (int i = 0; i < size; i++)
            // {
            //     printf("-%x", hcp_buf[i]);
            // }
            // printf("---------%d\r\n", size);
            // vTaskDelay(100 / portTICK_RATE_MS);
            memset(hcp_buf, 0, sizeof(hcp_buf));
            HAL_UART_Receive_IT(&huart2, &hcp_rec_s, 1);
        }
    }
}

void hcp_recv_callback(unsigned short Size)
{
    xTaskNotifyFromISR(hcp_task_handle, Size, eSetValueWithOverwrite, NULL);
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, hcp_buf, sizeof(hcp_buf));
}
