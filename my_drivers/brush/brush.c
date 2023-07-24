#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor_drive.h"
#include "brush.h"
#include "flashdb.h"
#include "console.h"
#include "string.h"
#include "liquid_level.h"
#include <stdio.h>

// extern unsigned int MOTOR_ERR_ID;

TaskHandle_t brush_deamon_task_handle = NULL;
TaskHandle_t brush_fb_123_task_handle = NULL;
TaskHandle_t brush_ud_123_task_handle = NULL;
TaskHandle_t all_arm_start_task_handle = NULL;
TaskHandle_t button_all_task_handle = NULL;
TaskHandle_t self_cali_task_handle = NULL;

// void (*RUNNING_FUN)(void) = NULL;

// long int FB_TARG_PULSE = 1200000;  // Translation brush group end point pulse value
long int FB1_TARG_PULSE = 1100000; // Translation brush group end point pulse value
long int FB2_TARG_PULSE = 900000;  // Translation brush group end point pulse value
long int FB3_TARG_PULSE = 800000;  // Translation brush group end point pulse value
// long int FB1_TARG_PULSE = 1500000; // Translation brush group end point pulse value
// long int FB2_TARG_PULSE = 1200000; // Translation brush group end point pulse value
// long int FB3_TARG_PULSE = 1200000; // Translation brush group end point pulse value
// long int FB1_ORIGIN_PULSE = 0;  // Translation brush group end point pulse value
// long int FB2_ORIGIN_PULSE = 0;
// long int FB3_ORIGIN_PULSE = 0;
// long int UD_TARG_PULSE = 50000;    // Lifting brush group end point pulse value
long int UD1_TARG_PULSE = 600000;  // Lifting brush group end point pulse value
long int UD2_TARG_PULSE = 2700000; // Lifting brush group end point pulse value
long int UD3_TARG_PULSE = 3800000; // Lifting brush group end point pulse value
// long int UD1_ORIGIN_PULSE = 0;  // Lifting brush group end point pulse value
// long int UD2_ORIGIN_PULSE = 0;
// long int UD3_ORIGIN_PULSE = 0;
// long int B_ARM_ORIGIN_PULSE = 0;
// long int S_ARM_ORIGIN_PULSE = 0;

#define CAL_PULSE 10000
#define B_ARM_CAL_PULSE 500000

long int MOTOR_ORIGIN_PULSE[8] = {0};

typedef enum
{
    STOP = 0,
    RUNNING_TARG,
    RUNNING_ORIGIN,
    // ARRIVED,
} MOTOR_STAT;

typedef enum
{
    ORIGIN = 0,
    TARG,
    MIDDLE
} MOTOR_IN_PLACE_STAT;

MOTOR_STAT MOTOR_STATE_TABLE[8] = {STOP};
MOTOR_IN_PLACE_STAT MOTOR_IN_PLACE_STAT_TABLE[8] = {ORIGIN};

unsigned char MOTOR_ERR_CODE_TABLE[8] = {0};
int MOTOR_ERR_CNT[8] = {0};

void brush_position_set(unsigned char *data)
{
    long int temp_val = 0;
    temp_val = ((data[0] << 8) | (data[1]));
    if ((temp_val < 0) || (temp_val > 390))
    {
        ;
    }
    else
    {
        UD3_TARG_PULSE = temp_val * 10000;
    }

    temp_val = ((data[2] << 8) | (data[3]));
    if ((temp_val < 0) || (temp_val > 270))
    {
        ;
    }
    else
    {
        UD2_TARG_PULSE = temp_val * 10000;
    }

    temp_val = data[4];
    if ((temp_val < 0) || (temp_val > 120))
    {
        ;
    }
    else
    {
        UD1_TARG_PULSE = temp_val * 10000;
    }

    temp_val = data[5];
    if ((temp_val < 0) || (temp_val > 120))
    {
        ;
    }
    else
    {
        FB3_TARG_PULSE = temp_val * 10000;
    }

    temp_val = data[6];
    if ((temp_val < 0) || (temp_val > 120))
    {
        ;
    }
    else
    {
        FB2_TARG_PULSE = temp_val * 10000;
    }

    temp_val = data[7];
    if ((temp_val < 0) || (temp_val > 150))
    {
        ;
    }
    else
    {
        FB1_TARG_PULSE = temp_val * 10000;
    }

    elog_i("SET_PULSE", "UD123-%ld-%ld-%ld,fb123-%ld-%ld-%ld", UD1_TARG_PULSE, UD2_TARG_PULSE, UD3_TARG_PULSE, FB1_TARG_PULSE, FB2_TARG_PULSE, FB3_TARG_PULSE);
    set_ratio(data[8]);
}

