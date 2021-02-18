/*
 * eee.h
 *
 *  Created on: Feb 17, 2021
 *      Author: makso
 *
 * EEPROM EMULATOR (on flash)
 */

#ifndef INC_EEE_H_
#define INC_EEE_H_

#define PAGES_PRO_PROGRAM (2) // use 2 pages pro one virtual page

#define PAGE_SIZE  ((uint16_t)0x400)  /* Page size = 1KByte */

/* EEPROM end address in Flash */
#define EEPROM_END_ADDRESS    ((uint32_t)0x08010000) /* EEPROM emulation end address:
                                                  	  	  for 64KByte Flash memory */

#define PAGE_COUNT (4) // 4 programmable buttons = 4 pages

#define PAGE_START(page) 	(EEPROM_END_ADDRESS - ((uint32_t)(PAGE_COUNT - page)) * PAGES_PRO_PROGRAM * PAGE_SIZE)
//#define PAGE_END(page)		(EEPROM_END_ADDRESS - (PAGE_COUNT - page - 1) * PAGE_SIZE - 1)

void EEE_erase(uint8_t page);
void EEE_write(uint8_t page, uint16_t pos, uint32_t data);
uint32_t EEE_read(uint8_t page, uint16_t pos);

#endif /* INC_EEE_H_ */
