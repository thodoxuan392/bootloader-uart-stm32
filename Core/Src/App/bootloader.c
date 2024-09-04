/*
 * bootloader.c
 *
 *  Created on: Sep 4, 2024
 *      Author: xuanthodo
 */


#include <App/bootloader.h>

#include <App/crc.h>

#include <Hal/timer.h>
#include <Hal/flash.h>

#include <utils/utils_logger.h>


static void BOOTLOADER_runWaitForCommand(void);
static void BOOTLOADER_runReprogramming(void);
static void BOOTLOADER_runCheckAppCrc(void);
static void BOOTLOADER_runJumpToApp(void);
static void BOOTLOADER_runBlockingErr(void);

static void BOOTLOADER_interrupt1ms(void);

static BOOTLOADER_State BOOTLOADER_state = BOOTLOADER_STATE_WAIT_FOR_COMMAND;
static BOOTLOADER_ReProgrammingCallback BOOTLOADER_reProgrammingCallback;
static uint32_t BOOTLOADER_timeoutCnt = 0;
static bool BOOTLOADER_timeoutFlag = false;

static bool BOOTLOADER_enterReProgrammingRequest = false;
static bool BOOTLOADER_exitReProgrammingRequest = false;
static bool BOOTLOADER_reProgrammingRequest = false;
static uint8_t BOOTLOADER_reProgrammingBuffer[BOOTLOADER_REPROGRAMMING_BUFFER_MAX_SIZE];
static uint32_t BOOTLOADER_reProgrammingBufferLength = 0;
static uint32_t BOOTLOADER_reProgrammingWrittenLength = 0;

static uint32_t BOOTLOADER_applicationSize;
static uint32_t BOOTLOADER_applicationCrc;
static uint8_t BOOTLOADER_applicationVersionMajor;
static uint8_t BOOTLOADER_applicationVersionMinor;
static uint8_t BOOTLOADER_applicationVersionPatch;

static uint8_t BOOTLOADER_crcCheckBuffer[BOOTLOADER_CRC_CHECK_SIZE];
static uint32_t BOOTLOADER_crcCheckedSize = 0;

void BOOTLOADER_init(void){
	TIMER_attach_intr_1ms(BOOTLOADER_interrupt1ms);
	BOOTLOADER_timeoutCnt = BOOTLOADER_NO_ACTIVITY_TIMEOUT;
	BOOTLOADER_timeoutFlag = false;
}

void BOOTLOADER_deinit(void){

}

void BOOTLOADER_setReProgrammingCallback(BOOTLOADER_ReProgrammingCallback callback){
	BOOTLOADER_reProgrammingCallback = callback;
}

void BOOTLOADER_run(void){
	switch (BOOTLOADER_state) {
		case BOOTLOADER_STATE_WAIT_FOR_COMMAND:
			BOOTLOADER_runWaitForCommand();
			break;
		case BOOTLOADER_STATE_REPROGRAMMING:
			BOOTLOADER_runReprogramming();
			break;
		case BOOTLOADER_STATE_CHECK_APP_CRC:
			BOOTLOADER_runCheckAppCrc();
			break;
		case BOOTLOADER_STATE_JUMP_TO_APP:
			BOOTLOADER_runJumpToApp();
			break;
		case BOOTLOADER_STATE_BLOCKING_ERR:
			BOOTLOADER_runBlockingErr();
			break;
		default:
			break;
	}
}
BOOTLOADER_State BOOTLOADER_getState(void){
	return BOOTLOADER_state;
}
bool BOOTLOADER_enterReProgramming(uint8_t appVerMajor, uint8_t appVerMinor, uint8_t appVerPatch, uint32_t appSize, uint16_t appCrc){
	// Write to Firmware Information
	BOOTLOADER_enterReProgrammingRequest = true;
	BOOTLOADER_applicationVersionMajor = appVerMajor;
	BOOTLOADER_applicationVersionMinor = appVerMinor;
	BOOTLOADER_applicationVersionPatch = appVerPatch;
	BOOTLOADER_applicationSize = appSize;
	BOOTLOADER_applicationCrc = appCrc;
	return true;
}

bool BOOTLOADER_exitReProgramming(void){
	BOOTLOADER_exitReProgrammingRequest = true;
}

bool BOOTLOADER_rePrograming(uint32_t address, uint8_t *data, uint32_t dataSize){
	if(dataSize > BOOTLOADER_REPROGRAMMING_BUFFER_MAX_SIZE){
		return false;
	}
	BOOTLOADER_reProgrammingRequest = true;
	memcpy(BOOTLOADER_reProgrammingBuffer, data, dataSize);
	BOOTLOADER_reProgrammingBufferLength = dataSize;
	return true;
}

bool BOOTLOADER_isAddressValid(uint32_t address){
	return ((address % 1024) == 0);
}

bool BOOTLOADER_isAppAddress(uint32_t address){
	return (APPLICATION_FLASH_ADDR == address);
}