/// @brief Control brush group 1 to move forward and backward
/// @param state
void brush_fb_1(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_FB_1]);
    MOTOR_BK_ON[MOTOR_FB_1]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_1], FB1_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_1], FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_1] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] = MIDDLE;
            elog_d("BRUSH", "set motor 1 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_1]();
            elog_e("BRUSH", "set motor 1 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_1], MOTOR_ORIGIN_PULSE[MOTOR_FB_1] - CAL_PULSE, FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_1] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] = MIDDLE;
            elog_d("BRUSH", "set motor 1 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_1]();
            elog_e("BRUSH", "set motor 1 pulse fail");
        }
    }
}

void brush_fb_2(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_FB_2]);
    MOTOR_BK_ON[MOTOR_FB_2]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_2], FB2_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_2], FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_2] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] = MIDDLE;
            elog_d("BRUSH", "set motor 2 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_2]();
            elog_e("BRUSH", "set motor 2 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_2], MOTOR_ORIGIN_PULSE[MOTOR_FB_2] - CAL_PULSE, FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_2] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] = MIDDLE;
            elog_d("BRUSH", "set motor 2 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_2]();
            elog_e("BRUSH", "set motor 2 pulse fail");
        }
    }
}

void brush_fb_3(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_FB_3]);
    MOTOR_BK_ON[MOTOR_FB_3]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_3], FB3_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_3], FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_3] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] = MIDDLE;
            elog_d("BRUSH", "set motor 3 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_3]();
            elog_e("BRUSH", "set motor 3 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_3], MOTOR_ORIGIN_PULSE[MOTOR_FB_3] - CAL_PULSE, FB_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_FB_3] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] = MIDDLE;
            elog_d("BRUSH", "set motor 3 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_FB_3]();
            elog_e("BRUSH", "set motor 3 pulse fail");
        }
    }
}

static void brush_fb_123_task(void *state)
{
    switch (*((unsigned char *)state))
    {
    case 1:
        // go to targ
        brush_fb_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_3(1);
        break;
    case 0:
        // go to origin
        brush_fb_1(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_2(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_3(0);
        break;
    case 3:
        // stop
        motor_stop(MOTOR_STATION[MOTOR_FB_1]);
        MOTOR_BK_OFF[MOTOR_FB_1]();
        motor_stop(MOTOR_STATION[MOTOR_FB_2]);
        MOTOR_BK_OFF[MOTOR_FB_2]();
        motor_stop(MOTOR_STATION[MOTOR_FB_3]);
        MOTOR_BK_OFF[MOTOR_FB_3]();
        break;
    }
    brush_fb_123_task_handle = NULL;
    vTaskDelete(NULL);
}

/// @brief One click start brush group 123 pan
/// @param state
void brush_fb_123(unsigned char state)
{
    static unsigned char temp_state;
    temp_state = state;
    if (brush_fb_123_task_handle == NULL)
    {
        xTaskCreate(brush_fb_123_task, "brush_fb_123_task", 100, (void *)(&temp_state), 0, &brush_fb_123_task_handle);
    }
}

void brush_ud_1(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_UD_1]);
    MOTOR_BK_ON[MOTOR_UD_1]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_1], UD1_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_UD_1], UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_1] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] = MIDDLE;
            elog_d("BRUSH", "set motor 4 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_1]();
            elog_e("BRUSH", "set motor 4 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_1], MOTOR_ORIGIN_PULSE[MOTOR_UD_1] - CAL_PULSE, UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_1] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] = MIDDLE;
            elog_d("BRUSH", "set motor 4 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_1]();
            elog_e("BRUSH", "set motor 4 pulse fail");
        }
    }
}

void brush_ud_2(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_UD_2]);
    MOTOR_BK_ON[MOTOR_UD_2]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_2], UD2_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_UD_2], UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_2] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] = MIDDLE;
            elog_d("BRUSH", "set motor 5 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_2]();
            elog_e("BRUSH", "set motor 5 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_2], MOTOR_ORIGIN_PULSE[MOTOR_UD_2] - CAL_PULSE, UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_2] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] = MIDDLE;
            elog_d("BRUSH", "set motor 5 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_2]();
            elog_e("BRUSH", "set motor 5 pulse fail");
        }
    }
}

void brush_ud_3(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_UD_3]);
    MOTOR_BK_ON[MOTOR_UD_3]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_3], UD3_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_UD_3], UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_3] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] = MIDDLE;
            elog_d("BRUSH", "set motor 6 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_3]();
            elog_e("BRUSH", "set motor 6 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_3], MOTOR_ORIGIN_PULSE[MOTOR_UD_3] - CAL_PULSE, UD_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_UD_3] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] = MIDDLE;
            elog_d("BRUSH", "set motor 6 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_UD_3]();
            elog_e("BRUSH", "set motor 6 pulse fail");
        }
    }
}

