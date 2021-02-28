/*
 * kb_prog.h
 *
 *  Created on: Feb 14, 2021
 *      Author: makso
 */

#ifndef INC_KB_PROG_H_
#define INC_KB_PROG_H_

#define PROG_LONG_PRESS (3000)

#define PROG_MAX_POS (PAGE_SIZE*PAGES_PRO_PROGRAM/sizeof(uint32_t))
#define PROG_TIME_MULT (6) // 2^6 = 64ms
#define SPEEDY_CLICKY (1) // 2^1 -> two times

#define PROG_STEPS_TO_ERROR (20)

uint8_t is_prog_long_pressed(void);
uint8_t is_disco_time(void);
uint8_t is_prog_error(void);
uint8_t is_prog_in_progress(void);

void prog_push_code(uint8_t code, uint8_t make);
void prog_pop_code(void);

void prog_pressed(uint8_t code);
void prog_released(uint8_t code);

#endif /* INC_KB_PROG_H_ */
