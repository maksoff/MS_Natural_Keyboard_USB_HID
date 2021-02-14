/*
 * keyboard.c
 *
 *  Created on: Feb 13, 2021
 *      Author: makso
 */

#include "main.h"
#include "keycode.h"
#include "keyboard.h"
#include "usbd_hid.h"

uint8_t ready_to_send = 1;

extern USBD_HandleTypeDef hUsbDeviceFS;

void send_keyboard_report(void);

report_keyboard_t keyboard_report;
uint8_t mods = 0;

typedef struct  {
	uint8_t  report_id;
	uint16_t usage;
} __attribute__ ((packed))  report_extra_t;

static void send_system(uint16_t data)
{
	static uint16_t last_data = 0;
	if (data == last_data) return;
	last_data = data;

	report_extra_t report = {
		.report_id = REPORT_ID_SYSTEM,
		.usage = data
	};
	ready_to_send = 0;
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&report, sizeof (report_extra_t));
//	if (usbInterruptIsReady3()) {
//		usbSetInterrupt3((void *)&report, sizeof(report));
	// TODO
//	}
}

static void send_consumer(uint16_t data)
{
	static uint16_t last_data = 0;
	if (data == last_data) return;
	last_data = data;

	report_extra_t report = {
		.report_id = REPORT_ID_CONSUMER,
		.usage = data
	};
	ready_to_send = 0;
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&report, sizeof (report_extra_t));
//	if (usbInterruptIsReady3()) {
//		usbSetInterrupt3((void *)&report, sizeof(report));
	// TODO
//	}
}


void add_mods(uint8_t amods) { mods |= amods; }
void del_mods(uint8_t amods) { mods &= ~amods; }


static inline void add_key_byte(uint8_t code)
{
    int8_t i = 0;
    int8_t empty = -1;
    for (; i < REPORT_KEYS; i++) {
        if (keyboard_report.keys[i] == code) {
            break;
        }
        if (empty == -1 && keyboard_report.keys[i] == 0) {
            empty = i;
        }
    }
    if (i == REPORT_KEYS) {
        if (empty != -1) {
            keyboard_report.keys[empty] = code;
        }
    }
}

static inline void del_key_byte(uint8_t code)
{
    for (uint8_t i = 0; i < REPORT_KEYS; i++) {
        if (keyboard_report.keys[i] == code) {
            keyboard_report.keys[i] = 0;
        }
    }
}

void clear_keys(void)
{
    // not clear mods
    for (int8_t i = 1; i < REPORT_SIZE; i++) {
        keyboard_report.raw[i] = 0;
    }
}



uint8_t keymap_key_to_keycode(uint8_t row, uint8_t col)
{
	return keymaps[0][row][col];
}


void register_code(uint8_t code)
{
    if (code == KC_NO) {
        return;
    }

    else if IS_KEY(code) {
	    add_key_byte(code);
	    send_keyboard_report();
    }
    else if IS_MOD(code) {
        add_mods(MOD_BIT(code));
        send_keyboard_report();
    }
    else if IS_SYSTEM(code) {
		send_system(KEYCODE2SYSTEM(code));
	}
    else if IS_CONSUMER(code) {
        send_consumer(KEYCODE2CONSUMER(code)); //TODO: consumer keys to change volume etc.
    }
}

void unregister_code(uint8_t code)
{
    if (code == KC_NO) {
        return;
    }
    else if IS_KEY(code) {
        del_key_byte(code);
        send_keyboard_report();
    }
    else if IS_MOD(code) {
        del_mods(MOD_BIT(code));
        send_keyboard_report();
    }
    else if IS_SYSTEM(code) {
	    send_system(0);
    }
    else if IS_CONSUMER(code) {
    	send_consumer(0);
	}
}




/* buffer for keyboard */
#define KBUF_SIZE 16
report_keyboard_t kbuf[KBUF_SIZE];
uint8_t kbuf_head = 0;
uint8_t kbuf_tail = 0;

void kbuf_push(report_keyboard_t *report);
report_keyboard_t kbuf_pop(void);
uint8_t kbuf_has_data(void);
void kbuf_clear(void);
/*********************/


void process_keyboard_USB(void)
{
#define cnt_max (250)
	static uint16_t cnt = cnt_max;
	if (kbuf_head != kbuf_tail)
	{
		if (ready_to_send)
		{
			ready_to_send = 0;
			cnt = cnt_max;
			// TODO send this data!
			USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&kbuf[kbuf_tail], sizeof (report_keyboard_t));
	        kbuf_tail = (kbuf_tail + 1) % KBUF_SIZE;
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

void kbuf_push(report_keyboard_t *report)
{
    uint8_t next = (kbuf_head + 1) % KBUF_SIZE;
    // don't add new values if buffer is already full
    if (next != kbuf_tail) {
        kbuf[kbuf_head] =*report;
        kbuf_head = next;
    }
}

uint8_t kbuf_has_data(void)
{
    return (kbuf_head != kbuf_tail);
}

void kbuf_clear(void)
{
    kbuf_head = kbuf_tail = 0;
}

void send_keyboard_report(void){
	keyboard_report.mods = mods;
	kbuf_push(&keyboard_report);
}

//////

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: default
     * ,---.   ,---------------. ,---------------. ,---------------. ,-----------.     ,-----------.
     * |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau|     |Pwr|Slp|Wak|
     * `---'   `---------------' `---------------' `---------------' `-----------'     `-----------'
     * ,-----------------------------------------------------------. ,-----------. ,---------------.
     * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backspa| |Ins|Hom|PgU| |NmL|  /|  *|  -|
     * |-----------------------------------------------------------| |-----------| |---------------|
     * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|    \| |Del|End|PgD| |  7|  8|  9|   |
     * |-----------------------------------------------------------| `-----------' |-----------|  +|
     * |CapsLo|  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |               |  4|  5|  6|   |
     * |-----------------------------------------------------------|     ,---.     |---------------|
     * |Shift   |  Z|  X|  C|  V|  B|  N|  M|  ,|  ,|  /|Shift     |     |Up |     |  1|  2|  3|   |
     * |-----------------------------------------------------------| ,-----------. |-----------|Ent|
     * |Ctrl |Gui |Alt |         Space         |Alt |Gui |Menu|Ctrl| |Lef|Dow|Rig| |      0|  .|   |
     * `-----------------------------------------------------------' `-----------' `---------------'
     */
    KEYMAP(
    ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,BRK,
    GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,
    TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSLS,     DEL, END, PGDN,    P7,  P8,  P9,
    CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     ENT,                         P4,  P5,  P6,  PPLS,
    LSFT,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,          RSFT,          UP,           P1,  P2,  P3,
    LCTL,LGUI,LALT,          SPC,                     RALT,RGUI,APP, RCTL,     LEFT,DOWN,RGHT,    P0,       PDOT,PENT
    ),
};
