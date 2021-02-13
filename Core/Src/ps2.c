/*
 * ps2.c
 *
 *  Created on: Feb 9, 2021
 *      Author: maksoff
 *
 *
 * *****************************
 * host samples/updates data on falling edge
 *
 * it is hard to decide, if State Machine or Simple bit numbering is better
 *
 * I've decided to use bit numbering, it makes more sense (I hope)
 *
 * device -> host (data read on falling edge)
 *  0 - START
 *  1...8 - DATA (LSB first)
 *  9 - odd parity
 * 10 - STOP
 *
 * host -> device (data prepared on falling edge)
 *  0...7 - DATA (LSB first)
 *  8 - odd parity
 *  9 - STOP
 * 10 - read ACK (on falling edge)
 *
 */

#include "main.h"
#include "tim.h"
#include "ps2.h"
#include "gpio.h"
#include "usbd_conf.h"
#include "kb_matrix.h"

#define PS2_RESET		0xFF
#define PS2_ACK         0xFA
#define PS2_RESEND      0xFE
#define PS2_DEFAULT		0xF6
#define PS2_DISABLE		0xF5
#define PS2_ENABLE		0xF4
#define PS2_ID			0xF2
#define PS2_ECHO		0xEE
#define PS2_SET_LED     0xED
#define PS2_SELF_TESTED	0xAA

uint8_t sendMode = 0;
uint8_t outputData = 0;
int8_t bitNr = 0;

uint8_t leds_updated;
uint8_t leds_data;

uint8_t sleep = 0;

void send_PS2(uint8_t data);
void leds_PS2(uint8_t led);
uint8_t wait_response_PS2(void);

/******************/
/* buffer for PS2 */
#define PBUF_SIZE 64
uint8_t pbuf[PBUF_SIZE];
uint8_t pbuf_head = 0;
uint8_t pbuf_tail = 0;

void buf_push(uint8_t data);
uint8_t buf_pop(void);
uint8_t buf_has_data(void);
void buf_clear(void);
/******************/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint8_t parity = 0;
	static uint8_t data = 0;
	if (GPIO_Pin == PS2_CLK_Pin)
	{
		if (sendMode)
		{
			/** we are sending! **/

			if (sendMode == 1)
			{
				// ignore first interrupt, we are signaling to device
				sendMode = 2;
				bitNr = 0;
				parity = 0;
				return;
			}

			if (bitNr <= 7)
			{
				// shift out the bits, LSB first
				if (outputData & (1<<bitNr)) {
					parity = !parity;
					data_release();
				} else {
					data_low();
				}
			}
			else if (bitNr == 8)
			{
				// send parity
				if (!parity)
					data_release();
				else
					data_low();

			}
			else if (bitNr == 9)
			{
				// send stop bit
				data_release();
			}
			else if ( bitNr == 10 )
			{
				data_release();
				sendMode = 0;
				// do I need to check ACK? Don't think so =>
				// we are receiving clock already, so device is alive
				parity = 0;
				bitNr = 0;
				return;
			}
			bitNr++;
			return;
		}
		else
		{
			/** We are receiving! **/

			if (!check_us_counter(200)) // from last received bit is more than 200us! Probably new data.
			{
				bitNr = 0;
			}
			reset_us_counter();
			if (bitNr == 0)
			{
				// receiving START bit
				if (HAL_GPIO_ReadPin(PS2_DATA_GPIO_Port, PS2_DATA_Pin))
					return; // it should be start bit! If not, just ignore it
				data = 0;
				parity = 0;
			}
			else if (bitNr <= 8)
			{
				// shift in data bits
				data >>= 1; // LSB received first
				if (HAL_GPIO_ReadPin(PS2_DATA_GPIO_Port, PS2_DATA_Pin))
				{
					data |= 0x80;
					parity = !parity;
				}

			}
			else if (bitNr == 9)
			{
				// receive parity bit
				if (HAL_GPIO_ReadPin(PS2_DATA_GPIO_Port, PS2_DATA_Pin))
					parity = !parity;
			}
			else if (bitNr == 10)
			{
				// receive STOP bit
				if (HAL_GPIO_ReadPin(PS2_DATA_GPIO_Port, PS2_DATA_Pin) && (parity))
					buf_push(data); // STOP bit issued, no parity errors
				bitNr = 0;
				return;
			}

		}
		bitNr++;
	}
}


