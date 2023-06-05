#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor_drive.h"
#include "brush.h"
#include "flashdb.h"
#include "console.h"
#include "string.h"
#include "liquid_level.h"
// extern unsigned int MOTOR_ERR_ID;

TaskHandle_t brush_deamon_task_handle = NULL;
TaskHandle_t brush_fb_123_task_handle = NULL;
TaskHandle_t brush_ud_123_task_handle = NULL;
TaskHandle_t all_arm_start_task_handle = NULL;
TaskHandle_t button_all_task_handle = NULL;

// void (*RUNNING_FUN)(void) = NULL;

long int FB_TARG_PULSE = 1200000; // Translation brush group end point pulse value
// long int FB1_ORIGIN_PULSE = 0;  // Translation brush group end point pulse value
// long int FB2_ORIGIN_PULSE = 0;
// long int FB3_ORIGIN_PULSE = 0;
// long int UD_TARG_PULSE = 50000;    // Lifting brush group end point pulse value
long int UD1_TARG_PULSE = 1500000; // Lifting brush group end point pulse value
long int UD2_TARG_PULSE = 2700000; // Lifting brush group end point pulse value
long int UD3_TARG_PULSE = 4700000; // Lifting brush group end point pulse value
// long int UD1_ORIGIN_PULSE = 0;  // Lifting brush group end point pulse value
// long int UD2_ORIGIN_PULSE = 0;
// long int UD3_ORIGIN_PULSE = 0;
// long int B_ARM_ORIGIN_PULSE = 0;
// long int S_ARM_ORIGIN_PULSE = 0;

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

/// @brief Control brush group 1 to move forward and backward
/// @param state
void brush_fb_1(unsigned char state)
{
    MOTOR_BK_ON[MOTOR_FB_1]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_1], FB_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_1], FB_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_1], MOTOR_ORIGIN_PULSE[MOTOR_FB_1], FB_SPEED) == HAL_OK)
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
    MOTOR_BK_ON[MOTOR_FB_2]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_2], FB_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_2], FB_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_2], MOTOR_ORIGIN_PULSE[MOTOR_FB_2], FB_SPEED) == HAL_OK)
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
    MOTOR_BK_ON[MOTOR_FB_3]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_3], FB_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_FB_3], FB_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_FB_3], MOTOR_ORIGIN_PULSE[MOTOR_FB_3], FB_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_1], MOTOR_ORIGIN_PULSE[MOTOR_UD_1], UD_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_2], MOTOR_ORIGIN_PULSE[MOTOR_UD_2], UD_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_UD_3], MOTOR_ORIGIN_PULSE[MOTOR_UD_3], UD_SPEED) == HAL_OK)
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

void water_pump_set(unsigned char *data)
{
    if (data[0] == 1)
    {
        PUMP_1_ON();
    }
    else
    {
        PUMP_1_OFF();
    }

    if (data[1] == 1)
    {
        PUMP_2_ON();
    }
    else
    {
        PUMP_2_OFF();
    }

    if (data[2] == 1)
    {
        PUMP_3_ON();
    }
    else
    {
        PUMP_3_OFF();
    }

    if (data[3] == 1)
    {
        PUMP_4_ON();
    }
    else
    {
        PUMP_4_OFF();
    }
}

