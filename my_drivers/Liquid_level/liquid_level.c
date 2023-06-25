#include "liquid_level.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include <stdio.h>
#include "flowmeter_hl.h"
#include "motor_hal.h"
#include "console.h"
#include "elog.h"
#include <stdlib.h>
#include <string.h>

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

// char HCP_START_ALLOCATE = 1;
char stat = 1;
char TARG_CAP = 90;
char EME_CAP = 95;
float rat = 10.0;

void cs_set_ratio_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "-");
    s_id = atoi(temp_p);
    rat = s_id;
    elog_i("RATIO", "set ratio %f", rat);
}

void accurate_ratio_task()
{
    // static unsigned int switch_flag = 0;

    // water level
    unsigned char wat_level = 0;
    unsigned char det_level = 0;
    unsigned char mix_level = 0;
    unsigned char spr_level = 0;

    // flow
    float wat_flo = 0;
    float det_flo = 0;

    // targ flow
    float temp_targ_wat_flo = 0;
    float temp_targ_det_flo = 0;

    // complete flag
    char wat_cmp = 0;
    char det_cmp = 0;

    // wait system stable
    PUMP_1_OFF();
    PUMP_2_OFF();
    PUMP_3_ON();
    PUMP_4_OFF();

    cs_reg_fun("setratio", cs_set_ratio_cb);

    vTaskDelay(4000 / portTICK_RATE_MS);

    // get_liquid_level(&wat_level, &det_level, &mix_level, &spr_level);
    // fm_get_total_flow(&wat_flo, &det_flo);

    // printf("%d,%d,%d,%d---  1->>%f  2->>%f\r\n", wat_level, det_level, mix_level, spr_level, wat_flo, det_flo);

    // printf("%d,%d,%d,%d---  1->>%f  2->>%f\r\n", wat_level, det_level, mix_level, spr_level, wat_flo, det_flo);

    while (1)
    {
        get_liquid_level(&wat_level, &det_level, &mix_level, &spr_level);
        fm_get_total_flow(&wat_flo, &det_flo);

        // printf("%d,%d,%d,%d---  1->>%f  2->>%f\r\n", wat_level, det_level, mix_level, spr_level, wat_flo, det_flo);
        // printf("%d,%d,%d,%d\r\n", spr_level, mix_level, det_level, wat_level);

        switch (stat)
        {
        case 0:
            PUMP_1_OFF();
            PUMP_2_OFF();
            PUMP_3_OFF();
            PUMP_4_OFF();
            stat = 3;
            break;
        case 1:
            // get_liquid_level(&wat_level, &det_level, &mix_level, &spr_level);
            // printf("%d,%d,%d,%d\r\n", wat_level, det_level, mix_level, spr_level);
            if ((wat_level > 5) && (det_level > 5))
            {
                if ((mix_level < 10))
                {
                    elog_i("MIX", "Start proportioning!");
                    temp_targ_wat_flo = (((90.0 - (float)mix_level) / 100.0) * 20.0) * (rat / (rat + 1.0));
                    temp_targ_det_flo = (((90.0 - (float)mix_level) / 100.0) * 20.0) * (1.0 / (rat + 1.0));
                    PUMP_1_ON();
                    PUMP_2_ON();
                    PUMP_3_OFF();
                    stat = 2;
                }
            }
            break;
        case 2:
            fm_get_total_flow(&wat_flo, &det_flo);
            printf("targ wat - >%f,tart det - >%f  ------  curt wat - >%f,curt det - >%f\r\n", temp_targ_wat_flo, temp_targ_det_flo, wat_flo, det_flo);
            if (wat_flo >= temp_targ_wat_flo - 0.1)
            {
                fm_reset_wat_flow();
                PUMP_1_OFF();
                wat_cmp = 1;
                // elog_i("WAT","water is ok!");
            }
            if (det_flo >= temp_targ_det_flo - 0.2)
            // if (wat_flo >= temp_targ_det_flo-0.2)
            {
                fm_reset_det_flow();
                PUMP_2_OFF();
                det_cmp = 1;
                // elog_i("DET","detergent is ok!");
            }
            if (wat_cmp && det_cmp)
            {
                wat_cmp = 0;
                det_cmp = 0;
                PUMP_3_ON();
                stat = 1;
                elog_i("MIX", "all is ok!");
            }
            if (mix_level >= 90)
            {
                PUMP_1_OFF();
                PUMP_2_OFF();
                PUMP_3_ON();
                stat = 1;
                elog_i("MIX", "mix is full!");
                fm_reset_wat_flow();
                fm_reset_det_flow();
            }

            break;
        case 3:

            break;
        }

        // if (switch_flag % 4 == 0)
        // {
        //     PUMP_2_ON();
        // }
        // else if (switch_flag % 5 == 0)
        // {
        //     PUMP_2_OFF();
        // }
        // switch_flag++;
        // switch_flag%=1000000;
        // switch_flag = !switch_flag;

        // Emergency judgment
        // if (mix_level > 90)
        // {
        //     PUMP_1_OFF();
        //     PUMP_2_OFF();
        //     PUMP_3_ON();
        //     PUMP_4_OFF();
        //     fm_reset_det_flow();
        //     fm_reset_wat_flow();
        //     stat = 1;
        // }
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

// Control allocation flag bit
void allocate_en(char en)
{
    stat = en;
}