static void BOOTLOADER_runWaitForCommand(void){
	if(BOOTLOADER_timeoutFlag){
		BOOTLOADER_state = BOOTLOADER_STATE_CHECK_APP_CRC;
		return;
	}
	if(BOOTLOADER_enterReProgrammingRequest){
		BOOTLOADER_enterReProgrammingRequest = false;
		BOOTLOADER_state = BOOTLOADER_STATE_REPROGRAMMING;
	}
}
static void BOOTLOADER_runReprogramming(void){
	if(BOOTLOADER_exitReProgrammingRequest){
		BOOTLOADER_state = BOOTLOADER_STATE_CHECK_APP_CRC;
		return;
	}
	if(BOOTLOADER_reProgrammingRequest){
		BOOTLOADER_reProgrammingRequest = false;
		// Reprogramming
		if(!FLASH_erase(APPLICATION_FLASH_ADDR + BOOTLOADER_reProgrammingWrittenLength, BOOTLOADER_reProgrammingBufferLength)){
			BOOTLOADER_reProgrammingCallback(BOOTLOADER_REPROGRAMMING_RESULT_FAILED);
			utils_log_error("[BOOTLOADER] Flash erase failed at address %x, len %d\r\n",
							APPLICATION_FLASH_ADDR + BOOTLOADER_reProgrammingWrittenLength,
							BOOTLOADER_reProgrammingBufferLength);
			return;
		}
		if(!FLASH_write_buf(APPLICATION_FLASH_ADDR + BOOTLOADER_reProgrammingWrittenLength, BOOTLOADER_reProgrammingBuffer, BOOTLOADER_reProgrammingBufferLength)){
			BOOTLOADER_reProgrammingCallback(BOOTLOADER_REPROGRAMMING_RESULT_FAILED);
			utils_log_error("[BOOTLOADER] Flash write failed at address %x, len %d\r\n",
					APPLICATION_FLASH_ADDR + BOOTLOADER_reProgrammingWrittenLength,
					BOOTLOADER_reProgrammingBufferLength);
			return;
		}
		BOOTLOADER_reProgrammingWrittenLength += BOOTLOADER_reProgrammingBufferLength;
		// Check if Reprogramming is done
		if(BOOTLOADER_reProgrammingWrittenLength >= BOOTLOADER_applicationSize){
			BOOTLOADER_state = BOOTLOADER_STATE_CHECK_APP_CRC;
		}
		BOOTLOADER_reProgrammingCallback(BOOTLOADER_REPROGRAMMING_RESULT_SUCCESS);
	}
}
static void BOOTLOADER_runCheckAppCrc(void){
	// Check Partial CRC in Cyclic
	static uint16_t crc16 = 0;
	uint32_t crcCheckSize;
	if(BOOTLOADER_applicationSize - BOOTLOADER_crcCheckedSize > BOOTLOADER_CRC_CHECK_SIZE){
		crcCheckSize = BOOTLOADER_CRC_CHECK_SIZE;
	}else {
		crcCheckSize = BOOTLOADER_applicationSize - BOOTLOADER_crcCheckedSize;
	}
	if(!FLASH_read_buf(APPLICATION_FLASH_ADDR + BOOTLOADER_crcCheckedSize, BOOTLOADER_crcCheckBuffer, crcCheckSize)){
		utils_log_error("[BOOTLOADER]: Flash read buf failed at addr %x, size %d\r\n",
								APPLICATION_FLASH_ADDR + BOOTLOADER_crcCheckedSize,
								crcCheckSize);
		BOOTLOADER_state = BOOTLOADER_STATE_BLOCKING_ERR;
		return;
	}
	crc16 = CRC_calculate(crc16, BOOTLOADER_crcCheckBuffer, crcCheckSize);
	BOOTLOADER_crcCheckedSize += crcCheckSize;
	// Read CRC done
	if(BOOTLOADER_crcCheckedSize == BOOTLOADER_applicationSize){
		if(crc16 == BOOTLOADER_applicationCrc){
			BOOTLOADER_state = BOOTLOADER_STATE_JUMP_TO_APP;
		}else {
			BOOTLOADER_state = BOOTLOADER_STATE_BLOCKING_ERR;
		}
	}
}

static void BOOTLOADER_runJumpToApp(void){
	void (*appResetHandler)(void) = (void*)(*(volatile uint32_t*) (APPLICATION_FLASH_ADDR + 4));
	appResetHandler();
	while(1){
		// Don't reach here
	}
}

static void BOOTLOADER_runBlockingErr(void){
	// Nothing to do -> Block here, Need to reset board to exit
}

static void BOOTLOADER_interrupt1ms(void){
	if(BOOTLOADER_timeoutCnt){
		BOOTLOADER_timeoutCnt--;
		if(BOOTLOADER_timeoutCnt == 0){
			BOOTLOADER_timeoutFlag = true;
		}
	}
}