static void brush_ud_123_task(void *state)
{
    switch (*((unsigned char *)state))
    {
    case 1:
        // go to targ
        brush_ud_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_3(1);
        break;
    case 0:
        // go to origin
        brush_ud_1(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_3(0);
        break;
    case 3:
        // stop
        motor_stop(MOTOR_STATION[MOTOR_UD_1]);
        MOTOR_BK_OFF[MOTOR_UD_1]();
        motor_stop(MOTOR_STATION[MOTOR_UD_2]);
        MOTOR_BK_OFF[MOTOR_UD_2]();
        motor_stop(MOTOR_STATION[MOTOR_UD_3]);
        MOTOR_BK_OFF[MOTOR_UD_3]();
        break;
    }
    brush_ud_123_task_handle = NULL;
    vTaskDelete(NULL);
}

void brush_ud_123(unsigned char state)
{
    static unsigned char temp_state;
    temp_state = state;
    if (brush_ud_123_task_handle == NULL)
    {
        xTaskCreate(brush_ud_123_task, "brush_ud_123_task", 100, (void *)(&temp_state), 0, &brush_ud_123_task_handle);
    }
}

void drum_revolve(unsigned char state)
{
    switch (state)
    {
    case 0:
        DRUM_OFF[0]();
        DRUM_OFF[1]();
        DRUM_OFF[2]();
        break;
    case 1:
        DRUM_ON[0]();
        DRUM_ON[1]();
        DRUM_ON[2]();
        break;
    case 0x11:
        DRUM_ON[0]();
        break;
    case 0x12:
        DRUM_ON[1]();
        break;
    case 0x13:
        DRUM_ON[2]();
        break;
    }
}

// Switching on and off the water pump
void water_pump_set(unsigned char *data)
{
    static int auto_allocate_flag = 0;

    if (data[0] == 0)
    {
        if (auto_allocate_flag == 1) // allocate en
        {
            for (int i = 0; i < 4; i++)
            {
                if (data[i + 3] == 1)
                {
                    PUMP_ON[i + 2]();
                }
                else
                {
                    PUMP_OFF[i + 2]();
                }
            }
        }
        else // allocate off
        {
            for (int i = 0; i < 7; i++)
            {
                if (data[i + 1] == 1)
                {
                    PUMP_ON[i]();
                }
                else
                {
                    PUMP_OFF[i]();
                }
            }
        }
    }
    // Do not determine whether to enable automatic liquid dispensing
    else if (data[0] == 1)
    {
        if (auto_allocate_flag == 0)
        {
            auto_allocate_flag = 1;
            allocate_en(1);
            elog_e("auto", "----------------------------------on-----\r\n");
        }
    }
    else if (data[0] == 2)
    {
        if (auto_allocate_flag == 1)
        {
            auto_allocate_flag = 0;
            allocate_en(0);
            elog_e("auto", "----------------------------------off-----\r\n");
        }
    }
}

/// @brief start big arm motor
/// @param state
void big_arm_start(unsigned char state)
{
    motor_en(MOTOR_STATION[MOTOR_B_ARM]);
    MOTOR_BK_ON[MOTOR_B_ARM]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_B_ARM], B_ARM_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_B_ARM], B_ARM_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_B_ARM] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
            elog_d("BRUSH", "set motor 7 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_B_ARM]();
            elog_e("BRUSH", "set motor 7 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_B_ARM], MOTOR_ORIGIN_PULSE[MOTOR_B_ARM] + B_ARM_CAL_PULSE, B_ARM_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_B_ARM] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
            elog_d("BRUSH", "set motor 7 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_B_ARM]();
            elog_e("BRUSH", "set motor 7 pulse fail");
        }
    }
}

void small_arm_start(unsigned char state)
{
    // motor_en(MOTOR_STATION[MOTOR_S_ARM]);
    MOTOR_BK_ON[MOTOR_S_ARM]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], S_ARM_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_S_ARM], S_ARM_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_S_ARM] = RUNNING_TARG;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
            elog_d("BRUSH", "set motor 8 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_S_ARM]();
            elog_e("BRUSH", "set motor 8 pulse fail");
        }
    }
    else
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], MOTOR_ORIGIN_PULSE[MOTOR_S_ARM] - CAL_PULSE, S_ARM_SPEED) == HAL_OK)
        {
            MOTOR_STATE_TABLE[MOTOR_S_ARM] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
            elog_d("BRUSH", "set motor 8 pulse ok");
        }
        else
        {
            MOTOR_BK_OFF[MOTOR_S_ARM]();
            elog_e("BRUSH", "set motor 8 pulse fail");
        }
    }
}

