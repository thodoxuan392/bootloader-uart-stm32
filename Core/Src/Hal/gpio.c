/*
 * gpio.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */

#ifndef SRC_HAL_GPIO_C_
#define SRC_HAL_GPIO_C_

#include "main.h"
#include "Hal/gpio.h"

bool GPIO_init(){
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE();
}


void GPIO_test(){
	GPIO_InitTypeDef init;
	init.Pin = GPIO_PIN_3;
	init.Mode = GPIO_MODE_OUTPUT_OD;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &init);
	while(1){
		HAL_Delay(1);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	}
}
#endif /* SRC_HAL_GPIO_C_ */
