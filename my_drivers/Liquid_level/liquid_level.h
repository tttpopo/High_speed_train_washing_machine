#ifndef _LIQUID_LEVEL_H_
#define _LIQUID_LEVEL_H_

void liquid_level_task(void);
void get_liquid_level(unsigned char *l_1, unsigned char *l_2, unsigned char *l_3, unsigned char *l_4);

// accurate ratio part
void accurate_ratio_task(void);
void allocate_en(char en);
void set_ratio(float ratio);
#endif
