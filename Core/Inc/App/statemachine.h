/*
 * statemachine.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

typedef enum {
	STATEMACHINE_STATE_NORMAL,
	STATEMACHINE_STATE_ERROR,
	STATEMACHINE_STATE_LOCKED,
	STATEMACHINE_STATE_DISCONNECTED,
}STATEMACHINE_State;

void STATEMACHINE_init(void);
void STATEMACHINE_run(void);

void STATEMACHINE_ping(void);
STATEMACHINE_State STATEMACHINE_getState(void);



#endif /* INC_APP_STATEMACHINE_H_ */