static void all_arm_start_task(void *state)
{
    unsigned char err_code = 0;
    switch (*((unsigned char *)state))
    {
    case 1:
        // go to targ
        small_arm_start(1);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] != TARG)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_S_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = TARG;
                MOTOR_BK_OFF[MOTOR_S_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_S_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_S_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_S_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_S_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        big_arm_start(1);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != TARG)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_B_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = TARG;
                MOTOR_BK_OFF[MOTOR_B_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_B_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_B_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_B_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_B_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        break;
    case 0:
        // go to origin
        big_arm_start(0);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != ORIGIN)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_B_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = ORIGIN;
                MOTOR_BK_OFF[MOTOR_B_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_B_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_B_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_B_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_B_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        small_arm_start(0);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] != ORIGIN)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_S_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = ORIGIN;
                MOTOR_BK_OFF[MOTOR_S_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_S_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_S_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_S_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_S_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        break;
    case 3:
        // stop
        motor_stop(MOTOR_STATION[MOTOR_B_ARM]);
        MOTOR_BK_OFF[MOTOR_B_ARM]();
        motor_stop(MOTOR_STATION[MOTOR_S_ARM]);
        MOTOR_BK_OFF[MOTOR_S_ARM]();
        break;
    }
    all_arm_start_task_handle = NULL;
    vTaskDelete(NULL);
}

void all_arm_start(unsigned char state)
{
    static unsigned char temp_state;
    temp_state = state;
    if (all_arm_start_task_handle == NULL)
    {
        xTaskCreate(all_arm_start_task, "all_arm_start_task", 100, (void *)(&temp_state), 0, &all_arm_start_task_handle);
    }
}

static void button_all_task(void *state)
{
    unsigned char err_code = 0;
    elog_w("TEST", "button all task is start");
    switch (*((unsigned char *)state))
    {
    case 1:
        small_arm_start(1);
        vTaskDelay(80 / portTICK_RATE_MS);
        small_arm_start(1);
        // while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] != TARG)
        while (MOTOR_LIMIT_FLAG[MOTOR_S_ARM * 2]() != SET)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_S_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = TARG;
                MOTOR_BK_OFF[MOTOR_S_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_S_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_S_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_S_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_S_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        vTaskDelay(200 / portTICK_RATE_MS);
        big_arm_start(1);
        vTaskDelay(80 / portTICK_RATE_MS);
        big_arm_start(1);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != TARG)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_B_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = TARG;
                MOTOR_BK_OFF[MOTOR_B_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_B_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_B_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_B_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_B_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        vTaskDelay(200 / portTICK_RATE_MS);
        brush_fb_1(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_2(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_3(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_3(1);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] != TARG))
        {
            for (int i = MOTOR_FB_1; i < MOTOR_FB_1 + 3; i++)
            {
                if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
                {
                    MOTOR_STATE_TABLE[i] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
                    MOTOR_BK_OFF[i]();
                    motor_stop(MOTOR_STATION[i]);
                }
                motor_read_errcode(MOTOR_STATION[i], &err_code);
                if (err_code != 0)
                {
                    vTaskDelay(500 / portTICK_RATE_MS);
                    motor_read_errcode(MOTOR_STATION[i], &err_code);
                    if (err_code != 0)
                    {
                        MOTOR_ERR_CODE_TABLE[i] = err_code;
                        MOTOR_BK_OFF[i]();
                        MOTOR_STATE_TABLE[i] = STOP;
                        MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
                        elog_e("ERR-CODE", "moto%d--->%d", i, err_code);
                    }
                }
                vTaskDelay(50 / portTICK_RATE_MS);
            }
        }
        vTaskDelay(200 / portTICK_RATE_MS);
        brush_ud_1(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_ud_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_ud_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_3(1);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_ud_3(1);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] != TARG))
        {
            for (int i = MOTOR_UD_1; i < MOTOR_UD_1 + 3; i++)
            {
                if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
                {
                    MOTOR_STATE_TABLE[i] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
                    MOTOR_BK_OFF[i]();
                    motor_stop(MOTOR_STATION[i]);
                }
                motor_read_errcode(MOTOR_STATION[i], &err_code);
                if (err_code != 0)
                {
                    vTaskDelay(500 / portTICK_RATE_MS);
                    motor_read_errcode(MOTOR_STATION[i], &err_code);
                    if (err_code != 0)
                    {
                        MOTOR_ERR_CODE_TABLE[i] = err_code;
                        MOTOR_BK_OFF[i]();
                        MOTOR_STATE_TABLE[i] = STOP;
                        MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
                        elog_e("ERR-CODE", "moto%d--->%d", i, err_code);
                    }
                }
                vTaskDelay(50 / portTICK_RATE_MS);
            }
        }

        break;
    case 0:
        brush_ud_3(0);
        vTaskDelay(200 / portTICK_RATE_MS);
        brush_ud_3(0);

        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(0);
        vTaskDelay(200 / portTICK_RATE_MS);
        brush_ud_2(0);

        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_1(0);
        vTaskDelay(200 / portTICK_RATE_MS);
        brush_ud_1(0);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] != ORIGIN))
        {
            for (int i = MOTOR_UD_1; i < MOTOR_UD_1 + 3; i++)
            {
                if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
                {
                    MOTOR_STATE_TABLE[i] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
                    MOTOR_BK_OFF[i]();
                    motor_stop(MOTOR_STATION[i]);
                }
                motor_read_errcode(MOTOR_STATION[i], &err_code);
                if (err_code != 0)
                {
                    vTaskDelay(500 / portTICK_RATE_MS);
                    motor_read_errcode(MOTOR_STATION[i], &err_code);
                    if (err_code != 0)
                    {
                        MOTOR_ERR_CODE_TABLE[i] = err_code;
                        MOTOR_BK_OFF[i]();
                        MOTOR_STATE_TABLE[i] = STOP;
                        MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
                        elog_e("ERR-CODE", "moto%d--->%d", i, err_code);
                    }
                }
                vTaskDelay(50 / portTICK_RATE_MS);
            }
        }
        vTaskDelay(200 / portTICK_RATE_MS);
        brush_fb_1(0);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_1(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);

        brush_fb_2(0);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_2(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);

        brush_fb_3(0);
        vTaskDelay(100 / portTICK_RATE_MS);
        brush_fb_3(0);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] != ORIGIN))
        {
            for (int i = MOTOR_FB_1; i < MOTOR_FB_1 + 3; i++)
            {
                if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
                {
                    MOTOR_STATE_TABLE[i] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
                    MOTOR_BK_OFF[i]();
                    motor_stop(MOTOR_STATION[i]);
                }
                motor_read_errcode(MOTOR_STATION[i], &err_code);
                if (err_code != 0)
                {
                    vTaskDelay(500 / portTICK_RATE_MS);
                    motor_read_errcode(MOTOR_STATION[i], &err_code);
                    if (err_code != 0)
                    {
                        MOTOR_ERR_CODE_TABLE[i] = err_code;
                        MOTOR_BK_OFF[i]();
                        MOTOR_STATE_TABLE[i] = STOP;
                        MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
                        elog_e("ERR-CODE", "moto%d--->%d", i, err_code);
                    }
                }
                vTaskDelay(50 / portTICK_RATE_MS);
            }
        }
        vTaskDelay(200 / portTICK_RATE_MS);
        big_arm_start(0);
        vTaskDelay(100 / portTICK_RATE_MS);
        big_arm_start(0);
        // while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != ORIGIN)
        while (MOTOR_LIMIT_FLAG[MOTOR_B_ARM * 2 + 1]() != SET)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_B_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = ORIGIN;
                MOTOR_BK_OFF[MOTOR_B_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_B_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_B_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_B_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_B_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_B_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_B_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        vTaskDelay(200 / portTICK_RATE_MS);
        small_arm_start(0);
        vTaskDelay(100 / portTICK_RATE_MS);
        small_arm_start(0);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] != ORIGIN)
        {
            if (motor_read_target_reached(MOTOR_STATION[MOTOR_S_ARM]) == HAL_OK)
            {
                MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = ORIGIN;
                MOTOR_BK_OFF[MOTOR_S_ARM]();
                motor_stop(MOTOR_STATION[MOTOR_S_ARM]);
            }
            motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
            if (err_code != 0)
            {
                vTaskDelay(500 / portTICK_RATE_MS);
                motor_read_errcode(MOTOR_STATION[MOTOR_S_ARM], &err_code);
                if (err_code != 0)
                {
                    MOTOR_ERR_CODE_TABLE[MOTOR_S_ARM] = err_code;
                    MOTOR_BK_OFF[MOTOR_S_ARM]();
                    MOTOR_STATE_TABLE[MOTOR_S_ARM] = STOP;
                    MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
                    elog_e("ERR-CODE", "moto%d--->%d", MOTOR_S_ARM, err_code);
                }
            }
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        break;
    }
    elog_w("TEST", "button all task is delete");
    button_all_task_handle = NULL;
    vTaskDelete(NULL);
}

