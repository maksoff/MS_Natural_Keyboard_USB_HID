/*
 * eee.c
 *
 *  Created on: Feb 17, 2021
 *      Author: makso
 *
 *      EEPROM EMULATOR (on flash)
 */
// TODO error checks

#include "main.h"
#include "gpio.h"
#include "eee.h"

void EEE_erase(uint8_t page)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);

	HAL_FLASH_Unlock();
   /* Fill EraseInit structure*/
   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
   EraseInitStruct.PageAddress = PAGE_START(page);
   EraseInitStruct.NbPages     = PAGES_PRO_PROGRAM;

   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
   {
	 /*Error occurred while page erase.*/
	  // TODO process error //return HAL_FLASH_GetError ();
   }
	HAL_FLASH_Lock();

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

void EEE_write(uint8_t page, uint16_t pos, uint32_t data)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
	HAL_FLASH_Unlock();
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PAGE_START(page) + (uint32_t)pos*sizeof(data), data) != HAL_OK)
	{
	/* Error occurred while writing data in Flash memory*/
	 // TODO check error. and lock flash return HAL_FLASH_GetError ();
	}
	HAL_FLASH_Lock();
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

uint32_t EEE_read(uint8_t page, uint16_t pos)
{
	uint32_t data;
	data = *(__IO uint32_t *)(PAGE_START(page) + (uint32_t)pos*sizeof(data));
	return data;
}



