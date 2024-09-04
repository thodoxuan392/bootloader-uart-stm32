/*
 * commandhandler.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */
#include "App/commandhandler.h"

#include "main.h"
#include "string.h"
#include "config.h"
#include "cast.h"

#include "App/protocol.h"
#include "App/bootloader.h"

#include <Hal/rtc.h>

#include "utils/utils_logger.h"

typedef void (*COMMANDHANDLER_HandleFunc)(PROTOCOL_t *);

/**
 * @defgroup Command Handler Function Group
 *
 */
static void COMMANDHANDLER_handleReset(PROTOCOL_t *);
static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *);
static void COMMANDHANDLER_handleOtaStartReprogramming(PROTOCOL_t *);
static void COMMANDHANDLER_handleOtaStopReprogramming(PROTOCOL_t *);
static void COMMANDHANDLER_handleOtaTransferDataReprogramming(PROTOCOL_t *);


/**
 * @defgroup Command Response Function Group
 *
 */
static void COMMANDHANDLER_sendResetResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_ResultCode_t resultCode,
                                          uint8_t *bootloaderVersion);
static void COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendOtaStopReprogrammingResponse(PROTOCOL_ResultCode_t resultCode);
static void COMMANDHANDLER_sendOtaTransferDataResponse(PROTOCOL_ResultCode_t resultCode);

/**
 * @defgroup Callback Function
 */
static void COMMANDHANDLER_reProgrammingResultCallback(BOOTLOADER_ReProgrammingResult result);

/**
 * @defgroup Miscellaneous Function Group
 */
static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *);

static COMMANDHANDLER_HandleFunc COMMANDHANDLER_handleFuncTable[] = {
    [PROTOCOL_ID_CMD_RESET] = COMMANDHANDLER_handleReset,
    [PROTOCOL_ID_CMD_REQUEST_VERSION] = COMMANDHANDLER_handleRequestVersion,
    [PROTOCOL_ID_CMD_OTA_START_REPROGRAMMING] = COMMANDHANDLER_handleOtaStartReprogramming,
    [PROTOCOL_ID_CMD_OTA_STOP_REPROGRAMMING] = COMMANDHANDLER_handleOtaStopReprogramming,
    [PROTOCOL_ID_CMD_OTA_TRANSFER_DATA] = COMMANDHANDLER_handleOtaTransferDataReprogramming,
};
static PROTOCOL_t protocolMessage;

void COMMANDHANDLER_init() {
	BOOTLOADER_setReProgrammingCallback(COMMANDHANDLER_reProgrammingResultCallback);
}

void COMMANDHANDLER_run() {
	if (PROTOCOL_receive(&protocolMessage)) {
		if (!COMMANDHANDLER_isCommandValid(&protocolMessage)) {
			utils_log_error("[COMMANDHANDLER] Command is not valid\r\n");
			return;
		}
		// Execute Command Handler
		COMMANDHANDLER_handleFuncTable[protocolMessage.protocol_id](
			&protocolMessage);
	}
	return;
}

static void COMMANDHANDLER_handleReset(PROTOCOL_t *proto) {
	if (proto->data_len != 0) {
	utils_log_error("HandleReset failed: Invalid data_len %d, expected 0\r\n",
					proto->data_len);
	COMMANDHANDLER_sendResetResponse(
		PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
	return;
	}
	COMMANDHANDLER_sendResetResponse(PROTOCOL_RESULT_SUCCESS);

	// Reset
	HAL_NVIC_SystemReset();
}

static void COMMANDHANDLER_handleRequestVersion(PROTOCOL_t *proto) {
	uint8_t bootloaderVersion[3];

	if (proto->data_len != 0) {
	utils_log_error(
		"HandleRequestVersion failed: Invalid data_len %d, expected 0\r\n",
		proto->data_len);
	COMMANDHANDLER_sendRequestVersionResponse(
		PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID, bootloaderVersion);
	return;
	}

	bootloaderVersion[0] = BOOTLOADER_VERSION_MAJOR;
	bootloaderVersion[1] = BOOTLOADER_VERSION_MINOR;
	bootloaderVersion[2] = BOOTLOADER_VERSION_PATCH;


	COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_RESULT_SUCCESS, bootloaderVersion);
}


