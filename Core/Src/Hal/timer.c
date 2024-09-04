/*
 * timer.c
 *
 *  Created on: May 28, 2023
 *      Author: xuanthodo
 */

#ifndef SRC_HAL_TIMER_C_
#define SRC_HAL_TIMER_C_

#include "main.h"
#include "Hal/timer.h"
#include "utils/utils_logger.h"

#define TIMER_FN_MAX_SIZE	10

static TIMER_fn fn_table_origin[TIMER_FN_MAX_SIZE];
static size_t fn_table_len_origin = 0;

static TIMER_fn fn_table_1ms[TIMER_FN_MAX_SIZE];
static uint8_t fn_table_1ms_cnt[TIMER_FN_MAX_SIZE];
static size_t fn_table_len_1ms = 0;
static const uint32_t fn_1ms_counter = TIMER_FREQ / 1000;

TIM_HandleTypeDef htim3 = {
	.Instance = TIM3,
	.Init = {
		.Prescaler = 63,
		.CounterMode = TIM_COUNTERMODE_UP,
		.Period = (1000000 / TIMER_FREQ) - 1,
		.ClockDivision = TIM_CLOCKDIVISION_DIV1,
		.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
	}
};


bool TIMER_init(){
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	return true;
}

bool TIMER_attach_intr_origin(void (*fn)(void)){
	if(fn_table_len_origin >= TIMER_FN_MAX_SIZE){
		return false;
	}
	fn_table_origin[fn_table_len_origin++] = fn;
	return true;
}

bool TIMER_attach_intr_1ms(void (*fn)(void)){
	if(fn_table_len_1ms >= TIMER_FN_MAX_SIZE){
		return false;
	}
	fn_table_1ms[fn_table_len_1ms++] = fn;
	return true;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim){
	if(htim->Instance == htim3.Instance){
		for (int fn_idx = 0; fn_idx < fn_table_len_1ms; ++fn_idx) {
			fn_table_1ms_cnt[fn_idx]++;
			if(fn_table_1ms_cnt[fn_idx] == fn_1ms_counter){
				fn_table_1ms_cnt[fn_idx] = 0;
				fn_table_1ms[fn_idx]();
			}
		}
		for (int fn_idx = 0; fn_idx < fn_table_len_origin; ++fn_idx) {
			fn_table_origin[fn_idx]();
		}
	}
}

void TIMER_test(){
	static uint32_t counter = 1000;
	counter --;
	if(counter == 0){
		counter = 1000;
		utils_log_info("Test\r\n");
	}
}


#endif /* SRC_HAL_TIMER_C_ */
