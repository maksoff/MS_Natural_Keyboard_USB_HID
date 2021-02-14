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
uint8_t keymap_key_to_keycode(uint8_t row, uint8_t col);

/* report id */
#define REPORT_ID_MOUSE     1
#define REPORT_ID_SYSTEM    2
#define REPORT_ID_CONSUMER  3


/* Consumer Page(0x0C)
 * following are supported by Windows: http://msdn.microsoft.com/en-us/windows/hardware/gg463372.aspx
 */
#define AUDIO_MUTE              0x00E2
#define AUDIO_VOL_UP            0x00E9
#define AUDIO_VOL_DOWN          0x00EA
#define TRANSPORT_NEXT_TRACK    0x00B5
#define TRANSPORT_PREV_TRACK    0x00B6
#define TRANSPORT_STOP          0x00B7
#define TRANSPORT_STOP_EJECT    0x00CC
#define TRANSPORT_PLAY_PAUSE    0x00CD
/* application launch */
#define AL_CC_CONFIG            0x0183
#define AL_EMAIL                0x018A
#define AL_CALCULATOR           0x0192
#define AL_LOCAL_BROWSER        0x0194
/* application control */
#define AC_SEARCH               0x0221
#define AC_HOME                 0x0223
#define AC_BACK                 0x0224
#define AC_FORWARD              0x0225
#define AC_STOP                 0x0226
#define AC_REFRESH              0x0227
#define AC_BOOKMARKS            0x022A
/* supplement for Bluegiga iWRAP HID(not supported by Windows?) */
#define AL_LOCK                 0x019E
#define TRANSPORT_RECORD        0x00B2
#define TRANSPORT_REWIND        0x00B4
#define TRANSPORT_EJECT         0x00B8
#define AC_MINIMIZE             0x0206

/* Generic Desktop Page(0x01) - system power control */
#define SYSTEM_POWER_DOWN       0x0081
#define SYSTEM_SLEEP            0x0082
#define SYSTEM_WAKE_UP          0x0083

/*
 * keyboard report is 8-byte array retains state of 8 modifiers and 6 keys.
 *
 * byte |0       |1       |2       |3       |4       |5       |6       |7
 * -----+--------+--------+--------+--------+--------+--------+--------+--------
 * desc |mods    |reserved|keys[0] |keys[1] |keys[2] |keys[3] |keys[4] |keys[5]
 *
 * It is exended to 16 bytes to retain 120keys+8mods when NKRO mode.
 *
 * byte |0       |1       |2       |3       |4       |5       |6       |7        ... |15
 * -----+--------+--------+--------+--------+--------+--------+--------+--------     +--------
 * desc |mods    |bits[0] |bits[1] |bits[2] |bits[3] |bits[4] |bits[5] |bits[6]  ... |bit[14]
 *
 * mods retains state of 8 modifiers.
 *
 *  bit |0       |1       |2       |3       |4       |5       |6       |7
 * -----+--------+--------+--------+--------+--------+--------+--------+--------
 * desc |Lcontrol|Lshift  |Lalt    |Lgui    |Rcontrol|Rshift  |Ralt    |Rgui
 *
 */


/* keycode to system usage */ // TODO  WTF
#define KEYCODE2SYSTEM(key) \
    (key == KC_SYSTEM_POWER ? SYSTEM_POWER_DOWN : \
    (key == KC_SYSTEM_SLEEP ? SYSTEM_SLEEP : \
    (key == KC_SYSTEM_WAKE  ? SYSTEM_WAKE_UP : 0)))

/* keycode to consumer usage */ // TODO  WTF
#define KEYCODE2CONSUMER(key) \
    (key == KC_AUDIO_MUTE       ?  AUDIO_MUTE : \
    (key == KC_AUDIO_VOL_UP     ?  AUDIO_VOL_UP : \
    (key == KC_AUDIO_VOL_DOWN   ?  AUDIO_VOL_DOWN : \
    (key == KC_MEDIA_NEXT_TRACK ?  TRANSPORT_NEXT_TRACK : \
    (key == KC_MEDIA_PREV_TRACK ?  TRANSPORT_PREV_TRACK : \
    (key == KC_MEDIA_STOP       ?  TRANSPORT_STOP : \
    (key == KC_MEDIA_EJECT      ?  TRANSPORT_STOP_EJECT : \
    (key == KC_MEDIA_PLAY_PAUSE ?  TRANSPORT_PLAY_PAUSE : \
    (key == KC_MEDIA_SELECT     ?  AL_CC_CONFIG : \
    (key == KC_MAIL             ?  AL_EMAIL : \
    (key == KC_CALCULATOR       ?  AL_CALCULATOR : \
    (key == KC_MY_COMPUTER      ?  AL_LOCAL_BROWSER : \
    (key == KC_WWW_SEARCH       ?  AC_SEARCH : \
    (key == KC_WWW_HOME         ?  AC_HOME : \
    (key == KC_WWW_BACK         ?  AC_BACK : \
    (key == KC_WWW_FORWARD      ?  AC_FORWARD : \
    (key == KC_WWW_STOP         ?  AC_STOP : \
    (key == KC_WWW_REFRESH      ?  AC_REFRESH : \
    (key == KC_WWW_FAVORITES    ?  AC_BOOKMARKS : 0)))))))))))))))))))


#endif /* INC_KEYBOARD_H_ */
