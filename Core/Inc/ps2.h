/*
 * ps2.h
 *
 *  Created on: Feb 9, 2021
 *      Author: maksoff
 */

/*
 * Primitive PS/2 Library for AVR <- from git robszy\ps2usb
 *
 * PS/2 Resources
 * --------------
 * [1] The PS/2 Mouse/Keyboard Protocol
 * http://www.computer-engineering.org/ps2protocol/
 * Concise and thorough primer of PS/2 protocol.
 *
 * [2] Keyboard and Auxiliary Device Controller
 * http://www.mcamafia.de/pdf/ibm_hitrc07.pdf
 * Signal Timing and Format
 *
 * [3] Keyboards(101- and 102-key)
 * http://www.mcamafia.de/pdf/ibm_hitrc11.pdf
 * Keyboard Layout, Scan Code Set, POR, and Commands.
 *
 * [4] PS/2 Reference Manuals
 * http://www.mcamafia.de/pdf/ibm_hitrc07.pdf
 * Collection of IBM Personal System/2 documents.
 *
 * [5] TrackPoint Engineering Specifications for version 3E
 * https://web.archive.org/web/20100526161812/http://wwwcssrv.almaden.ibm.com/trackpoint/download.html
 */

#ifndef INC_PS2_H_
#define INC_PS2_H_


#define USB_LED_NUM_LOCK 	(1<<0)
#define USB_LED_CAPS_LOCK	(1<<1)
#define USB_LED_SCROLL_LOCK	(1<<2)

#define PS2_LED_SCROLL_LOCK (1<<0)
#define PS2_LED_NUM_LOCK    (1<<1)
#define PS2_LED_CAPS_LOCK   (1<<2)

void init_PS2(void);
void process_PS2(void);
void leds_PS2(uint8_t led);

uint8_t leds_updated;
uint8_t leds_data;

#endif /* INC_PS2_H_ */
