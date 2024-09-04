#ifndef FLASH_H
#define FLASH_H


#include "stdint.h"
#include "string.h"
#include "stdbool.h"

bool 	FLASH_erase(uint32_t addr,uint32_t num_bytes);
bool 	FLASH_write_int(uint32_t addr, uint16_t data);
bool 	FLASH_read_int(uint32_t addr, uint16_t * data);
bool 	FLASH_write_buf(uint32_t addr, uint8_t * data , uint32_t data_len);
bool 	FLASH_read_buf(uint32_t addr, uint8_t * data, uint32_t data_len);


#endif	// FLASH_H
