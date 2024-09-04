/*
 * rtc.c
 *
 *  Created on: Sep 2, 2024
 *      Author: xuanthodo
 */

#include <Hal/rtc.h>

#include <main.h>

#define RTC_BKP_REG_DATE	RTC_BKP_DR5

static void RTC_dateToUint32(RTC_DateTypeDef *date, uint32_t *dateU32);
static void RTC_uint32ToDate( uint32_t *dateU32, RTC_DateTypeDef *date);


RTC_HandleTypeDef hrtc = {
	.Instance = RTC,
	.Init = {
		.AsynchPrediv = RTC_AUTO_1_SECOND,
		.OutPut = RTC_OUTPUTSOURCE_NONE
	},
};

void RTC_init(void){
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
	uint32_t dateRegData = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_REG_DATE);
	RTC_uint32ToDate(&dateRegData, &hrtc.DateToUpdate);
}

void RTC_deinit(void){
	HAL_RTC_DeInit(&hrtc);
}

bool RTC_get(RTC_t *rtc){
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	if(HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK){
		return false;
	}
	if(HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK){
		return false;
	}
	rtc->year = date.Year;
	rtc->month = date.Month;
	rtc->date = date.Date;
	rtc->hour = time.Hours;
	rtc->minute = time.Minutes;
	rtc->second = time.Seconds;
	return true;
}

bool RTC_set(RTC_t *rtc){
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	date.Year = rtc->year;
	date.Month = rtc->month;
	date.Date = rtc->date;
	time.Hours = rtc->hour;
	time.Minutes = rtc->minute;
	time.Seconds = rtc->second;
	if(HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK){
		return false;
	}
	if(HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK){
		return false;
	}
	// Backup Date
	uint32_t dateRegData;
	RTC_dateToUint32(&hrtc.DateToUpdate, &dateRegData);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_REG_DATE, dateRegData);
	return true;
}

bool RTC_isExpired(RTC_t current, RTC_t expireTime){
	/*---------------Year-------------- */
	if(current.year < expireTime.year){
		// Not expired yet
		return false;
	}
	if(current.year > expireTime.year){
		// Expired
		return true;
	}

	/*---------------Month-------------- */
	if(current.month < expireTime.month){
		// Not expired yet
		return false;
	}
	if(current.month > expireTime.month){
		// Expired
		return true;
	}

	/*---------------Date-------------- */
	if(current.date < expireTime.date){
		// Not expired yet
		return false;
	}
	if(current.date > expireTime.date){
		// Expired
		return true;
	}

	/*---------------Hour-------------- */
	if(current.hour < expireTime.hour){
		// Not expired yet
		return false;
	}
	if(current.hour > expireTime.hour){
		// Expired
		return true;
	}
	return true;
}

static void RTC_dateToUint32(RTC_DateTypeDef *date, uint32_t *dateU32){
	uint32_t ret = 0;
	ret |= (uint32_t)date->WeekDay << 24;
	ret |= (uint32_t)date->Month << 16;
	ret |= (uint32_t)date->Date << 8;
	ret |= (uint32_t)date->Year;
	*dateU32 = ret;
}

static void RTC_uint32ToDate( uint32_t *dateU32, RTC_DateTypeDef *date){
	date->WeekDay = (*dateU32) >> 24;
	date->Month = (*dateU32) >> 16;
	date->Date = (*dateU32) >> 8;
	date->Year = (*dateU32);
}
