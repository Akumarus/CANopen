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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_open.h"
#include "obj.h"
#include "pdo.h"
#include "sdo.h"
#include "fifo.h"
#include "port.h"
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
canopen_t canopen;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  canopen_isr_handler(&canopen, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  canopen_isr_handler(&canopen, CAN_RX_FIFO1);
}

canopen_msg_t pdo0;
canopen_msg_t pdo1 = {0};
canopen_msg_t pdo2;
canopen_msg_t pdo3;
canopen_msg_t pdo4;
canopen_msg_t pdo5;
canopen_msg_t pdo6;
canopen_msg_t pdo7;
canopen_msg_t pdo8;
canopen_msg_t pdo9;

uint32_t pdo0_id = 0x00000100;
uint32_t pdo1_id = 0x00000200;
uint32_t pdo2_id = 0x00000300;
uint32_t pdo3_id = 0x00000400;
uint32_t pdo4_id = 0x00000500;
uint32_t pdo5_id = 0x00000600;
uint32_t pdo6_id = 0x00000700;
uint32_t pdo7_id = 0x00000701;
uint32_t pdo8_id = 0x00000702;
uint32_t pdo9_id = 0x00000703;

void pdo1_callback(canopen_msg_t *msg)
{
  uint16_t lol = 0;
}

void pdo22_send()
{
  // pdo1.frame.pdo.data[0] = 10;
  // pdo2.frame.pdo.data[1] = 11;
  // canopen_send_pdo(&canopen, &pdo1);
  // canopen_send_pdo(&canopen, &pdo2);
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

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // device_data_t device_data = {
  //   .error_register = 0,
  //   .node_id = 1,
  //   .heartbeat_time = 1000,
  //   .device_name = "MY_CANOPEN_DEVICE",
  //   .serial_number = 0x12345678,
  //   .product_code = 0xABCD,
  //   .baudrate = 250,
  //   .operating_mode = 3,
  //   .target_position = 0,
  //   .actual_position = 0,
  //   .target_velocity = 0,
  //   .actual_velocity = 0,
  //   .control_word = 0,
  //   .status_word = 0,
  //   .temperature = 25,
  //   .error_history = {0}
  // };

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
  canopen_init(&canopen, COB_ID_STD);

  canopen_server_config_pdo1_tx(&canopen, &pdo1, 12, 8);
  canopen_server_config_pdo2_tx(&canopen, &pdo2, 12, 8);
  canopen_client_config_pdo1_rx(&canopen, 12, &pdo1_callback);
  canopen_client_config_pdo2_rx(&canopen, 12, &pdo1_callback);
  // canopen_config_callback(&canopen, pdo1_id, 1, &pdo1_callback);
  // canopen_config_callback(&canopen, pdo2_id, 0, &pdo1_callback);

  HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    canopen_pdo_data_t msg = {0};
    // msg.word0 = 12345;
    msg.word1 = 12345;
    // msg.word2 = 12345;
    // msg.word3 = 12345;
    canopen_send_pdo(&canopen, &pdo1, &msg);
    canopen_process_rx(&canopen);
    canopen_process_tx(&canopen);
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