// void brush_ud_2_negative_pulse()
// {
//     motor_en(MOTOR_STATION[MOTOR_UD_2]);
//     MOTOR_BK_ON[MOTOR_UD_2]();
//     vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
//     if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_2], MOTOR_ORIGIN_PULSE[MOTOR_UD_2] - 5000000, UD_SPEED) == HAL_OK)
//     {
//         MOTOR_STATE_TABLE[MOTOR_UD_2] = RUNNING_ORIGIN;
//         MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] = MIDDLE;
//         elog_d("BRUSH", "set motor 5 pulse ok");
//     }
//     else
//     {
//         MOTOR_BK_OFF[MOTOR_UD_2]();
//         elog_e("BRUSH", "set motor 5 pulse fail");
//     }
// }

// void brush_ud_3_negative_pulse()
// {
//     motor_en(MOTOR_STATION[MOTOR_UD_3]);
//     MOTOR_BK_ON[MOTOR_UD_3]();
//     vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
//     if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_3], MOTOR_ORIGIN_PULSE[MOTOR_UD_3] - 5000000, UD_SPEED) == HAL_OK)
//     {
//         MOTOR_STATE_TABLE[MOTOR_UD_3] = RUNNING_ORIGIN;
//         MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] = MIDDLE;
//         elog_d("BRUSH", "set motor 6 pulse ok");
//     }
//     else
//     {
//         MOTOR_BK_OFF[MOTOR_UD_3]();
//         elog_e("BRUSH", "set motor 6 pulse fail");
//     }
// }