static void COMMANDHANDLER_handleOtaStartReprogramming(PROTOCOL_t * proto){
	if (proto->data_len != 13) {
		utils_log_error(
			"HandleOtaStartReprogramming failed: Invalid data_len %d, expected 9\r\n",
			proto->data_len);
		COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
		return;
	}

	uint8_t appVerMajor = proto->data[0];
	uint8_t appVerMinor = proto->data[1];
	uint8_t appVerPatch = proto->data[2];
	uint32_t appAddr = ((uint32_t)proto->data[3] << 24) |
							((uint32_t)proto->data[4] << 16) |
							((uint32_t)proto->data[5] << 8) |
							((uint32_t)proto->data[6]);
	uint32_t appSize = ((uint32_t)proto->data[7] << 24) |
							((uint32_t)proto->data[8] << 16) |
							((uint32_t)proto->data[9] << 8) |
							((uint32_t)proto->data[10]);
	uint16_t appCrc = ((uint16_t)proto->data[11] << 8) | (uint16_t)proto->data[12];

	if(!BOOTLOADER_isAppAddress(appAddr)){
		COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_RESULT_CMD_OTA_ADDRESS_INVALID);
		return;
	}

	if(!BOOTLOADER_enterReProgramming(appVerMajor, appVerMinor, appVerPatch, appSize, appCrc)){
		COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_RESULT_ERROR);
		return;
	}

	COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_RESULT_SUCCESS);
}

static void COMMANDHANDLER_handleOtaStopReprogramming(PROTOCOL_t * proto){
	if (proto->data_len != 0) {
		utils_log_error(
			"HandleOtaStopReprogramming failed: Invalid data_len %d, expected 0\r\n",
			proto->data_len);
		COMMANDHANDLER_sendOtaStopReprogrammingResponse(PROTOCOL_RESULT_COMM_PROTOCOL_DATA_LEN_INVALID);
		return;
	}

	if(!BOOTLOADER_exitReProgramming()){
		COMMANDHANDLER_sendOtaStopReprogrammingResponse(PROTOCOL_RESULT_ERROR);
		return;
	}

	COMMANDHANDLER_sendOtaStopReprogrammingResponse(PROTOCOL_RESULT_SUCCESS);
}

static void COMMANDHANDLER_handleOtaTransferDataReprogramming(PROTOCOL_t * proto){

	uint32_t address = ((uint32_t)proto->data[0] << 24) |
							((uint32_t)proto->data[1] << 16) |
							((uint32_t)proto->data[2] << 8) |
							((uint32_t)proto->data[3]);
	uint8_t *data = &proto->data[4];
	uint32_t dataSize = proto->data_len - 4;

	if(!BOOTLOADER_isAddressValid(address)){
		COMMANDHANDLER_sendOtaTransferDataResponse(PROTOCOL_RESULT_CMD_OTA_ADDRESS_INVALID);
		return;
	}

	if(!BOOTLOADER_rePrograming(address, data, dataSize)){
		COMMANDHANDLER_sendOtaTransferDataResponse(PROTOCOL_RESULT_ERROR);
		return;
	}
}

static void COMMANDHANDLER_sendResetResponse(PROTOCOL_ResultCode_t resultCode) {
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_RESET;
	protocol.data_len = 1;
	protocol.data[0] = resultCode;

	PROTOCOL_send(&protocol);
}
static void COMMANDHANDLER_sendRequestVersionResponse(PROTOCOL_ResultCode_t resultCode,
                                          uint8_t *bootloaderVersion) {
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_REQUEST_VERSION;
	protocol.data_len = 0;
	protocol.data[protocol.data_len++] = resultCode;
	protocol.data[protocol.data_len++] = bootloaderVersion[0];
	protocol.data[protocol.data_len++] = bootloaderVersion[1];
	protocol.data[protocol.data_len++] = bootloaderVersion[2];

	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendOtaStartReprogrammingResponse(PROTOCOL_ResultCode_t resultCode){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_OTA_START_REPROGRAMMING;
	protocol.data_len = 0;
	protocol.data[protocol.data_len++] = resultCode;

	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendOtaStopReprogrammingResponse(PROTOCOL_ResultCode_t resultCode){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_OTA_STOP_REPROGRAMMING;
	protocol.data_len = 0;
	protocol.data[protocol.data_len++] = resultCode;

	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendOtaTransferDataResponse(PROTOCOL_ResultCode_t resultCode){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CMD_OTA_TRANSFER_DATA;
	protocol.data_len = 0;
	protocol.data[protocol.data_len++] = resultCode;

	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_reProgrammingResultCallback(BOOTLOADER_ReProgrammingResult result){
	PROTOCOL_ResultCode_t resultCode = (result == BOOTLOADER_REPROGRAMMING_RESULT_SUCCESS? PROTOCOL_RESULT_SUCCESS : PROTOCOL_RESULT_ERROR);
	COMMANDHANDLER_sendOtaTransferDataResponse(resultCode);
}

static bool COMMANDHANDLER_isCommandValid(PROTOCOL_t *proto) {
	if((proto->protocol_id >= PROTOCOL_ID_CMD_MAX) || (proto->protocol_id <= PROTOCOL_ID_CMD_MIN)){
		return false;
	}
	if(COMMANDHANDLER_handleFuncTable[proto->protocol_id] == NULL){
		return false;
	}
	return true;
}

