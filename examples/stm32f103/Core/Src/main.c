/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_open.h"
#include "fifo.h"
#include "obj.h"
#include "params.h"
#include "pdo.h"
#include "port.h"
#include "sdo.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

device_data_t device_data = {.error_register = 0,
                             .node_id = 1,
                             .heartbeat_time = 1000,
                             .device_name = "MY_CANOPEN_DEVICE",
                             .serial_number = 0x12345678,
                             .product_code = 0xABCD,
                             .baudrate = 250,
                             .operating_mode = 3,
                             .target_position = 0,
                             .actual_position = 0,
                             .target_velocity = 0,
                             .actual_velocity = 0,
                             .control_word = 0,
                             .status_word = 0,
                             .temperature = 25,
                             .error_history = {0}};

od_type object_dictionary[OBJECT_DICTIONARY_SIZE] = {
    /* --------------------------------------------------------------------------------------------------------------------------
     */
    /* | Parameter name   | Index | Sub index |   Data type     | Flag  | Data
       pointer | Min | Max        |*/
    /* --------------------------------------------------------------------------------------------------------------------------
     */
    {"Error register  ",
     0x1000,
     0x00,
     OD_TYPE_UINT32,
     OD_RO,
     &device_data.error_register,
     {0},
     {.u32 = 0xFFFFFFFF}},
    {"Error history[0]",
     0x1001,
     0x00,
     OD_TYPE_INT8,
     OD_RO,
     &device_data.error_history[0],
     {0},
     {.i8 = 127}},
    {"Device Name     ",
     0x1008,
     0x00,
     OD_TYPE_STRING,
     OD_RO,
     &device_data.device_name,
     {0},
     {0}},
    {"Heartbeat_time  ",
     0x1010,
     0x00,
     OD_TYPE_UINT32,
     OD_RO,
     &device_data.heartbeat_time,
     {0},
     {.u32 = 0xFFFFFFFF}}};

co_obj_t canopen_client;
co_obj_t canopen_server;

#define NODE_ID_PLATE1 1
#define NODE_ID_PLATE2 2
#define NODE_ID_PLATE3 3

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    // TODO нужно спрятать под капот определение инстанса
    co_msg_t msg;
    canopen_get_msg_from_handler(&msg, CAN_RX_FIFO0);
    switch (canopen_get_node_id(&msg)) {
    case NODE_ID_PLATE1:
        canopen_send_msg_to_fifo_rx(&canopen_server, &msg);
        break;
    case NODE_ID_PLATE2:
        canopen_send_msg_to_fifo_rx(&canopen_client, &msg);
        break;
    default:
        break;
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t node_id;
    co_msg_t msg = {0};
    canopen_get_msg_from_handler(&msg, CAN_RX_FIFO1);
    node_id = canopen_get_node_id(&msg);
    switch (node_id) {
    case NODE_ID_PLATE1:
        canopen_send_msg_to_fifo_rx(&canopen_server, &msg);

        break;
    case NODE_ID_PLATE2:
        canopen_send_msg_to_fifo_rx(&canopen_client, &msg);
        break;
    default:
        break;
    }
}

void canopen_sdo_callback(co_obj_t *canopen, co_msg_t *msg) {}

co_msg_t sdo_client;
co_msg_t sdo_server;
co_msg_t pdo0;
co_msg_t pdo1 = {0};

uint32_t pdo0_id = 0x00000100;
uint32_t pdo1_id = 0x00000200;

void pdo1_callback(co_msg_t *msg)
{
    // uint16_t lol = 0;
}

void sdo_callback(co_msg_t *msg) {}

void pdo22_send()
{
    // pdo1.frame.pdo.data[0] = 10;
    // pdo2.frame.pdo.data[1] = 11;
    // co_pdo_send(&canopen, &pdo1);
    // co_pdo_send(&canopen, &pdo2);
}

// CAN_TxHeaderTypeDef txHeader;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CAN_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */
    /*CANopen CLIENT
     * ========================================================================*/
    co_init(&canopen_client, CANOPEN_CLIENT, NODE_ID_PLATE2, COB_ID_STD);
    canopen_config_node_id(&canopen_client, NODE_ID_PLATE1);

    /* Конфигурация PDO сообщений */
    co_pdo1_cfg_rx(&canopen_client, NODE_ID_PLATE1, &pdo1_callback);
    co_pdo2_cfg_rx(&canopen_client, NODE_ID_PLATE1, &pdo1_callback);

    /* Конфигурация SDO сообщений */
    co_sdo_cfg(&canopen_client, &sdo_client, NODE_ID_PLATE1, &sdo_callback);
    /*=======================================================================================*/

    /*CANopen SERVER
     * ========================================================================*/
    co_init(&canopen_server, CANOPEN_SERVER, NODE_ID_PLATE1, COB_ID_STD);
    canopen_config_node_id(&canopen_server, NODE_ID_PLATE1);
    canopen_config_node_id(&canopen_server, NODE_ID_PLATE2);
    canopen_config_node_id(&canopen_server, NODE_ID_PLATE3);

    /* Конфигурация PDO сообщений */
    co_pdo1_cfg_tx(&canopen_server, &pdo0, NODE_ID_PLATE2, 8);
    co_pdo2_cfg_tx(&canopen_server, &pdo1, NODE_ID_PLATE2, 8);

    /* Конфигурация SDO сообщений */
    co_sdo_cfg(&canopen_server, &sdo_server, NODE_ID_PLATE2, &sdo_callback);
    /*=======================================================================================*/

    HAL_TIM_Base_Start_IT(&htim1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // co_pdo_data_t pdo_data = {0};
        // pdo_data.word1 = 12345;

        canopen_sdo_read_8(&canopen_client, &sdo_client, 0x1010, 0);

        canopen_process_rx(&canopen_server);
        canopen_process_tx(&canopen_server);

        canopen_process_rx(&canopen_client);
        canopen_process_tx(&canopen_client);
        HAL_Delay(500);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
