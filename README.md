# MS_Natural_Keyboard_USB_HID

So, idea was/is to use old but very good Microsoft Natural Keyboard with PS/2 interface, and add new and shiny USB interface.

So, after some research and combinations I adapted keyboard for HID/USB.

Some features: F1...F12 now are always F1...F12 (no special F keys anymore)
Logoff key mapped to Win+L (I couldn't find any HID code for this function... Standart one (Logoff/Screensaver...) didn't work)
"WWW Home", "Mail" & "Player" keys are still mapped.

"My Documents", "My Pictures", "My Videos" & "Messenger" - now are macro programming keys (saved to Flash), can save up to 256 keystrokes per page. (256 press & 256 release). 
Time quanting = 64ms, max Time = 6 days.

To start programming press & hold button for 3 seconds (leds start blinking). Press again to finish. 
If programming started with left Ctrl -> macro recorded with 2x speed. If pressed with right Ctrl -> macro recorded at max speed (64ms delay between keypresses)
(Ctrl should be released before Macro key is released)

Press shortly to play sequence. (left Ctrl -> 2x speed, right Ctrl -> max speed (64ms))

Caps <-> Num blinking sequently => Programming
Caps & Num blinking synchronously => last 10 keystrokes left

Used BluePill, added USB_P 1.5k Pull-up to Pin, removed 10k, series Resistor changed to 68R.

About PS2 connections:
https://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf
http://www.lucadavidian.com/2017/11/15/interfacing-ps2-keyboard-to-a-microcontroller/
http://www.burtonsys.com/ps2_chapweske.htm

Some setups for PS2:
https://controllerstech.com/how-to-use-stm32-as-a-keyboard/

Very great code for PS/2 to USB-HID converter:
https://github.com/robszy/ps2usb (need to be udpated tho)
https://github.com/tmk/tmk_keyboard/blob/master/keyboard/stm32_f103_onekey/matrix.c

About HID descriptors for multiple reports:
https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/

How to read Caps/Num lock from PC:
https://community.st.com/s/question/0D50X00009Xkh3g/usb-hid-keyboard-leds

HID descriptor tool: // added custom codes; this tool can't do everything :(
https://www.usb.org/document-library/hid-descriptor-tool

HID keycodes: 
https://usb.org/sites/default/files/hut1_21_0.pdf
https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/translate.pdf

Some extra scancodes from MS keyboards:
https://www.win.tue.nl/~aeb/linux/kbd/scancodes-6.html

https://controllerstech.com/flash-programming-in-stm32/

Disassembly of keyboard:
https://www.ifixit.com/Guide/Microsoft+Natural+Multimedia+Keyboard+disassembly+and+cleaning/11799
