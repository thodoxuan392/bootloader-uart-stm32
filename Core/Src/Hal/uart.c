/*
 * uart.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Hal/uart.h"
#include "utils/utils_buffer.h"
#include "utils/utils_logger.h"

#define TX_TIMEOUT		0xFFFF

typedef struct {
	UART_HandleTypeDef * huart_p;
	void (*callback)(uint8_t *dataPtr, uint32_t dataSize);
	utils_buffer_t * buffer;
	uint16_t temp_data;
}UART_info_t;

UART_HandleTypeDef huart1 = {
	.Instance = USART1,
	.Init.BaudRate = 9600,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};


UART_HandleTypeDef huart4 = {
	.Instance = UART4,
	.Init.BaudRate = 9600,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16
};

static utils_buffer_t uart_buffer[UART_MAX];

static UART_info_t uart_table[UART_MAX] = {
		[UART_1] = {
			.huart_p = &huart1,
			.buffer = &uart_buffer[UART_1]
		},
		[UART_4] = {
			.huart_p = &huart4,
			.buffer = &uart_buffer[UART_4]
		},
};


bool UART_init(){
	bool success = true;
	// Init hal
	success = (HAL_UART_Init(uart_table[UART_1].huart_p) == HAL_OK) && success;
	success = (HAL_UART_Init(uart_table[UART_4].huart_p) == HAL_OK) && success;
	// Init buffer
	success = utils_buffer_init(uart_table[UART_1].buffer, sizeof(uint8_t)) && success;
	success = utils_buffer_init(uart_table[UART_4].buffer, sizeof(uint8_t)) && success;

	HAL_UART_Receive_IT(uart_table[UART_1].huart_p, &uart_table[UART_1].temp_data, 1);
	HAL_UART_Receive_IT(uart_table[UART_4].huart_p, &uart_table[UART_4].temp_data, 1);
	return success;
}
bool UART_send(UART_id_t id, uint8_t *data , size_t len){
	HAL_UART_Transmit(uart_table[id].huart_p, data, len, 0xFFFFFFFF);
}
bool UART_receive_available(UART_id_t id){
	return utils_buffer_is_available(uart_table[id].buffer);
}

uint16_t UART_receive_data(UART_id_t id){
	uint16_t data;
	utils_buffer_pop(uart_table[id].buffer, &data);
	return data;
}

void UART_set_receive_callback(UART_id_t id, void (*callback)(uint8_t *dataPtr, uint32_t dataSize)){
	uart_table[id].callback = callback;
}

void UART_clear_buffer(UART_id_t id){
	utils_buffer_drop_all(uart_table[id].buffer);
}

void UART_test(){
	while(1){
		HAL_Delay(1000);
		UART_send(UART_1, "AT\r\n", 4);
		utils_log_info("Sending AT command\r\n");
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	if(huart->Instance == uart_table[UART_1].huart_p->Instance){
		utils_buffer_push(uart_table[UART_1].buffer, &uart_table[UART_1].temp_data);
		HAL_UART_Receive_IT(uart_table[UART_1].huart_p, &uart_table[UART_1].temp_data, 1);
		if(uart_table[UART_1].callback) uart_table[UART_1].callback(&uart_table[UART_1].temp_data, 1);
	}else if(huart->Instance == uart_table[UART_4].huart_p->Instance){
		utils_buffer_push(uart_table[UART_4].buffer, &uart_table[UART_4].temp_data);
		HAL_UART_Receive_IT(uart_table[UART_4].huart_p, &uart_table[UART_4].temp_data, 1);
		if(uart_table[UART_4].callback) uart_table[UART_4].callback(&uart_table[UART_4].temp_data, 1);
	}
}
