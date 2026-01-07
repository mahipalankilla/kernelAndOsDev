#ifndef CLASSICPS2_H
#define CLASSICPS2_H
#include "keyboard.h"

#define PS2_PORT_NUMBER 0x64
#define FIRST_PS2_PORT_ENABLE 0xAE

#define CLASSIC_KEYBOARD_KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

struct keyboard* classic_init();
#endif