/// @brief start big arm motor
/// @param state
void big_arm_start(unsigned char state)
{
    MOTOR_BK_ON[MOTOR_B_ARM]();
    vTaskDelay(MOTOR_BK_DELAY / portTICK_RATE_MS);
    if (state)
    {
        if (motor_set_pulse(MOTOR_STATION[MOTOR_B_ARM], B_ARM_TARG_PULSE + MOTOR_ORIGIN_PULSE[MOTOR_B_ARM], B_AEM_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_B_ARM], MOTOR_ORIGIN_PULSE[MOTOR_B_ARM], B_AEM_SPEED) == HAL_OK)
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
        if (motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], MOTOR_ORIGIN_PULSE[MOTOR_S_ARM], S_ARM_SPEED) == HAL_OK)
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
    switch (*((unsigned char *)state))
    {
    case 1:
        // go to targ
        small_arm_start(1);
        while (MOTOR_STATE_TABLE[MOTOR_S_ARM] != STOP)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        big_arm_start(1);
        break;
    case 0:
        // go to origin
        big_arm_start(0);
        while (MOTOR_STATE_TABLE[MOTOR_B_ARM] != STOP)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        small_arm_start(0);
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
    elog_e("TEST", "button all task is start");
    switch (*((unsigned char *)state))
    {
    case 1:
        // small_arm_start(1);
        // while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_S_ARM] != TARG)
        // {
        //     vTaskDelay(100 / portTICK_RATE_MS);
        // }
        big_arm_start(1);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != TARG)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }

        brush_fb_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_3(1);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_2] != TARG) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_1] != TARG))
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }

        brush_ud_1(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(1);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_3(1);
        break;
    case 0:
        brush_ud_1(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_2(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_ud_3(0);

        while ((MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_1] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_2] != ORIGIN) || (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_UD_3] != ORIGIN))
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }

        brush_fb_1(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_2(0);
        vTaskDelay(BRUSH_START_DELAY_TIME / portTICK_RATE_MS);
        brush_fb_3(0);

        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_FB_3] != ORIGIN)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }

        big_arm_start(0);
        while (MOTOR_IN_PLACE_STAT_TABLE[MOTOR_B_ARM] != ORIGIN)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        // small_arm_start(0);
        break;
    }
    elog_e("TEST", "button all task is delete");
    button_all_task_handle = NULL;
    vTaskDelete(NULL);
}

/// @brief button start
/// @param
void button_start(void)
{
    static unsigned char temp_state;
    temp_state = 1;
    if (button_all_task_handle == NULL)
    {
        xTaskCreate(button_all_task, "button_all_task", 150, (void *)(&temp_state), 0, &button_all_task_handle);
    }
    else
    {
        vTaskDelete(button_all_task_handle);
        xTaskCreate(button_all_task, "button_all_task", 150, (void *)(&temp_state), 0, &button_all_task_handle);
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
        xTaskCreate(button_all_task, "button_all_task", 150, (void *)(&temp_state), 0, &button_all_task_handle);
    }
    else
    {
        vTaskDelete(button_all_task_handle);
        xTaskCreate(button_all_task, "button_all_task", 150, (void *)(&temp_state), 0, &button_all_task_handle);
    }
}

/// @brief EMERGENCY STOP
/// @param
void button_stop(void)
{
    for (int i = 0; i < 4; i++)
    {
        PUMP_OFF[i]();
    }

    for (int i = 0; i < 3; i++)
    {
        DRUM_OFF[i]();
    }

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

    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        motor_stop(MOTOR_STATION[i]);
        MOTOR_STATE_TABLE[i] = STOP;
        MOTOR_BK_OFF[i]();
    }
}

/// @brief Stop the motor by detecting the limit switch
static void limit_stop()
{
    // Determination of limit switches for the two motors of the first set of roller brushes
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (((MOTOR_LIMIT_FLAG[i * 2]() == SET) && (MOTOR_STATE_TABLE[i] == RUNNING_TARG)) || ((MOTOR_LIMIT_FLAG[i * 2 + 1]() == SET) && (MOTOR_STATE_TABLE[i] == RUNNING_ORIGIN)))
        {
            if (MOTOR_STATE_TABLE[i] == RUNNING_TARG)
            {
                MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
            }
            else if (MOTOR_STATE_TABLE[i] == RUNNING_ORIGIN)
            {
                MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
            }
            MOTOR_STATE_TABLE[i] = STOP;
            motor_stop(MOTOR_STATION[i]);
            MOTOR_BK_OFF[i]();
        }
    }
}

