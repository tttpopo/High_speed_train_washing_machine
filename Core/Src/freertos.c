/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "motor_drive.h"
#include "can.h"
#include "adc.h"
#include "usart.h"
#include "elog.h"
#include "flashdb.h"
#include "tim.h"
#include "console.h"
#include "ws2812.h"
#include "liquid_level.h"
#include "hcp.h"
#include "brush.h"
#include "flowmeter_hl.h"
// #include "battery.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

TaskHandle_t test_task_handle;
extern TaskHandle_t console_task_handle;
extern TaskHandle_t motor_task_handle;
extern TaskHandle_t rgb_task_handle;
extern TaskHandle_t liquid_level_task_handle;
extern TaskHandle_t hcp_task_handle;
extern TaskHandle_t brush_deamon_task_handle;
extern TaskHandle_t flowmeter_task_handle;
extern TaskHandle_t battery_task_handle;
extern TaskHandle_t accurate_ratio_task_handle;

void test_task(void);

unsigned char u1_rec_buf[100];

// static uint32_t boot_count = 0;
// struct fdb_default_kv default_kv;
// static struct fdb_kvdb kvdb = {0};
// static struct fdb_default_kv_node default_kv_table[] = {
//     {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
// };

unsigned char pb_bat_level = 0;
unsigned char pb_charge_state = 0;
unsigned char pb_rec_buf[5] = {0};
unsigned char pb_rec = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void cs_task_manager_cb(unsigned char *cmd);
void cs_task_HighWaterMark_cb(unsigned char *cmd);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  // default_kv.kvs = default_kv_table;
  // default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
  // fdb_kvdb_init(&kvdb, "NVS", "fdb", &default_kv, NULL);
  elog_my_init();
  cs_init();
  HAL_UART_Receive_IT(&huart5, &pb_rec, 1);
  // cs_recv_start();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 300);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  xTaskCreate((TaskFunction_t)console_task, "console_task", 600, NULL, 0, &console_task_handle);
  xTaskCreate((TaskFunction_t)hcp_task, "hcp task", 600, NULL, 5, &hcp_task_handle);
  // xTaskCreate((TaskFunction_t)test_task, "test_task", 400, NULL, 0, &test_task_handle);
  xTaskCreate((TaskFunction_t)motor_task, "motor_task", 400, NULL, 0, &motor_task_handle);
  // xTaskCreate((TaskFunction_t)rgb_task, "rgb_task", 100, NULL, 2, &rgb_task_handle);
  xTaskCreate((TaskFunction_t)liquid_level_task, "liquid level task", 100, NULL, 0, &liquid_level_task_handle);
  xTaskCreate((TaskFunction_t)brush_deamon_task, "brush deamon task", 400, NULL, 4, &brush_deamon_task_handle);
  xTaskCreate((TaskFunction_t)flowmeter_task, "flowmeter task", 200, NULL, 1, &flowmeter_task_handle);
  // xTaskCreate((TaskFunction_t)battery_task, "battery task", 200, NULL, 1, &battery_task_handle);
  xTaskCreate((TaskFunction_t)accurate_ratio_task, "accurate ratio task", 200, NULL, 1, &accurate_ratio_task_handle);
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */

  // struct fdb_blob blob;
  // int boot_count = 0;

  // printf("\r==================== kvdb_basic_sample ====================\r\n");
  // { /* GET the KV value */
  //   /* get the "boot_count" KV value */
  //   fdb_kv_get_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
  //   /* the blob.saved.len is more than 0 when get the value successful */
  //   if (blob.saved.len > 0)
  //   {
  //     printf("get the 'boot_count' value is %d\r\n", boot_count);
  //   }
  //   else
  //   {
  //     printf("get the 'boot_count' failed\r\n");
  //   }
  // }
  // { /* CHANGE the KV value */
  //   /* increase the boot count */
  //   boot_count++;
  //   /* change the "boot_count" KV's value */
  //   fdb_kv_set_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
  //   // printf("set the 'boot_count' value to %d\n", boot_count);
  // }
  // printf("===========================================================\r\n");

  cs_reg_fun("info", cs_task_HighWaterMark_cb);
  cs_reg_fun("sys", cs_task_manager_cb);
  // MOTOR_BK_ON[0]();
  // MOTOR_BK_ON[1]();
  // MOTOR_BK_ON[2]();
  // MOTOR_BK_ON[3]();
  // MOTOR_BK_ON[4]();
  // MOTOR_BK_ON[5]();
  // MOTOR_BK_ON[6]();
  // MOTOR_BK_ON[7]();

  // vTaskDelete(NULL);
  // char bat = 0;

  for (;;)
  {
    // bat_get_power(&bat);
    // printf("%d", bat);
    osDelay(200);
    // printf("%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d\r\n",
    //    HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_F_GPIO_Port, MOTO_FB1_LIMIT_F_Pin),
    //    HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_B_GPIO_Port, MOTO_FB1_LIMIT_B_Pin),
    //    HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_F_GPIO_Port, MOTO_FB2_LIMIT_F_Pin),
    //    HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_B_GPIO_Port, MOTO_FB2_LIMIT_B_Pin),
    //    HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_F_GPIO_Port, MOTO_FB3_LIMIT_F_Pin),
    //    HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_B_GPIO_Port, MOTO_FB3_LIMIT_B_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_U_GPIO_Port, MOTO_UD1_LIMIT_U_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_D_GPIO_Port, MOTO_UD1_LIMIT_D_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_U_GPIO_Port, MOTO_UD2_LIMIT_U_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_D_GPIO_Port, MOTO_UD2_LIMIT_D_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_D_GPIO_Port, MOTO_UD3_LIMIT_D_Pin),
    //    HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_U_GPIO_Port, MOTO_UD3_LIMIT_U_Pin),
    //    HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_F_GPIO_Port, MOTO_BARM_LIMIT_F_Pin),
    //    HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_B_GPIO_Port, MOTO_BARM_LIMIT_B_Pin),
    //    HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_U_GPIO_Port, MOTO_SARM_LIMIT_U_Pin),
    //    HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_D_GPIO_Port, MOTO_SARM_LIMIT_D_Pin));

    // HAL_GPIO_TogglePin(OU_RES_4_GPIO_Port, OU_RES_4_Pin);
    // HAL_GPIO_TogglePin(OU_RES_5_GPIO_Port, OU_RES_5_Pin);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void test_task()
{

  // char send_buf[25];
  // unsigned int liquid_level_1 = 0;
  // unsigned int liquid_level_2 = 0;
  // unsigned int liquid_level_3 = 0;
  // unsigned int liquid_level_4 = 0;
  // rgb_test_fun();
  // HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
  // __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 20);

  while (1)
  {
    // get_liquid_level(&liquid_level_1, &liquid_level_2, &liquid_level_3, &liquid_level_4);
    // sprintf(send_buf, "%d,%d,%d,%d\r\n", liquid_level_1, liquid_level_2, liquid_level_3, liquid_level_4);
    // printf("%s", send_buf);

    // HAL_GPIO_TogglePin(SIGN_LIGHT_1_GPIO_Port, SIGN_LIGHT_1_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(SIGN_LIGHT_2_GPIO_Port, SIGN_LIGHT_2_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(SIGN_LIGHT_3_GPIO_Port, SIGN_LIGHT_3_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(SIGN_LIGHT_4_GPIO_Port, SIGN_LIGHT_4_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_1_GPIO_Port, MOTO_BK_1_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_2_GPIO_Port, MOTO_BK_2_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_3_GPIO_Port, MOTO_BK_3_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_4_GPIO_Port, MOTO_BK_4_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_5_GPIO_Port, MOTO_BK_5_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_6_GPIO_Port, MOTO_BK_6_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_7_GPIO_Port, MOTO_BK_7_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(MOTO_BK_8_GPIO_Port, MOTO_BK_8_Pin);

    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(PUMP_1_GPIO_Port, PUMP_1_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(PUMP_2_GPIO_Port, PUMP_2_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(PUMP_3_GPIO_Port, PUMP_3_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(PUMP_4_GPIO_Port, PUMP_4_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);

    // HAL_GPIO_TogglePin(DRUM_1_GPIO_Port, DRUM_1_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(DRUM_2_GPIO_Port, DRUM_2_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(DRUM_3_GPIO_Port, DRUM_3_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);

    // HAL_GPIO_TogglePin(SPOT_LIGHT_1_GPIO_Port, SPOT_LIGHT_1_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);
    // HAL_GPIO_TogglePin(SPOT_LIGHT_2_GPIO_Port, SPOT_LIGHT_2_Pin);
    // vTaskDelay(50 / portTICK_RATE_MS);

    // HAL_UART_Transmit(&huart2, "hello\r\n", 7, 1000);
    vTaskDelay(500 / portTICK_RATE_MS);
    printf("%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d\r\n",
           HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_F_GPIO_Port, MOTO_FB1_LIMIT_F_Pin),
           HAL_GPIO_ReadPin(MOTO_FB1_LIMIT_B_GPIO_Port, MOTO_FB1_LIMIT_B_Pin),
           HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_F_GPIO_Port, MOTO_FB2_LIMIT_F_Pin),
           HAL_GPIO_ReadPin(MOTO_FB2_LIMIT_B_GPIO_Port, MOTO_FB2_LIMIT_B_Pin),
           HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_F_GPIO_Port, MOTO_FB3_LIMIT_F_Pin),
           HAL_GPIO_ReadPin(MOTO_FB3_LIMIT_B_GPIO_Port, MOTO_FB3_LIMIT_B_Pin),
           HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_U_GPIO_Port, MOTO_UD1_LIMIT_U_Pin),
           HAL_GPIO_ReadPin(MOTO_UD1_LIMIT_D_GPIO_Port, MOTO_UD1_LIMIT_D_Pin),
           HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_U_GPIO_Port, MOTO_UD2_LIMIT_U_Pin),
           HAL_GPIO_ReadPin(MOTO_UD2_LIMIT_D_GPIO_Port, MOTO_UD2_LIMIT_D_Pin),
           HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_U_GPIO_Port, MOTO_UD3_LIMIT_U_Pin),
           HAL_GPIO_ReadPin(MOTO_UD3_LIMIT_D_GPIO_Port, MOTO_UD3_LIMIT_D_Pin),
           HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_F_GPIO_Port, MOTO_BARM_LIMIT_F_Pin),
           HAL_GPIO_ReadPin(MOTO_BARM_LIMIT_B_GPIO_Port, MOTO_BARM_LIMIT_B_Pin),
           HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_U_GPIO_Port, MOTO_SARM_LIMIT_U_Pin),
           HAL_GPIO_ReadPin(MOTO_SARM_LIMIT_D_GPIO_Port, MOTO_SARM_LIMIT_D_Pin));
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  // Log serial port
  if (huart == &huart1)
  {
    cs_recv_callback(Size);
  }
  else if (huart == &huart4)
  {
    fm_recv_callback(Size);
  }
  // 232 serial port of upper computer
  // else if (huart == &huart2)
  // {
  //   hcp_recv_callback(Size);
  // }
  // power
  // else if (huart == &huart3)
  // {
  //   bat_recv_callback(Size);
  // }
}

