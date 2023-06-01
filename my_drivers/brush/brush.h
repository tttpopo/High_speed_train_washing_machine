#ifndef _BRUSH_H_
#define _BRUSH_H_

#include "main.h"
#include "gpio.h"

void brush_fb_1(unsigned char state);
void brush_fb_2(unsigned char state);
void brush_fb_3(unsigned char state);
void brush_fb_123(unsigned char state);
void brush_ud_1(unsigned char state);
void brush_ud_2(unsigned char state);
void brush_ud_3(unsigned char state);
void brush_ud_123(unsigned char state);
void drum_revolve(unsigned char state);
void water_pump_set(unsigned char *data);
void big_arm_start(unsigned char state);
void small_arm_start(unsigned char state);
void all_arm_start(unsigned char state);
void button_start(void);
void button_reset(void);
void button_stop(void);
void brush_get_state(unsigned char *data);
void motor_record_stat(unsigned char cmd);
void brush_deamon_task(void);

#endif
