#include "motor_drive.h"
#include "elog.h"
#include "console.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char u3_recv_buf[50] = {0};
unsigned char u3_recv_flag = 0;
unsigned char ALL_STOP_FLAG = 0;

TaskHandle_t motor_task_handle;
extern TaskHandle_t brush_deamon_task_handle;
extern unsigned char MOTOR_REC_FLAG;
extern unsigned char can_rx_buf[10];
/*
 ===============================================================================
                 ##### control functions #####
 ===============================================================================
 */

// Set the node to an operational state
void motor_ctrol_en(void)
{
    unsigned char CMD_BUF[] = {0x01, 0x00};
    motor_send_data(0, CMD_BUF, 2);
}

HAL_StatusTypeDef motor_en(unsigned int s_id)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[][8] = {
        {0x2b, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00}
    };

    for (char i = 0; i < 3; i++)
    {
        ret = motor_send_data(s_id, &CMD_BUF[i][0], 8);
        if (ret != HAL_OK)
        {
            return ret;
        }
    }
    return ret;
}


/// @brief set position mode
/// @return
HAL_StatusTypeDef motor_set_Position_Mode(unsigned int s_id)
{
    HAL_StatusTypeDef ret;
    unsigned char CMD_BUF[][8] = {
        {0X01, 0X00},
        {0x2f, 0x60, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00},
        {0x23, 0x83, 0x60, 0x00, 0x88, 0x13, 0x00, 0x00},
        {0x23, 0x84, 0x60, 0x00, 0x88, 0x13, 0x00, 0x00},
        // {0x23, 0x83, 0x60, 0x00, 0x64, 0x00, 0x00, 0x00},
        // {0x23, 0x84, 0x60, 0x00, 0x64, 0x00, 0x00, 0x00},
        {0x2b, 0x93, 0x60, 0x01, 0x01, 0x00, 0x00, 0x00},
        {0x2b, 0x93, 0x60, 0x02, 0x01, 0x00, 0x00, 0x00},
    };
    motor_send_data(0, &CMD_BUF[0][0], 2);
    for (char i = 0; i < 5; i++)
    {
        ret = motor_send_data(s_id, &CMD_BUF[i+1][0], 8);
        if (ret != HAL_OK)
        {
            return ret;
        }
    }
    return ret;
}

/// @brief set velocity mode
/// @return
HAL_StatusTypeDef motor_set_Velocity_Mode(unsigned int s_id)
{
    HAL_StatusTypeDef ret;
    unsigned char CMD_BUF[][8] = {
        {0X01, 0X00},
        {0x2f, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00},
        {0x23, 0x83, 0x60, 0x00, 0xe8, 0x03, 0x00, 0x00},
        {0x23, 0x84, 0x60, 0x00, 0xe8, 0x03, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00},
    };

    motor_send_data(0, &CMD_BUF[0][0], 2);

    for (char i = 0; i < 6; i++)
    {
        ret = motor_send_data(s_id, &CMD_BUF[i+1][0], 8);
        if (ret != HAL_OK)
        {
            return ret;
        }
    }
    return ret;
}

/// @brief Set speed in speed mode
/// @param speed
/// @return
HAL_StatusTypeDef motor_set_speed(unsigned int s_id, long int speed)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[] = {0x23, 0xff, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
    CMD_BUF[4] = (unsigned char)speed;
    CMD_BUF[5] = (unsigned char)(speed >> 8);
    CMD_BUF[6] = (unsigned char)(speed >> 16);
    CMD_BUF[7] = (unsigned char)(speed >> 24);
    ret = motor_send_data(s_id, CMD_BUF, 8);
    return ret;
}

/// @brief In position mode, set the target position and speed
/// @param pulse
/// @param speed
/// @return
HAL_StatusTypeDef motor_set_pulse(unsigned int s_id, long int pulse, long int speed)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[][8] = {
        {0x23, 0x7a, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x23, 0x81, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x1f, 0x00, 0x00, 0x00},
        {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00},
    };

    CMD_BUF[0][4] = (unsigned char)pulse;
    CMD_BUF[0][5] = (unsigned char)(pulse >> 8);
    CMD_BUF[0][6] = (unsigned char)(pulse >> 16);
    CMD_BUF[0][7] = (unsigned char)(pulse >> 24);

    CMD_BUF[1][4] = (unsigned char)speed;
    CMD_BUF[1][5] = (unsigned char)(speed >> 8);
    CMD_BUF[1][6] = (unsigned char)(speed >> 16);
    CMD_BUF[1][7] = (unsigned char)(speed >> 24);

    for (char i = 0; i < 5; i++)
    {
        ret = motor_send_data(s_id, &CMD_BUF[i][0], 8);
        if (ret != HAL_OK)
        {
            return ret;
        }
    }
    return ret;
}

