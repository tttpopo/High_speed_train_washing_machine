#include "console.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

TaskHandle_t console_task_handle = NULL;
// Chain head
fun_list_t *CS_LIST_HEAD = NULL;
// recv buf
unsigned char console_rx_buf[50];

/// @brief Enable serial port dma reception
static void cs_recv_start()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, console_rx_buf, sizeof(console_rx_buf));
}

/// @brief This function needs to be placed in the callback function of the serial port idle interrupt
/// @param Size data size
void cs_recv_callback(unsigned short Size)
{
    xTaskNotifyFromISR(console_task_handle, Size, eSetValueWithOverwrite, NULL);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, console_rx_buf, sizeof(console_rx_buf));
}

/// @brief Initialize the chain header and enable serial port reception
/// @return
Console_StatusTypeDef cs_init()
{
    cs_recv_start();
    CS_LIST_HEAD = (fun_list_t *)malloc(sizeof(fun_list_t));
    if (CS_LIST_HEAD != NULL)
    {
        CS_LIST_HEAD->cmd = NULL;
        CS_LIST_HEAD->next = NULL;
        return CONSOLE_OK;
    }
    else
    {
        elog_e("CONSOLE", "init fail");
        return CONSOLE_ERROR;
    }
}

/// @brief Registering Command Words and Functions
/// @param cmd command
/// @param fun function
/// @return
Console_StatusTypeDef cs_reg_fun(unsigned char *cmd, CS_FUN fun)
{
    fun_list_t *now = NULL;
    now = CS_LIST_HEAD;

    while (now->next != NULL)
    {
        if (strcmp((const char *)cmd, (const char *)now->next->cmd) == 0)
        {
            elog_e("CONSOLE", "Same name");
            return CONSOLE_OK;
        }
        now = now->next;
    }

    now->next = (fun_list_t *)malloc(sizeof(fun_list_t));

    if (now->next == NULL)
    {
        elog_e("CONSOLE", "malloc fail");
        return CONSOLE_ERROR;
    }

    now->next->cmd = cmd;
    now->next->fun = fun;

    return CONSOLE_OK;
}

/// @brief Unregistered command
/// @param cmd
/// @return
Console_StatusTypeDef cs_dereg_fun(unsigned char *cmd)
{
    return CONSOLE_OK;
}

/// @brief Call this callback function when receiving data
/// @param cmd
/// @return
static Console_StatusTypeDef cs_fun_callback(unsigned char *cmd)
{
    unsigned char index = 0;
    fun_list_t *now = NULL;
    now = CS_LIST_HEAD;
    while (now->next != NULL)
    {
        while (1)
        {
            if (now->next->cmd[index] == cmd[index])
            {
                index++;
                continue;
            }
            else
            {
                if (now->next->cmd[index] == '\0')
                {
                    now->next->fun(cmd);
                    return CONSOLE_OK;
                }
                else
                {
                    break;
                }
            }
        }
        index = 0;
        now = now->next;
    }
    elog_e("CONSOLE", "Unknown command");
    return CONSOLE_ERROR;
}

/// @brief List currently existing commands
void cs_display_list(unsigned char *cmd)
{
    fun_list_t *now = NULL;
    now = CS_LIST_HEAD;
    while (now->next != NULL)
    {
        printf(" -> %s\r\n", now->next->cmd);
        now = now->next;
    }
}

/// @brief console client task
unsigned char temp_cnt = 0;
void console_task()
{
    unsigned char temp_buf[sizeof(console_rx_buf)] = {0};
    unsigned int size = 0;
    cs_reg_fun("list", cs_display_list);
    while (1)
    {
        xTaskNotifyWait(0, 0, &size, portMAX_DELAY);
        memcpy(&(temp_buf[temp_cnt]), console_rx_buf, size);
        temp_cnt += size;
         
        if ((console_rx_buf[size - 1] == '\r') || (console_rx_buf[size - 1] == '\n'))
        {
            printf("\r\n");
            if ((temp_cnt != 1) && (temp_cnt < sizeof(console_rx_buf)))
            {
                cs_fun_callback(temp_buf);
                memset(temp_buf, 0, sizeof(console_rx_buf));
            }
            printf("\r\n>>");
            temp_cnt = 0;
        }
        else if (console_rx_buf[size - 1] == '\b')
        {
            if (temp_cnt > 0)
            {
                temp_cnt -= 2;
            }

            printf("\b \b");
        }
        else
        {
            printf("%c", console_rx_buf[size - 1]);
        }
    }
}
