/*
 * kb_prog.c
 *
 *  Created on: Feb 14, 2021
 *      Author: makso
 */

#include "main.h"
#include "eee.h"
#include "keycode.h"
#include "kb_prog.h"
#include "kb_matrix.h"

static uint8_t programming_in_progress = 0;
static uint8_t prog_is_running = 0;
static uint8_t prog_error = 0;

static uint32_t key_timer = 0;

static uint32_t next_time = 0;

static uint8_t current_code = 0;
static uint16_t current_pos = 0;
static uint32_t last_prog_time = 0;

// no "make" codes should be left, all should be "broken"
// theoretically, we can simply clean the kbd matrix, but it deletes user keys too (e.g. Shift..)
// if user have pressed any keys while macro plays
// also here can be used simple matrix 32xbytes (for 256 keys), but I wanted to break them in
// reverse order.
static uint8_t code_to_break[PROG_MAX_POS] = {0};

uint8_t is_prog_error(void) { return prog_error; }
uint8_t is_prog_in_progress(void) { return programming_in_progress; }
uint8_t is_prog_long_pressed(void) { return (PROG_LONG_PRESS < (HAL_GetTick() - key_timer)); }

uint8_t is_disco_time(void)
{
	return ((key_timer && (PROG_LONG_PRESS < (HAL_GetTick() - key_timer))) ||
			(programming_in_progress||prog_is_running));
}


/**
 * Array has following composition:
 * | 23 bits of 64ms steps | 1 bit (1 = make) | 8 bits code |
 * first 23 bits -> 64ms steps of delay to next code. If =0 -> this is last code
 * 1 (pos. 9)  bit - 1 if make, 0 if break
 * 8 last bits -> code
 *
 * All written inverted (so empty flash 0xffff -> transforms to 0x0000, which means no code last code)
 */
void prog_push_code(uint8_t code, uint8_t make)
{
	uint32_t packet = 0;
	if (!programming_in_progress)
		return;
	if (current_pos > PROG_MAX_POS - PROG_STEPS_TO_ERROR)
		prog_error = 1;
	if (current_pos == PROG_MAX_POS)
		return;
	if (last_prog_time)
	{
		packet = HAL_GetTick() - last_prog_time;
		if (packet >= (1 << 23))
			packet = ~0;
		packet = (packet >> PROG_TIME_MULT) << 9; // divide 64 and shift time to the right
	}
	if (packet == 0)
		packet = (1<<9);
	last_prog_time = HAL_GetTick();
	if (make)
		packet |= (1<<8);
	packet |= code;
	EEE_write(current_code - KP_START, current_pos++, ~packet);
}

void break_unbreaked(void)
{
	// go from last to first
	for (uint16_t i = PROG_MAX_POS; i > 0; i--)
		if (code_to_break[i-1] != 0)
			matrix_break(code_to_break[i-1]);
}

void prog_pop_code(void)
{
	uint32_t packet;
	if (!prog_is_running)
		return;
	if (next_time < HAL_GetTick())
	{
		if ((packet = ~EEE_read(current_code - KP_START, current_pos++)))
		{
			uint8_t code = (uint8_t)(packet & 0xFF);
			if (packet & (1<<8)) // make or break
			{
				// find first unused position, and add code here
				for (uint16_t i = 0; i < PROG_MAX_POS; i++)
					if (code_to_break[i] == 0)
					{
						code_to_break[i] = code;
						break;
					}
				matrix_make(code);
			}
			else
			{
				// find first position with code and remove it
				for (uint16_t i = 0; i < PROG_MAX_POS; i++)
					if (code_to_break[i] == code)
					{
						code_to_break[i] = 0;
						break;
					}
				matrix_break(code);
			}

			if (current_pos < PROG_MAX_POS)
			{
				packet = ~EEE_read(current_code - KP_START, current_pos);
				next_time = HAL_GetTick() + ((packet>>9)<<PROG_TIME_MULT);
				if (packet)
					return; // completed successfully, wait next step
			}
		}

		break_unbreaked();
		prog_is_running = 0;
		current_pos = 0;
		next_time = 0;
		return;
	}

}


void reset_prog(void)
{
	current_pos = 0;
	last_prog_time = 0;
	prog_error = 0;
	next_time = 0;
}

void prog_pressed(uint8_t code)
{
	if (key_timer)
		return; // another prog key pressed!
	// TODO detect if we are in programming or program is running, and compare code
	if (programming_in_progress && (code != current_code))
		return; // ignore key_press from over keys
	current_code = code; // remember this code!
	key_timer = HAL_GetTick();
}

void prog_released(uint8_t code)
{
	if (code != current_code)
		return; // we are in programming, ignore other prog buttons
	if (key_timer && (PROG_LONG_PRESS < (HAL_GetTick() - key_timer)))
	{
		// key pressed long, start programming
		programming_in_progress = 1;
		EEE_erase(code - KP_START);
	}
	else
	{
		// key pressed short, stop programming or do some nice things
		if (programming_in_progress)
		{
			programming_in_progress = 0;
		}
		else
		{
			reset_prog();
			if (prog_is_running)
				break_unbreaked(); // stopping program
			prog_is_running = !prog_is_running;
		}
	}
	key_timer = 0;
}