/// @brief button start
/// @param
void button_start(void)
{
    static unsigned char temp_state;
    temp_state = 1;
    if (button_all_task_handle == NULL)
    {
        xTaskCreate(button_all_task, "button_all_task", 180, (void *)(&temp_state), 0, &button_all_task_handle);
    }
    else
    {
        vTaskDelete(button_all_task_handle);
        xTaskCreate(button_all_task, "button_all_task", 180, (void *)(&temp_state), 0, &button_all_task_handle);
    }
}

/// @brief button reset
/// @param
void button_reset(void)
{
    static unsigned char temp_state;
    temp_state = 0;
    if (button_all_task_handle == NULL)
    {
        xTaskCreate(button_all_task, "button_all_task", 180, (void *)(&temp_state), 0, &button_all_task_handle);
    }
    else
    {
        vTaskDelete(button_all_task_handle);
        xTaskCreate(button_all_task, "button_all_task", 180, (void *)(&temp_state), 0, &button_all_task_handle);
    }
}

/// @brief EMERGENCY STOP
/// @param
void button_stop(void)
{
    if (brush_fb_123_task_handle != NULL)
    {
        vTaskDelete(brush_fb_123_task_handle);
        brush_fb_123_task_handle = NULL;
    }
    if (brush_ud_123_task_handle != NULL)
    {
        vTaskDelete(brush_ud_123_task_handle);
        brush_ud_123_task_handle = NULL;
    }
    if (all_arm_start_task_handle != NULL)
    {
        vTaskDelete(all_arm_start_task_handle);
        all_arm_start_task_handle = NULL;
    }
    if (button_all_task_handle != NULL)
    {
        vTaskDelete(button_all_task_handle);
        button_all_task_handle = NULL;
    }
    if (self_cali_task_handle != NULL)
    {
        vTaskDelete(self_cali_task_handle);
        self_cali_task_handle = NULL;
    }
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        motor_stop(MOTOR_STATION[i]);
        MOTOR_STATE_TABLE[i] = STOP;
        MOTOR_BK_OFF[i]();
    }
    allocate_en(0);
    for (int i = 0; i < 7; i++)
    {
        PUMP_OFF[i]();
    }

    for (int i = 0; i < 3; i++)
    {
        DRUM_OFF[i]();
    }
}

/// @brief Stop the motor by detecting the limit switch
static void limit_stop(char flag_cali)
{
    // Determination of limit switches for the two motors of the first set of roller brushes
    for (int i = 0; i < 8; i++)
    {
        if (MOTOR_STATE_TABLE[i] == RUNNING_ORIGIN)
        {
            if (MOTOR_LIMIT_FLAG[((2 * i) + 1)]() == GPIO_PIN_RESET)
            {
                MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
                MOTOR_STATE_TABLE[i] = STOP;
                MOTOR_BK_OFF[i]();
                motor_stop(MOTOR_STATION[i]);
                if (flag_cali == 1)
                {
                    MOTOR_ORIGIN_PULSE[i] = motor_read_position(MOTOR_STATION[i]);
                }
            }
        }
        else if (MOTOR_STATE_TABLE[i] == RUNNING_TARG)
        {
            if (MOTOR_LIMIT_FLAG[i * 2]() == GPIO_PIN_RESET)
            {
                MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
                MOTOR_STATE_TABLE[i] = STOP;
                MOTOR_BK_OFF[i]();
                motor_stop(MOTOR_STATION[i]);
            }
        }
    }
}

/// @brief Stop the motor by detecting the in place state
// static void reached_stop()
//{
//     static char MOTOR_REACHED_STAT_TABLE[MOTOR_MAX_NUM] = {0};
//     for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
//     {
//         if (MOTOR_STATE_TABLE[i] != STOP)
//         {
//             if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
//             {
//                 MOTOR_REACHED_STAT_TABLE[i]++;
//                 if (MOTOR_REACHED_STAT_TABLE[i] == 2)
//                 {
//                     MOTOR_REACHED_STAT_TABLE[i] = 0;
//                     if (MOTOR_STATE_TABLE[i] == RUNNING_TARG)
//                     {
//                         MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
//                     }
//                     else if (MOTOR_STATE_TABLE[i] == RUNNING_ORIGIN)
//                     {
//                         MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
//                     }
//                     printf("\r\nmoto reached--%d\r\n", i);
//                     MOTOR_STATE_TABLE[i] = STOP;
//                     MOTOR_BK_OFF[i]();
//                 }
//             }
//             else
//             {
//                 MOTOR_REACHED_STAT_TABLE[i] = 0;
//             }
//         }
//     }
// }