extern unsigned char hcp_buf[40];
extern unsigned char hcp_rec_s;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static unsigned char pb_rec_buf[5] = {0};
  static unsigned char pb_rec_index = 0;
  static unsigned int hcp_rec_index = 0;
  if (huart == &huart5)
  {
    if (pb_rec == 0x5a)
    {
      pb_rec_index = 0;
    }
    pb_rec_buf[pb_rec_index++] = pb_rec;
    if (pb_rec_index > 3)
    {
      if (pb_rec == 0xa5)
      {
        pb_bat_level = pb_rec_buf[1];
        pb_charge_state = pb_rec_buf[2];
        // printf("%d-%d\r\n", pb_bat_level,pb_charge_state);
      }
      pb_rec_index = 0;
    }
    HAL_UART_Receive_IT(&huart5, &pb_rec, 1);
  }
  else if (huart == &huart2)
  {
    if (hcp_rec_s == 0x5a)
    {
      hcp_rec_index = 0;
    }
    hcp_buf[hcp_rec_index++] = hcp_rec_s;
    if (hcp_rec_index > 20)
    {
      hcp_rec_index = 0;
    }
    if (hcp_rec_index == 16)
    {
      hcp_recv_callback(hcp_rec_index);
      hcp_rec_index = 0;
    }

    HAL_UART_Receive_IT(&huart2, &hcp_rec_s, 1);
  }
}
void cs_task_manager_cb(unsigned char *cmd)
{
  unsigned char write_buffer[500];
  vTaskList((char *)&write_buffer);
  printf("taskname      taskstate priority   free num\r\n");
  printf("%s\r\n\r\n\r\n\r\n", write_buffer);
}

