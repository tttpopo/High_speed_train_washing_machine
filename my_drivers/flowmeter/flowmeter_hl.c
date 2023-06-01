#include "usart.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "flowmeter_hl.h"
#include "stdio.h"
#include "motor_hal.h"
#include "console.h"
#include <string.h>

/*
meter1 - water
meter2 - detergent
*/

TaskHandle_t flowmeter_task_handle;

unsigned char fm_recv_buf[15] = {0};
float total_flow_1 = 0;
float total_flow_2 = 0;

int fm_get_total_flow(float *fm_1, float *fm_2)
{
    if ((total_flow_1 < 0) && (total_flow_2 < 0))
    {
        return 3;
    }
    else if (total_flow_1 < 0)
    {
        return 1;
    }
    else if (total_flow_2 < 0)
    {
        return 2;
    }

    *fm_1 = total_flow_1;
    *fm_2 = total_flow_2;

    return 0;
}

static void fm_recv_start()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, fm_recv_buf, sizeof(fm_recv_buf));
}

void fm_recv_callback(unsigned short Size)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, fm_recv_buf, sizeof(fm_recv_buf));
    xTaskNotifyFromISR(flowmeter_task_handle, Size, eSetValueWithOverwrite, NULL);
}

void cs_pump_on_cb(unsigned char *cmd)
{

    char *temp_p = NULL;
    unsigned int s_id = 0;
    long int speed = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "-");
    s_id = atoi(temp_p);

    switch (s_id)
    {
    case 1:
        PUMP_1_ON();
        break;
    case 2:
        PUMP_2_ON();
        break;
    case 3:
        PUMP_3_ON();
        break;
    case 4:
        PUMP_4_ON();
        break;
    }
    // PUMP_1_ON();
}

void cs_pump_off_cb(unsigned char *cmd)
{

    char *temp_p = NULL;
    unsigned int s_id = 0;
    long int speed = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "-");
    s_id = atoi(temp_p);

    switch (s_id)
    {
    case 1:
        PUMP_1_OFF();
        break;
    case 2:
        PUMP_2_OFF();
        break;
    case 3:
        PUMP_3_OFF();
        break;
    case 4:
        PUMP_4_OFF();
        break;
    case 5:
        PUMP_1_OFF();
        PUMP_2_OFF();
        PUMP_3_OFF();
        PUMP_4_OFF();
        break;
    }

    // PUMP_1_OFF();
}

void flowmeter_task()
{
    // PUMP_1_ON;

    char switch_fm_flag = 0;

    float temp_flow_1 = 0;
    float temp_flow_2 = 0;

    char first_flag_1 = 1;
    char first_flag_2 = 1;

    unsigned int size = 0;
    unsigned char send_buf_1[] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x04, 0xC5, 0xCB};
    unsigned char send_buf_2[] = {0x02, 0x03, 0x00, 0x08, 0x00, 0x04, 0xC5, 0xF8};
    int temp_data_f = 0;
    int temp_data_b = 0;
    fm_recv_start();

    cs_reg_fun("pon", cs_pump_on_cb);
    cs_reg_fun("pof", cs_pump_off_cb);

    while (1)
    {
        // HAL_UART_Transmit_DMA(&huart4,send_buf,sizeof(send_buf));

        if (switch_fm_flag == 0)
        {
            HAL_UART_Transmit(&huart4, send_buf_1, sizeof(send_buf_1), 1000);
        }
        else
        {
            HAL_UART_Transmit(&huart4, send_buf_2, sizeof(send_buf_2), 1000);
        }

        if (xTaskNotifyWait(0, 0, &size, 1000) == pdFALSE)
        {
        }
        else
        {
            if (size == 13)
            {
                temp_data_f = (fm_recv_buf[5] << 24 | fm_recv_buf[6] << 16 | fm_recv_buf[3] << 8 | fm_recv_buf[4]);
                temp_data_b = (fm_recv_buf[9] << 24 | fm_recv_buf[10] << 16 | fm_recv_buf[7] << 8 | fm_recv_buf[8]);
                if (switch_fm_flag == 0)
                {
                    total_flow_1 = ((*((float *)(&temp_data_f))) * 100) + (*((float *)(&temp_data_b)));
                }
                else
                {
                    total_flow_2 = ((*((float *)(&temp_data_f))) * 100) + (*((float *)(&temp_data_b)));
                }

                /////////////////////////////////////////////////
                if (first_flag_1)
                {
                    if (total_flow_1 != 0)
                    {
                        first_flag_1 = 0;
                        temp_flow_1 = total_flow_1;
                    }
                }
                if (first_flag_2)
                {
                    if (total_flow_2 != 0)
                    {

                        first_flag_2 = 0;
                        temp_flow_2 = total_flow_2;
                    }
                }

                // printf("1--->%f   2--->%f\r\n", total_flow_1 - temp_flow_1, total_flow_2 - temp_flow_2);
                printf("1--->%f   2--->%f\r\n", total_flow_1, total_flow_2);
                /////////////////////////////////////////////////
            }
        }

        switch_fm_flag = ~switch_fm_flag;
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
