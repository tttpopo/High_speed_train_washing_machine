#include "liquid_level.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include <stdio.h>
#include "flowmeter_hl.h"
#include "motor_hal.h"

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
        // printf("%d-%d-%d-%d\r\n", adc_buf[0],adc_buf[1],adc_buf[2],adc_buf[3]);
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

//////////////////////////////////// accurate ratio part///////////////////////////////////

/*
Corresponding relationship of liquid level gauge
ADC1 - water
ADC2 - detergent
ADC3 - mix
ADC4 - spray

Corresponding relationship of water pump
pump1 - water
pump2 - detergent
pump3 - mix
pump4 - spray
*/
TaskHandle_t accurate_ratio_task_handle = NULL;

#define MIX_CAPACITY 20

char HCP_START_ALLOCATE = 1;

float rat = 10.0;

void accurate_ratio_task()
{
    unsigned char wat_level = 0;
    unsigned char det_level = 0;
    unsigned char mix_level = 0;
    unsigned char spr_level = 0;

    float wat_flo = 0;
    float det_flo = 0;

    char start_flag = 0;

    // PUMP_ON[1]();
    while (1)
    {
        get_liquid_level(&wat_level, &det_level, &mix_level, &spr_level);
        fm_get_total_flow(&wat_flo, &det_flo);
        printf("%d,%d,%d,%d---  1->>%f  2->>%f\r\n", wat_level, det_level, mix_level, spr_level, wat_flo, det_flo);
        // printf("%d,%d,%d,%d\r\n", spr_level, mix_level, det_level, wat_level);

        if (mix_level < 40)
        {
            start_flag = 1;
        }

        if (mix_level > 80)
        {
            start_flag = 0;
        }

        if (start_flag)
        {
            PUMP_1_ON
        }
        else
        {
            PUMP_1_OFF();
            PUMP_2_OFF();
            PUMP_3_OFF();
            PUMP_4_OFF();
        }

        // if (EMERGENCY_KEY_FLAG() == 1)
        // {
        //     get_liquid_level(&wat_level, &det_level, &mix_level, &spr_level);

        //     printf("%d,%d,%d,%d\r\n", wat_level, det_level, mix_level, spr_level);
        //     // printf("%d,%d,%d,%d\r\n", spr_level, mix_level, det_level, wat_level);
        //     if (mix_level < 60)
        //     {
        //         PUMP_ON[0]();
        //         PUMP_ON[1]();
        //         fm_get_total_flow(&wat_flo, &det_flo);

        //         if (wat_flo / det_flo >= rat - 1) // det less
        //         {
        //             PUMP_ON[1]();
        //         }
        //         else if (wat_flo / det_flo < rat + 1) // det more
        //         {
        //             PUMP_OFF[1]();
        //         }
        //     }

        //     if (mix_level > 90)
        //     {
        //         PUMP_OFF[0]();
        //         PUMP_OFF[1]();
        //     }

        //     if ((spr_level < 50) && (mix_level > 50))
        //     {
        //         PUMP_ON[2]();
        //     }
        //     if (spr_level > 80)
        //     {
        //         PUMP_OFF[2]();
        //     }
        // }
        // else
        // {
        //     PUMP_OFF[0]();
        //     PUMP_OFF[1]();
        //     PUMP_OFF[2]();
        //     PUMP_OFF[3]();
        // }

        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

// Control allocation flag bit
void allocate_en(char en)
{
    HCP_START_ALLOCATE = en;
}
