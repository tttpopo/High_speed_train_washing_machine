#ifndef _BATTERY_H_
#define _BATTERY_H_


void bat_recv_callback(unsigned short Size);
void bat_get_power(char *data);
void battery_task(void);

#endif

