/*
 * clock.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_HAL_CLOCK_H_
#define INC_HAL_CLOCK_H_

#include "stdio.h"
#include "stdbool.h"

bool CLOCK_init();
void CLOCK_delayUs(uint32_t us);

#endif /* INC_HAL_CLOCK_H_ */
