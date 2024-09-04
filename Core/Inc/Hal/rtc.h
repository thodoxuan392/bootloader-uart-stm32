/*
 * rtc.h
 *
 *  Created on: Sep 2, 2024
 *      Author: xuanthodo
 */

#ifndef INC_HAL_RTC_H_
#define INC_HAL_RTC_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}RTC_t;

void RTC_init(void);
void RTC_deinit(void);

bool RTC_get(RTC_t *rtc);
bool RTC_set(RTC_t *rtc);
bool RTC_isExpired(RTC_t current, RTC_t expireTime);

#endif /* INC_HAL_RTC_H_ */