/// @brief Self calibration mode 1
static void calibration_1()
{
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        MOTOR_ORIGIN_PULSE[i] = motor_read_position(MOTOR_STATION[i]);
    }
}

/// @brief Self calibration mode 2
static void calibration_2()
{
    for (int i = MOTOR_UD_1; i < MOTOR_UD_1 + 3; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_BK_ON[i]();
        }
    }
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    for (int i = MOTOR_UD_1; i < MOTOR_UD_1 + 3; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_STATE_TABLE[i] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
            motor_set_pulse(MOTOR_STATION[i], -5000000, 10000);
        }
    }

    while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] != ORIGIN))
    {
        limit_stop(1);
        vTaskDelay(5 / portTICK_RATE_MS);
    }

    for (int i = MOTOR_FB_1; i < MOTOR_FB_1 + 3; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_BK_ON[i]();
        }
    }
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    for (int i = MOTOR_FB_1; i < MOTOR_FB_1 + 3; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_STATE_TABLE[i] = RUNNING_ORIGIN;
            MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
            motor_set_pulse(MOTOR_STATION[i], -5000000, 2000);
        }
    }

    while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] != ORIGIN))
    {
        limit_stop(1);
        vTaskDelay(5 / portTICK_RATE_MS);
    }

    if (MOTOR_BARM_B_FLAG() != SET)
    {
        MOTOR_BK_ON[MOTOR_B_ARM]();
        vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
        MOTOR_STATE_TABLE[MOTOR_B_ARM] = RUNNING_ORIGIN;
        MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] = MIDDLE;
        motor_set_pulse(MOTOR_STATION[MOTOR_B_ARM], 5000000, 400);
    }
    while (MOTOR_BARM_B_FLAG() != SET)
    {
        limit_stop(1);
        vTaskDelay(2 / portTICK_RATE_MS);
    }
    if (MOTOR_SARM_D_FLAG() != SET)
    {
        MOTOR_BK_ON[MOTOR_S_ARM]();
        vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
        MOTOR_STATE_TABLE[MOTOR_S_ARM] = RUNNING_ORIGIN;
        MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] = MIDDLE;
        motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], -5000000, 2000);
    }
    while (MOTOR_SARM_D_FLAG() != SET)
    {
        limit_stop(1);
        vTaskDelay(2 / portTICK_RATE_MS);
    }
    vTaskDelay(200 / portTICK_RATE_MS);
    calibration_1();
    vTaskDelay(200 / portTICK_RATE_MS);
    self_cali_task_handle = NULL;
    vTaskDelete(NULL);
}

/// @brief Self calibration
/// @param type
void self_calibration(unsigned char type)
{
    if (type == 1)
    {
        calibration_1();
    }
    else
    {
        calibration_2();
    }
}

void brush_ud_1_negative_pulse()
{
    // motor_en(MOTOR_STATION[MOTOR_UD_1]);
    // MOTOR_BK_ON[MOTOR_UD_1]();
    // vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    // if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_1], MOTOR_ORIGIN_PULSE[MOTOR_UD_1] - 5000000, UD_SPEED) == HAL_OK)
    // {
    //     MOTOR_STATE_TABLE[MOTOR_UD_1] = RUNNING_ORIGIN;
    //     MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] = MIDDLE;
    //     elog_d("BRUSH", "set motor 4 pulse ok");
    // }
    // else
    // {
    //     MOTOR_BK_OFF[MOTOR_UD_1]();
    //     elog_e("BRUSH", "set motor 4 pulse fail");
    // }

    // self_cali_task_handle
    if (self_cali_task_handle == NULL)
    {
        xTaskCreate((TaskFunction_t)calibration_2, "calibration task", 200, NULL, 0, &self_cali_task_handle);
    }
}

