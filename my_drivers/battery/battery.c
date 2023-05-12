#include "battery.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "stdio.h"
#include "elog.h"
#include "console.h"

TaskHandle_t battery_task_handle;
unsigned char bat_buf[50] = {0};
unsigned char send_buf[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
char bat_level = 0;
/// @brief Interrupt callback function
/// @param Size
void bat_recv_callback(unsigned short Size)
{
    xTaskNotifyFromISR(battery_task_handle, Size, eSetValueWithOverwrite, NULL);
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, bat_buf, sizeof(bat_buf));
}

/// @brief Obtain remaining power
/// @param data
void bat_get_power(char *data)
{
    *data = bat_level;
}

void battery_task()
{
    unsigned int size;
    while (HAL_UARTEx_ReceiveToIdle_IT(&huart3, bat_buf, sizeof(bat_buf)) != HAL_OK)
    {
    }

    while (1)
    {
        HAL_UART_Transmit_IT(&huart3, send_buf, sizeof(send_buf));
        if (xTaskNotifyWait(0, 0, &size, 500) == pdFALSE)
        {
            if (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_IDLE) == 0)
            {
                while (HAL_UARTEx_ReceiveToIdle_IT(&huart3, bat_buf, sizeof(bat_buf)) != HAL_OK)
                {
                    vTaskDelay(50 / portTICK_RATE_MS);
                }
            }
        }
        else
        {
            bat_level = (bat_buf[8] << 8 | bat_buf[9]) * 100 / 60400;
        }
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
