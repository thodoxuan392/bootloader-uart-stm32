/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
#include "Hal/clock.h"
#include "Hal/gpio.h"
#include "Hal/timer.h"
#include "Hal/i2c.h"
#include "Hal/rtc.h"
#include "Hal/uart.h"
#include "Hal/watchdog.h"
#include "App/protocol.h"
#include "App/commandhandler.h"
#include "App/schedulerport.h"
#include "App/statusreporter.h"
#include "App/bootloader.h"
#include "Lib/scheduler/scheduler.h"


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

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  // Hal init
  CLOCK_init();
  GPIO_init();
  I2C_init();
  TIMER_init();
  UART_init();
  RTC_init();

  // App Init
  COMMANDHANDLER_init();
  PROTOCOL_init();
  SCHEDULERPORT_init();
  STATUSREPORTER_init();
  BOOTLOADER_init();
  /* USER CODE END Init */

  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  GPIO_test();
//  SPI_test();
//  UART_test();
//  RTC_test();
//  EEPROM_test();
//  KEY_test();
//  CONFIG_test();
//  JSMNG_test();
//  CONFIG_clear();
//  WATCHDOG_test();
//  TIMER_test();
//  RFID_test();
//	WATERFLOW_test();
//  SOUND_test();
//  STEPPER_test();
//  MIXER_test();
//  PLACEDPOINT_test();
//  PIPELINE_test();
//  DOOR_test();
  while (1)
  {
	COMMANDHANDLER_run();
	PROTOCOL_run();
	BOOTLOADER_run();
	STATUSREPORTER_run();
	SCH_Dispatch_Tasks();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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

#ifdef  USE_FULL_ASSERT
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