void err_deal()
{
    button_stop();
    for (int i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (MOTOR_ERR_CODE_TABLE[i] != 0)
        {
            motor_clear_err(MOTOR_STATION[i]);
        }
    }
    vTaskDelay(5000 / portTICK_RATE_MS);
    for (int i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (MOTOR_ERR_CODE_TABLE[i] != 0)
        {
            motor_set_Position_Mode(MOTOR_STATION[i]);
            MOTOR_ERR_CODE_TABLE[i] = 0;
        }
    }

    // unsigned char err_code = 0;
    // static int temp_err_cnt[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // for (MOTOR_NUM i = MOTOR_FB_1; i < 7; i++)
    // {
    //     motor_read_errcode(MOTOR_STATION[i], &err_code);
    //     if (err_code != 0)
    //     {
    //         elog_e("ERR-DEAL", "motor %d error--->%d", i, err_code);
    //     }
    // }

    // for (MOTOR_NUM i = MOTOR_FB_1; i < 7; i++)
    // {
    //     motor_read_errcode(MOTOR_STATION[i], &err_code);
    //     MOTOR_ERR_CODE_TABLE[i] = err_code;
    //     //        printf("%d---%d\r\n",i,err_code);
    //     if (err_code != 0)
    //     {
    //         MOTOR_BK_OFF[i]();

    //         if (MOTOR_ERR_CNT[i] <= MOTOR_ERR_MAX)
    //         {
    //             motor_clear_err(MOTOR_STATION[i]);

    //             elog_e("ERR-DEAL", "motor %d error--->%d", i, err_code);

    //             MOTOR_ERR_CNT[i] += 1;

    //             temp_err_cnt[i] = MOTOR_STATE_TABLE[i];
    //         }

    //         MOTOR_STATE_TABLE[i] = STOP;
    //     }
    // }

    // for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    // {
    //     if (temp_err_cnt[i] != -1)
    //     {
    //         if (motor_set_Position_Mode(MOTOR_STATION[i]) == HAL_OK)
    //         {
    //             vTaskDelay(100 / portTICK_RATE_MS);
    //             elog_w("BRUSH", "motor %d set p mode", i);
    //             if (temp_err_cnt[i] != STOP)
    //             {
    //                 switch (i)
    //                 {
    //                 case MOTOR_FB_1:
    //                     brush_fb_1(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_FB_2:
    //                     brush_fb_2(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_FB_3:
    //                     brush_fb_3(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_UD_1:
    //                     brush_ud_1(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_UD_2:
    //                     brush_ud_2(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_UD_3:
    //                     brush_ud_3(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_B_ARM:
    //                     big_arm_start(temp_err_cnt[i]);
    //                     break;
    //                 case MOTOR_S_ARM:
    //                     small_arm_start(temp_err_cnt[i]);
    //                     break;
    //                 }
    //             }

    //             temp_err_cnt[i] = -1;
    //         }
    //     }
    // }
}

/// @brief Obtain system status and prepare to upload data packets
/// @param data
void brush_get_state(unsigned char *data)
{
    data[0] = !EMERGENCY_KEY_FLAG();
    data[1] = !ANTI_COLLISION_FLAG();
    for (int i = 0; i < 8; i++)
    {
        data[i + 2] = 0;
        switch (MOTOR_IN_PLACE_STAT_TABLE[i])
        {
        case ORIGIN:
            data[i + 2] |= 0X80;
            break;
        case TARG:
            data[i + 2] |= 0X40;
            break;
        case MIDDLE:
            data[i + 2] |= 0X20;
            break;
        }
        data[i + 2] |= MOTOR_ERR_CODE_TABLE[i];
    }
    for (int i = 0; i < 3; i++)
    {
        data[10 + i] = DRUM_STAT[i]();
    }

    for (int i = 0; i < 7; i++)
    {
        data[13 + i] = PUMP_STAT[i]();
    }

    get_liquid_level(&data[20], &data[21], &data[22], NULL);
}

unsigned char pre_cmd = 0;
/// @brief Record the last instruction on the system to restore the state
/// @param cmd
void motor_record_stat(unsigned char cmd)
{
    if ((cmd == 0x02) || (cmd == 0x03))
    {
        pre_cmd = cmd;
    }
}

/// @brief Restore system operation status
// static void motor_restore_stat()
//{
//     if (pre_cmd == 0x02)
//     {
//         button_start();
//     }
//     else if (pre_cmd == 0x03)
//     {
//         button_reset();
//     }
// }

/// @brief Daemon Thread
/// @param
void brush_deamon_task(void)
{
    static int temp_emer_state = 0;
    //    static int temp_anti_state = 1;
    motor_hal_can_init();
    // motor_ctrol_en();
    for (int i = 0; i < 6; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_IN_PLACE_STAT_TABLE[i] = MIDDLE;
        }
    }

    motor_wait_en();
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (motor_set_Position_Mode(MOTOR_STATION[i]) != HAL_OK)
        {
            elog_e("BRUSH", "set motor %d position mode fail", i + 1);
        }
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    // motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], 0, 0);

    self_calibration(1);

    for (int i = 0; i < 8; i++)
    {
        elog_w("ORI", "--%ld", MOTOR_ORIGIN_PULSE[i]);
    }
    elog_w("ORI", "-----------------------------------");

    while (1)
    {
        limit_stop(0);

        // reached_stop();
        // err_deal();

        if (temp_emer_state != EMERGENCY_KEY_FLAG())
        {
            temp_emer_state = EMERGENCY_KEY_FLAG();
            if (temp_emer_state == 1)
            {
                button_stop();
            }
            // else
            // {
            //     motor_restore_stat();
            // }
        }

        // if (temp_anti_state != ANTI_COLLISION_FLAG())
        // {
        //     temp_anti_state = ANTI_COLLISION_FLAG();
        //     if (temp_anti_state == 0)
        //     {
        //         button_stop();
        //     }
        // }

        // printf("position->%ld\r\n",motor_read_position(0x601));
        // printf("%d\r\n",motor_read_target_reached(0x601));
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}
