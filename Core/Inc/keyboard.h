/*
 * keyboard.h
 *
 *  Created on: Feb 13, 2021
 *      Author: makso
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

/* key report size */
#define REPORT_SIZE 8
#define REPORT_KEYS 6

typedef union {
    uint8_t raw[REPORT_SIZE];
    struct {
        uint8_t mods;
        uint8_t reserved;
        uint8_t keys[REPORT_KEYS];
    };
} report_keyboard_t;

void process_keyboard_USB(void);


#endif /* INC_KEYBOARD_H_ */
