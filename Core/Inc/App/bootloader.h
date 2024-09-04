/*
 * bootloader.h
 *
 *  Created on: Sep 4, 2024
 *      Author: xuanthodo
 */

#ifndef INC_APP_BOOTLOADER_H_
#define INC_APP_BOOTLOADER_H_

#include <stdbool.h>
#include <stdint.h>

#define BOOTLOADER_FLASH_ADDR		0x08000000
#define APPLICATION_INFO_FLASH_ADDR	0x08004F00
#define APPLICATION_FLASH_ADDR		0x08005000

#define BOOTLOADER_NO_ACTIVITY_TIMEOUT	3000	// 3s after no command from host controller
#define BOOTLOADER_REPROGRAMMING_BUFFER_MAX_SIZE	1024
#define BOOTLOADER_CRC_CHECK_SIZE 1024

typedef enum {
	BOOTLOADER_REPROGRAMMING_RESULT_SUCCESS = 0x00,
	BOOTLOADER_REPROGRAMMING_RESULT_FAILED,
}BOOTLOADER_ReProgrammingResult;

typedef enum {
	BOOTLOADER_STATE_WAIT_FOR_COMMAND = 0x00,
	BOOTLOADER_STATE_REPROGRAMMING = 0x01,
	BOOTLOADER_STATE_CHECK_APP_CRC = 0x02,
	BOOTLOADER_STATE_JUMP_TO_APP = 0x03,
	BOOTLOADER_STATE_BLOCKING_ERR = 0x04
}BOOTLOADER_State;

typedef void (*BOOTLOADER_ReProgrammingCallback)(BOOTLOADER_ReProgrammingResult result);

void BOOTLOADER_init(void);
void BOOTLOADER_deinit(void);
void BOOTLOADER_setReProgrammingCallback(BOOTLOADER_ReProgrammingCallback callback);

void BOOTLOADER_run(void);
BOOTLOADER_State BOOTLOADER_getState(void);
bool BOOTLOADER_enterReProgramming(uint8_t appVerMajor, uint8_t appVerMinor, uint8_t appVerPatch, uint32_t appSize, uint16_t appCrc);
bool BOOTLOADER_exitReProgramming(void);
bool BOOTLOADER_rePrograming(uint32_t address, uint8_t *data, uint32_t dataSize);

bool BOOTLOADER_isAddressValid(uint32_t address);
bool BOOTLOADER_isAppAddress(uint32_t address);




#endif /* INC_APP_BOOTLOADER_H_ */
