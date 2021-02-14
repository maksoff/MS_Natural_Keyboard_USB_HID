/*
 * keyboard.c
 *
 *  Created on: Feb 13, 2021
 *      Author: makso
 */


/*********************/

#include "main.h"
#include "keyboard.h"
#include "usbd_hid.h"

/* buffer for keyboard */
#define KBUF_SIZE 16
report_keyboard_t kbuf[KBUF_SIZE];
uint8_t kbuf_head = 0;
uint8_t kbuf_tail = 0;

void kbuf_push(report_keyboard_t data);
report_keyboard_t kbuf_pop(void);
uint8_t kbuf_has_data(void);
void kbuf_clear(void);
/*********************/

uint8_t ready_to_send = 1;

void process_keyboard_USB(void)
{
#define cnt_max (250)
	static uint16_t cnt = cnt_max;
	if (kbuf_has_data())
	{
		if (ready_to_send)
		{
			cnt = cnt_max;
			// TODO send this data!
		}
		else if (--cnt == 0) // timeout, just push this data
			ready_to_send = 1;
	}
}

void USB_HID_buffer_sent_Callback(void)
{
	ready_to_send = 1;
}






/********************************/
/* buffer for Keyboard */

void kbuf_push(report_keyboard_t data)
{
    uint8_t next = (kbuf_head + 1) % KBUF_SIZE;
    // don't add new values if buffer is already full
    if (next != kbuf_tail) {
        kbuf[kbuf_head] = data;
        kbuf_head = next;
    }
}

report_keyboard_t kbuf_pop(void)
{
	report_keyboard_t val;
    // if buffer is empty, return 0
    if (kbuf_head != kbuf_tail) {
        val = kbuf[kbuf_tail];
        kbuf_tail = (kbuf_tail + 1) % KBUF_SIZE;
    }
    return val;
}

uint8_t kbuf_has_data(void)
{
    return (kbuf_head != kbuf_tail);
}

void kbuf_clear(void)
{
    kbuf_head = kbuf_tail = 0;
}
