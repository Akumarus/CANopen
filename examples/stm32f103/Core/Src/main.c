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
#include "co.h"
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

device_data_t device_data = {.error_register = 0xFFFF,
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

co_od_t object_dictionary[OBJECT_DICTIONARY_SIZE] = {
    /* --------------------------------------------------------------------------------------------------------------------------
     */
    /* | Parameter name   | Index | Sub index |   Data type     | Flag  | Data
       pointer | Min | Max        |*/
    /* --------------------------------------------------------------------------------------------------------------------------
     */
    {"Error register  ",
     &device_data.error_register,
     0x1000,
     0x00,
     OD_TYPE_UINT32,
     OD_RO,
     {0},
     {.u32 = 0xFFFFFFFF}},
    {"Error history[0]",
     &device_data.error_history[0],
     0x1001,
     0x00,
     OD_TYPE_INT8,
     OD_RO,
     {0},
     {.i8 = 127}},
    {"Device Name     ", &device_data.device_name, 0x1008, 0x00, OD_TYPE_STRING, OD_RO, {0}, {0}},
    {"Heartbeat_time  ",
     &device_data.heartbeat_time,
     0x1010,
     0x00,
     OD_TYPE_UINT32,
     OD_RO,
     {0},
     {.u32 = 0xFFFFFFFF}}};

co_obj_t canopen_client;
co_obj_t canopen_server;

#define NODE_ID_PLATE1 1
#define NODE_ID_PLATE2 2
#define NODE_ID_PLATE3 3

#define NODE_ID2 2

void CAN_SendTestMessage(void) {
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint32_t txMailbox;

    txHeader.StdId = 0x123; // Идентификатор сообщения
    txHeader.ExtId = 0;
    txHeader.IDE = CAN_ID_STD;   // Стандартный идентификатор
    txHeader.RTR = CAN_RTR_DATA; // Data frame
    txHeader.DLC = 8;            // Длина данных
    txHeader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    co_msg_t msg = {0};
    canopen_get_msg_from_handler(&msg, CAN_RX_FIFO0);
    canopen_send_msg_to_fifo_rx(&canopen_server, &msg);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    co_msg_t msg = {0};
    canopen_get_msg_from_handler(&msg, CAN_RX_FIFO1);
    canopen_send_msg_to_fifo_rx(&canopen_server, &msg);
}

void canopen_sdo_callback(co_obj_t *canopen, co_msg_t *msg) {}

void pdo_callback(co_msg_t *msg) { 
    uint8_t lol;
    lol++;
}
void sdo_callback(co_msg_t *msg) {}

// CAN_TxHeaderTypeDef txHeader;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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

    /** CANopen Server example------------------------------------ */
    co_init(&canopen_server, CANOPEN_SERVER, NODE_ID2, COB_ID_STD);
    co_config_node_id(&canopen_server, 0);

    co_subscribe_pdo(&canopen_server, RPDO1(2), pdo_callback);
    co_subscribe_sdo(&canopen_server, 2, sdo_callback);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        co_process_msg_rx(&canopen_server);
        co_process_msg_tx(&canopen_server);
        HAL_Delay(1);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
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
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
void Error_Handler(void) {
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
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