/**
 * @brief RESETs keyboard, checks for ACK, waits 2.5sec
 */
void init_PS2(void)
{
	for(;;)
	{
		send_PS2(PS2_RESET);
		if (PS2_ACK == wait_response_PS2())
			break;
		_delay_ms(1000);
	}
	for (uint8_t i = 100; i > 0; i--) // wait 2.5 sec (100*25ms) or SELF-TEST from keyboard
	{
		if (PS2_SELF_TESTED == wait_response_PS2())
			break;
	}
}

/**
 * @brief issues send sequence, sets signaling bit
 * @param data: BYTE to be sent
 */
void send_PS2(uint8_t data)
{
	sendMode = 1;
	clock_low();
	bitNr = 0;
	outputData = data;
	_delay_us(100);

	data_low();
	_delay_us(10);
	clock_release();
}

void SET_LEDS_Callback(uint8_t usb_led)
{
	// Interestingly, USB HID not always receives the LED status
	// Second USB keyboard receives update always
	// Maybe Prio? Have reduced tick prio, doesn't help
	// Maybe Endpoint problem? No Idea.
	uint8_t data = 0;
	if (usb_led & USB_LED_CAPS_LOCK)
		data |= PS2_LED_CAPS_LOCK;
	if (usb_led & USB_LED_NUM_LOCK)
		data |= PS2_LED_NUM_LOCK;
	if (usb_led & USB_LED_SCROLL_LOCK)
		data |= PS2_LED_SCROLL_LOCK;
	leds_updated = 1;
	leds_data = data;
}

void USBD_Sleep_Callback(uint8_t enter_exit)
{
	// high if enters in sleep
	sleep = enter_exit + 1;
}

/**
 * @brief send LED state to keyboard
 * @param led: LEDs state (in PS2 format)
 * */
void leds_PS2(uint8_t led)
{
	if (!leds_updated)
		return;
	if (buf_has_data())
		return; // do nothing if buffer is not clear - user input is more important!
	leds_updated = 0;

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
	for (uint8_t i = 5; i > 0; i--){
		send_PS2(PS2_SET_LED);
		if ( wait_response_PS2() != PS2_ACK ) // no ACK, try again
		{
			_delay_ms(20);
			continue;
		}
		send_PS2(led);
		if ( wait_response_PS2() == PS2_ACK )
		{
			return;
		}
		_delay_ms(20); // no ACK, try again
	}
}

/**
 * @brief used together with send, waits response for 25ms
 * @retval returns received byte or 0
 */
uint8_t wait_response_PS2(void)
{
	// Command may take 25ms/20ms at most([5]p.46, [3]p.21)
	for(uint8_t i = 25; i > 0; i--)
	{
		if (buf_has_data())
			break;
		_delay_ms(1);
	}
	return buf_pop(); // returns 0 in case of time overflow
}

uint8_t read_PS2(void)
{
	return buf_pop(); // if no data in buffer, returns 0
}

/**
 * @brief if any data in buffer, or new LEDs, process it
 */
void process_PS2(void)
{
	if (sleep)
	{
		leds_updated = 1;
		if (sleep == 2) // we are entering sleep
		{
			leds_PS2(0);
		}
		else // we are exiting sleep
		{
			leds_PS2(leds_data);
		}
		sleep = 0;
	}
	if (buf_has_data())
		matrix_scan(buf_pop());
	else
		leds_PS2(leds_data);
}

/********************************/
/* buffer for PS2 */

void buf_push(uint8_t data)
{
    uint8_t next = (pbuf_head + 1) % PBUF_SIZE;
    // don't add new values if buffer is already full
    if (next != pbuf_tail) {
        pbuf[pbuf_head] = data;
        pbuf_head = next;
    }
}

uint8_t buf_pop(void)
{
    uint8_t val = 0;
    // if buffer is empty, return 0
    if (pbuf_head != pbuf_tail) {
        val = pbuf[pbuf_tail];
        pbuf_tail = (pbuf_tail + 1) % PBUF_SIZE;
    }
    return val;
}

uint8_t buf_has_data(void)
{
    return (pbuf_head != pbuf_tail);
}

void buf_clear(void)
{
    pbuf_head = pbuf_tail = 0;
}
