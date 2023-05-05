#include "ws2812.h"
#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

#define RGB_DELAY vTaskDelay(5 / portTICK_RATE_MS);
unsigned int Pixel_Buf[Pixel_NUM + 1][24] = {0};
TaskHandle_t rgb_task_handle;

void RGB_SetColor(unsigned char LedId, unsigned char R, unsigned char G, unsigned char B)
{
    unsigned char i;
    if (LedId > Pixel_NUM)
        return;

    for (i = 0; i < 8; i++)
        Pixel_Buf[LedId][i] = ((G & (1 << (7 - i))) ? (CODE_1) : CODE_0);
    for (i = 8; i < 16; i++)
        Pixel_Buf[LedId][i] = ((R & (1 << (15 - i))) ? (CODE_1) : CODE_0);
    for (i = 16; i < 24; i++)
        Pixel_Buf[LedId][i] = ((B & (1 << (23 - i))) ? (CODE_1) : CODE_0);
}

void Reset_Load(void)
{
    unsigned char i;
    for (i = 0; i < 24; i++)
    {
        Pixel_Buf[Pixel_NUM][i] = 0;
    }
}

void RGB_SendArray(void)
{
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (unsigned int *)Pixel_Buf, (Pixel_NUM + 1) * 24);
}

void rgb_test_fun(void)
{
    Reset_Load();
    for (int i = 0; i < 3; i++)
    {
        RGB_SetColor(i, 0, 0, 255);
    }
    RGB_SendArray();
    for (int x = 0; x < 24; x++)
    {
        printf("%d-", Pixel_Buf[0][x]);
    }
    printf("\r\n\r\n");
    for (int x = 0; x < 24; x++)
    {
        printf("%d-", Pixel_Buf[1][x]);
    }
    printf("\r\n\r\n");
    for (int x = 0; x < 24; x++)
    {
        printf("%d-", Pixel_Buf[2][x]);
    }
    printf("\r\n\r\n");
    for (int x = 0; x < 24; x++)
    {
        printf("%d-", Pixel_Buf[3][x]);
    }

    printf("\r\n\r\n");
    for (int x = 0; x < 24; x++)
    {
        printf("%d-", Pixel_Buf[4][x]);
    }

    printf("\r\n\r\n");
}

static void r_g_b_breath_program()
{
    for (int i = 0; i < 255; i++)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, i, 0, 0);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
    for (int i = 255; i > 0; i--)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, i, 0, 0);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
    for (int i = 0; i < 255; i++)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, 0, i, 0);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
    for (int i = 255; i > 0; i--)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, 0, i, 0);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
    for (int i = 0; i < 255; i++)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, 0, 0, i);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
    for (int i = 255; i > 0; i--)
    {
        for (int j = 0; j < Pixel_NUM; j++)
        {
            RGB_SetColor(j, 0, 0, i);
        }
        RGB_SendArray();
        RGB_DELAY;
    }
}

//static void tetris_program()
//{
//    for (int j = 0; j < Pixel_NUM; j++)
//    {
//        for (int i = 0; i < Pixel_NUM; i++)
//        {
//            RGB_SetColor(i, 0, 0, 0);
//        }
//        RGB_SetColor(j, 0, 255, 0);
//        RGB_SendArray();
//        vTaskDelay(10 / portTICK_RATE_MS);
//    }
//}

//static void meteor_program()
//{
//    static int x = 0;
//    for (int i = 0; i < Pixel_NUM; i++)
//    {
//        RGB_SetColor(i, 0, 0, 0);
//    }
//    RGB_SetColor(x, 10, 0, 0);
//    RGB_SetColor(x + 1, 30, 0, 0);
//    RGB_SetColor(x + 2, 60, 0, 0);
//    RGB_SetColor(x + 3, 90, 0, 0);
//    RGB_SetColor(x + 4, 130, 0, 0);
//    RGB_SetColor(x + 5, 254, 0, 0);
//    RGB_SendArray();

//    x++;
//    x %= Pixel_NUM;
//    vTaskDelay(70 / portTICK_RATE_MS);
//}

void rgb_task()
{
    Reset_Load();
    while (1)
    {
        // tetris_program();
        // meteor_program();
        r_g_b_breath_program();
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3)
    {
        HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
    }
}