/// @brief Stop the motor by detecting the in place state
static void reached_stop()
{
    static char MOTOR_REACHED_STAT_TABLE[MOTOR_MAX_NUM] = {0};
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (MOTOR_STATE_TABLE[i] != STOP)
        {
            if (motor_read_target_reached(MOTOR_STATION[i]) == HAL_OK)
            {
                MOTOR_REACHED_STAT_TABLE[i]++;
                if (MOTOR_REACHED_STAT_TABLE[i] == 2)
                {
                    MOTOR_REACHED_STAT_TABLE[i] = 0;
                    if (MOTOR_STATE_TABLE[i] == RUNNING_TARG)
                    {
                        MOTOR_IN_PLACE_STAT_TABLE[i] = TARG;
                    }
                    else if (MOTOR_STATE_TABLE[i] == RUNNING_ORIGIN)
                    {
                        MOTOR_IN_PLACE_STAT_TABLE[i] = ORIGIN;
                    }
                    printf("\r\nmoto reached--%d\r\n", i);
                    MOTOR_STATE_TABLE[i] = STOP;
                    MOTOR_BK_OFF[i]();
                }
            }
            else
            {
                MOTOR_REACHED_STAT_TABLE[i] = 0;
            }
        }
    }
}

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
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (MOTOR_LIMIT_FLAG[i * 2 + 1]() != SET)
        {
            MOTOR_BK_ON[i]();
            vTaskDelay(MOTOR_BK_DELAY / portTICK_PERIOD_MS);
            motor_set_pulse(MOTOR_STATION[i], -10000000, FB_SPEED);
            MOTOR_STATE_TABLE[i] = RUNNING_ORIGIN;
        }
    }
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        limit_stop();
        // cnt += MOTOR_STAT;
        if (MOTOR_STATE_TABLE[i] != STOP)
        {
            i = MOTOR_FB_1;
        }
    }
    calibration_1();
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
        calibration_1();
    }
}

/// @brief Self calibration mode 1
static void err_deal()
{
    unsigned char err_code = 0;
    static int temp_err_cnt[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    for (MOTOR_NUM i = MOTOR_FB_1; i < 7; i++)
    {
        motor_read_errcode(MOTOR_STATION[i], &err_code);
        if (err_code != 0)
        {
            elog_e("ERR-DEAL", "motor %d error--->%d", i, err_code);
        }
    }

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

    for (int i = 0; i < 8; i++)
    {
        data[i + 1] = 0;

        switch (MOTOR_IN_PLACE_STAT_TABLE[i])
        {
        case ORIGIN:
            data[i + 1] |= 0X80;
            break;
        case TARG:
            data[i + 1] |= 0X40;
            break;
        case MIDDLE:
            data[i + 1] |= 0X20;
            break;
        }

        data[i + 1] |= MOTOR_ERR_CODE_TABLE[i];
    }

    for (int i = 0; i < 3; i++)
    {
        data[9 + i] = !DRUM_STAT[i]();
    }

    for (int i = 0; i < 4; i++)
    {
        data[12 + i] = PUMP_STAT[i]();
    }

    get_liquid_level(&data[16], &data[17], &data[18], NULL);
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
static void motor_restore_stat()
{
    if (pre_cmd == 0x02)
    {
        button_start();
    }
    else if (pre_cmd == 0x03)
    {
        button_reset();
    }
}

/// @brief Daemon Thread
/// @param
void brush_deamon_task(void)
{
    static int temp_emer_state = 1;
    motor_hal_can_init();
    // motor_wait_en();
    int i = 0;
    for (MOTOR_NUM i = MOTOR_FB_1; i < MOTOR_MAX_NUM; i++)
    {
        if (motor_set_Position_Mode(MOTOR_STATION[i]) != HAL_OK)
        {
            elog_e("BRUSH", "set motor %d position mode fail", i + 1);
        }
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    motor_set_pulse(MOTOR_STATION[MOTOR_S_ARM], 0, 0);

    self_calibration(1);

    while (1)
    {
        i++;
        limit_stop();
        ///////////////////
        // if (i == 2)
        // {
        //     i = 0;
        reached_stop();
        err_deal();
        // }
        ///////////////////
        ///////////////////
        if (temp_emer_state != EMERGENCY_KEY_FLAG())
        {
            temp_emer_state = EMERGENCY_KEY_FLAG();
            if (temp_emer_state == 0)
            {
                button_stop();
            }
            // else
            // {
            //     motor_restore_stat();
            // }
        }
        // printf("position->%ld\r\n",motor_read_position(0x601));
        // printf("%d\r\n",motor_read_target_reached(0x601));
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}
