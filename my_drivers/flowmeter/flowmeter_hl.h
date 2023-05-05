#include "main.h"
#include "gpio.h"

#ifndef _FLOWMETER_H_
#define _FLOWMETER_H_

int fm_get_total_flow(float *fm_1, float *fm_2);
void fm_recv_callback(unsigned short Size);
void flowmeter_task(void);

#endif
