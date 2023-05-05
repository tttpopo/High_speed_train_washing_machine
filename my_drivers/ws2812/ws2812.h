#ifndef _WS2812_H_
#define _WS2812_H_

#include "main.h"

#define CODE_1 (60) // 1码定时器计数次数
#define CODE_0 (17) // 0码定时器计数次数

typedef struct
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
} RGB_Color_TypeDef;

#define Pixel_NUM 105

void RGB_SetColor(unsigned char LedId, unsigned char R, unsigned char G, unsigned char B);
void Reset_Load(void);
void RGB_SendArray(void);
void rgb_test_fun(void);
void rgb_task(void);

#endif
