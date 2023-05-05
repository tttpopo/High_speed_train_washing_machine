#ifndef __MOTOR_DRIVE_H_
#define __MOTOR_DRIVE_H_

#include "main.h"
#include "motor_hal.h"

/*
 ===============================================================================
                 ##### control functions #####
 ===============================================================================
 */

HAL_StatusTypeDef motor_set_Position_Mode(unsigned int s_id);
HAL_StatusTypeDef motor_set_Velocity_Mode(unsigned int s_id);
HAL_StatusTypeDef motor_set_speed(unsigned int s_id, long int speed);
HAL_StatusTypeDef motor_set_pulse(unsigned int s_id, long int pulse, long int speed);
long int motor_read_position(unsigned int s_id);
HAL_StatusTypeDef motor_read_target_reached(unsigned int s_id);
HAL_StatusTypeDef motor_clear_err(unsigned int s_id);
HAL_StatusTypeDef motor_read_errcode(unsigned int s_id, unsigned char *err_code);
HAL_StatusTypeDef motor_stop(unsigned int s_id);
void motor_task(void);

#endif
