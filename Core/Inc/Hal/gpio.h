/*
 * gpio.h
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */

#ifndef INC_HAL_GPIO_H_
#define INC_HAL_GPIO_H_

#include "main.h"
#include "stdio.h"
#include "stdbool.h"

typedef void (*GPIO_extCallback)(void);

typedef struct {
	GPIO_TypeDef * port;
	GPIO_InitTypeDef init_info;
	GPIO_extCallback callback;
}GPIO_info_t;

bool GPIO_init();
void GPIO_test();



#endif /* INC_HAL_GPIO_H_ */
