#include "main.h"
#include <Hal/flash.h>


static void FLASH_lock();
static void FLASH_unlock();

bool FLASH_erase(uint32_t addr,uint32_t num_bytes)
{
	bool success;
	uint32_t num_page = (uint32_t)(num_bytes/FLASH_PAGE_SIZE)+1;
	uint32_t page_error;
	FLASH_unlock();
	FLASH_EraseInitTypeDef erase_instance;
	erase_instance.TypeErase = FLASH_TYPEERASE_PAGES;
	erase_instance.NbPages = num_page;
	erase_instance.PageAddress = addr;
	success = (HAL_FLASHEx_Erase(&erase_instance, &page_error) == HAL_OK);
	FLASH_lock();
	return success;
}

bool FLASH_write_int(uint32_t addr, uint16_t data)
{
	FLASH_unlock();
	FLASH->CR |= FLASH_CR_PG;				/*!< Programming */
	while((FLASH->SR&FLASH_SR_BSY));
	*(__IO uint16_t*)addr = data;
	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH_lock();
	return true;
}

bool FLASH_read_int(uint32_t addr, uint16_t* val)
{
	*val = *(uint16_t *)addr;
	return true;
}

bool FLASH_write_buf(uint32_t addr, uint8_t* data, uint32_t data_len)
{
	FLASH_unlock();
	int i;
	FLASH->CR |= FLASH_CR_PG;
	for(i=0; i < data_len; i+=2)
	{
		while((FLASH->SR&FLASH_SR_BSY));
		*(__IO uint16_t*)(addr + i) = *(uint16_t *)(data+i);
	}
	while((FLASH->SR&FLASH_SR_BSY)){};
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH_lock();
	return true;
}

bool FLASH_read_buf(uint32_t addr1 , uint8_t* data, uint32_t data_len)
{
	uint16_t temp_data;
	for (int var = 0; var < data_len; var+=2) {
		FLASH_read_int(addr1 + var, &temp_data);
		data[var+1] = (temp_data&0xFF00)>> 8;
		data[var] = temp_data;
	}
	return true;
}

static void FLASH_lock()
{
	HAL_FLASH_Lock();
}

static void FLASH_unlock()
{
	HAL_FLASH_Unlock();
}