void cs_task_HighWaterMark_cb(unsigned char *cmd)
{
  // elog_d("TASK-STATE", "default task      - > %d", (int)uxTaskGetStackHighWaterMark(defaultTaskHandle));
  // elog_d("TASK-STATE", "test task         - > %d", (int)uxTaskGetStackHighWaterMark(test_task_handle));
  elog_d("TASK-STATE", "console task           - > %d", (int)uxTaskGetStackHighWaterMark(console_task_handle));
  elog_d("TASK-STATE", "motor task             - > %d", (int)uxTaskGetStackHighWaterMark(motor_task_handle));
  elog_d("TASK-STATE", "rgb task               - > %d", (int)uxTaskGetStackHighWaterMark(rgb_task_handle));
  elog_d("TASK-STATE", "liquid leval task      - > %d", (int)uxTaskGetStackHighWaterMark(liquid_level_task_handle));
  elog_d("TASK-STATE", "hcp task               - > %d", (int)uxTaskGetStackHighWaterMark(hcp_task_handle));
  elog_d("TASK-STATE", "brush deamon_task      - > %d", (int)uxTaskGetStackHighWaterMark(brush_deamon_task_handle));
  elog_d("TASK-STATE", "flowmeter task         - > %d", (int)uxTaskGetStackHighWaterMark(flowmeter_task_handle));
  // elog_d("TASK-STATE", "battery task           - > %d", (int)uxTaskGetStackHighWaterMark(battery_task_handle));
  elog_d("TASK-STATE", "accurate ratio task    - > %d", (int)uxTaskGetStackHighWaterMark(accurate_ratio_task_handle));
}

/* USER CODE END Application */