/// @brief read encoder absolute value
/// @return
long int motor_read_position(unsigned int s_id)
{
    HAL_StatusTypeDef ret;
    long int pos = 0;
    MOTOR_REC_FLAG = 0;
    unsigned char CMD_BUF[] = {0x40, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
    ret = motor_send_data(s_id, CMD_BUF, 8);
    if (ret != HAL_OK)
    {
        return 1314;
    }
    if (ret == HAL_OK)
    {
        pos |= can_rx_buf[7];
        pos <<= 8;
        pos |= can_rx_buf[6];
        pos <<= 8;
        pos |= can_rx_buf[5];
        pos <<= 8;
        pos |= can_rx_buf[7];
    }
    return pos;
}

/// @brief read target reached flag
/// @return
HAL_StatusTypeDef motor_read_target_reached(unsigned int s_id)
{
    HAL_StatusTypeDef ret;
    MOTOR_REC_FLAG = 0;
    unsigned char CMD_BUF[] = {0x40, 0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
    ret = motor_send_data(s_id, CMD_BUF, 8);
    if ((ret == HAL_OK) && (can_rx_buf[0] == 0x4b))
    {
        if (can_rx_buf[5] & (0x01 << 2))
        {
            return HAL_OK;
        }
        else
        {
            return HAL_BUSY;
        }
    }
    return ret;
}

/// @brief read error code
/// @param s_id
/// @param err_code
/// @return
HAL_StatusTypeDef motor_read_errcode(unsigned int s_id, unsigned char *err_code)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[] = {0x40, 0x12, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00};
    ret = motor_send_data(s_id, CMD_BUF, 8);
    if ((ret == HAL_OK) && (can_rx_buf[0] == 0x4b))
    {
        *err_code = can_rx_buf[4];
        return ret;
    }
    else if (ret != HAL_OK)
    {
        *err_code = 31;
        return ret;
    }

    *err_code = 0;
    return ret;
}

/// @brief Clear Error Codes
/// @param s_id
/// @return
HAL_StatusTypeDef motor_clear_err(unsigned int s_id)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[] = {0x2b, 0x03, 0x46, 0x00, 0x01};
    ret = motor_send_data(s_id, CMD_BUF, 5);
    if (ret != HAL_OK)
    {
        return ret;
    }
    return ret;
}

/// @brief stop motor
/// @param s_id
/// @return
HAL_StatusTypeDef motor_stop(unsigned int s_id)
{
    HAL_StatusTypeDef ret;

    unsigned char CMD_BUF[] = {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00};
    ret = motor_send_data(s_id, CMD_BUF, 8);
    if (ret != HAL_OK)
    {
        return ret;
    }
    return ret;
}

void cs_moto_set_velocity_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "\r");
    s_id = atoi(temp_p);
    printf("%d,%d\r\n", s_id, MOTOR_STATION[s_id]);
    if (motor_set_Velocity_Mode(MOTOR_STATION[s_id]) == HAL_OK)
    {
        elog_d("MOTOR", "motor set ok");
    }
    else
    {
        elog_e("MOTOR", "motor set fail");
    }
}

void cs_moto_set_Position_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "\r");
    s_id = atoi(temp_p);
    vTaskDelay(200 / portTICK_RATE_MS);
    if (motor_set_Position_Mode(MOTOR_STATION[s_id]) == HAL_OK)
    {
        elog_d("MOTOR", "motor set ok");
    }
    else
    {
        elog_e("MOTOR", "motor set fail");
    }
}

void cs_moto_start_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;
    long int speed = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "-");
    s_id = atoi(temp_p);
    temp_p = strtok(NULL, "\r");
    speed = atoi(temp_p);
    // MOTOR_FB1_BK_ON;
    if (motor_set_speed(MOTOR_STATION[s_id], speed) == HAL_OK)
    {
        elog_d("MOTOR", "motor set ok");
    }
    else
    {
        elog_e("MOTOR", "motor set fail");
    }
}

void cs_moto_set_pulse_cb(unsigned char *cmd)
{
    char *temp_p = NULL;

    unsigned int s_id = 0;
    long int speed = 0;
    long int pulse = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "-");
    s_id = atoi(temp_p);
    temp_p = strtok(NULL, "-");
    speed = atoi(temp_p);
    temp_p = strtok(NULL, "\r");
    pulse = atoi(temp_p);

    elog_d("MOTOR", "id = %d,pulse = %d,speed = %d", s_id, pulse, speed);
    MOTOR_BK_ON[s_id]();
    if (motor_set_pulse(MOTOR_STATION[s_id], pulse, speed) == HAL_OK)
    {
        elog_d("MOTOR", "motor set ok");
    }
    else
    {
        elog_e("MOTOR", "motor set fail");
    }
}

void cs_moto_restart_err_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "\r");
    s_id = atoi(temp_p);
    motor_clear_err(MOTOR_STATION[s_id]);
    elog_d("MOTOR", "restart");
}

void cs_moto_stop_cb(unsigned char *cmd)
{
    char *temp_p = NULL;
    unsigned int s_id = 0;

    temp_p = strtok((char *)cmd, "-");
    temp_p = strtok(NULL, "\r");
    s_id = atoi(temp_p);
    if (motor_stop(MOTOR_STATION[s_id]) == HAL_OK)
    {
        elog_d("MOTOR", "stop ok\r\n");
    }
    else
    {
        elog_e("MOTOR", "stop fail\r\n");
    }
}

void motor_task()
{
    // motor_wait_en();
    cs_reg_fun("vmode", cs_moto_set_velocity_cb);
    cs_reg_fun("pmode", cs_moto_set_Position_cb);
    cs_reg_fun("setv", cs_moto_start_cb);
    cs_reg_fun("setp", cs_moto_set_pulse_cb);
    cs_reg_fun("restart", cs_moto_restart_err_cb);
    cs_reg_fun("stop", cs_moto_stop_cb);
    vTaskDelete(NULL);
    while (1)
    {
        // motor_read_position(0x601, &pos);
        // elog_i("test", "%ld", pos);
        // if (motor_read_target_reached(0x601) != HAL_OK)
        // {
        //     elog_i("test", "moto no reached");
        // }
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
