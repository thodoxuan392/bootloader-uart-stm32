/*
 * statusreporter.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */
#include "App/statusreporter.h"

#include "config.h"
#include "cast.h"
#include "scheduler/scheduler.h"
#include "App/protocol.h"
#include "App/bootloader.h"

#define STATUSREPORT_INTERVAL		 1000 	// 1000ms


// Private function
static void STATUSREPORTER_sendAll(void);
static void STATUSREPORTER_sendStsOta(void);

static void STATUSREPORTER_buildStsOta(PROTOCOL_t *proto, BOOTLOADER_State otaState);

static void STATUSREPORTER_timeout();

static bool STATUSREPORTER_timeout_flag = true;

void STATUSREPORTER_init(){
	// Nothing to do
}

void STATUSREPORTER_run(){
	if(STATUSREPORTER_timeout_flag){
		STATUSREPORTER_timeout_flag = false;

		STATUSREPORTER_sendAll();

		SCH_Add_Task(STATUSREPORTER_timeout, STATUSREPORT_INTERVAL, 0);
	}
}

void STATUSREPORTER_sendAll(void){
	STATUSREPORTER_sendStsOta();
}


static void STATUSREPORTER_sendStsOta(void){
	PROTOCOL_t proto;

	BOOTLOADER_State state =  BOOTLOADER_getState();
	STATUSREPORTER_buildStsOta(&proto, state);

	PROTOCOL_send(&proto);
}

static void STATUSREPORTER_buildStsOta(PROTOCOL_t *proto, BOOTLOADER_State otaState){
	proto->protocol_id = PROTOCOL_ID_STS_OTA_STATUS;
	proto->data_len = 0;
	proto->data[proto->data_len++] = otaState;
}




static void STATUSREPORTER_timeout(){
	STATUSREPORTER_timeout_flag = true;
}
