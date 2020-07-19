#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "../utils/data_types.h"
#include "../utils/bit_handle.h"

#define KEYPAD_DIRC		(*(volatile uint8*) 0x31)
#define KEYPAD_PORT		(*(volatile uint8*) 0x32)
#define KEYPAD_PIN		(*(volatile uint8*) 0x30)

void keypad_init();
uint8 keypad_getKey();
uint8 key_wait_pressed();

#endif /* KEYPAD_H_ */