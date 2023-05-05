#include "liquid_level.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include <stdio.h>
#include "flowmeter_hl.h"

TaskHandle_t liquid_level_task_handle = NULL;
unsigned short adc_buf[10];

unsigned char liquid_level_1 = 0;
unsigned char liquid_level_2 = 0;
unsigned char liquid_level_3 = 0;
unsigned char liquid_level_4 = 0;

unsigned short ave_buf[4][500] = {0};

void liquid_level_task()
{
    static unsigned short index = 0;
    HAL_ADCEx_Calibration_Start(&hadc1);
    while (1)
    {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, 10);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        HAL_ADC_Stop_DMA(&hadc1);
        ave_buf[0][index] = adc_buf[0];
        ave_buf[1][index] = adc_buf[1];
        ave_buf[2][index] = adc_buf[2];
        ave_buf[3][index++] = adc_buf[3];
        index %= 500;
        vTaskDelay(5 / portTICK_RATE_MS);
        // printf("%d\r\n", adc_buf[0]);
    }
}

static void level_matrixing(unsigned int *data)
{
    if (*data >= 650)
    {
        *data = 100;
    }
    else if (*data < 20)
    {
        *data = 0;
    }
    else
    {
        *data = (((*data - 20) * 0.15873)) / 10 * 10;
    }
}

void get_liquid_level(unsigned char *l_1, unsigned char *l_2, unsigned char *l_3, unsigned char *l_4)
{
    unsigned int l1 = 0;
    unsigned int l2 = 0;
    unsigned int l3 = 0;
    unsigned int l4 = 0;
    for (unsigned short i = 0; i < 500; i++)
    {
        l1 += ave_buf[0][i];
        l2 += ave_buf[1][i];
        l3 += ave_buf[2][i];
        l4 += ave_buf[3][i];
    }
    l1 /= 500;
    l2 /= 500;
    l3 /= 500;
    l4 /= 500;

    level_matrixing(&l1);
    level_matrixing(&l2);
    level_matrixing(&l3);
    level_matrixing(&l4);

    if (l_1 != NULL)
    {
        *l_1 = l1;
    }

    if (l_2 != NULL)
    {
        *l_2 = l2;
    }

    if (l_3 != NULL)
    {
        *l_3 = l3;
    }

    if (l_4 != NULL)
    {
        *l_4 = l4;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    vTaskNotifyGiveFromISR(liquid_level_task_handle, NULL);
}

#define MIX_CAPACITY

unsigned int wat_rat = 10;
unsigned int det_rat = 1;

void accurate_ratio_task()
{
    unsigned char wat_level = 0;
    unsigned char det_level = 0;
    unsigned char mix_level = 0;
    char open_flag = 0;
    while (1)
    {
        get_liquid_level(&wat_level, &det_level, &mix_level, NULL);
        if (mix_level > 90)
        {
            open_flag = 0;
        }
        if (mix_level < 40)
        {
            open_flag = 1;
        }

        if (open_flag)
        {
            if ((wat_level > 1) && (det_level > 1) && (EMERGENCY_KEY_FLAG() == 0))
            {

            }

//            fm_get_total_flow
        }

        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

