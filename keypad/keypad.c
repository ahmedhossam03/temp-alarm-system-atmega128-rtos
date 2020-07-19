#include "keypad.h"

void keypad_init()
{
	//initialize the first 4 bits (rows) as output and the last 4 (cols) as inputs
	KEYPAD_DIRC = 0x0F;
	//initialize rows with high and cols as input pull up
	KEYPAD_PORT = 0xFF;
}

uint8 key;
uint8 keypad_getKey()
{
	key = 0;
	uint8 rows = 0;
	uint8 cols = 0;
	for(rows = 0; rows < 4; rows++)
	{
		//set a single row to low
		clearBit(KEYPAD_PORT, rows);
		//look for the column that reads the low bit
		for(cols = 4; cols < 8; cols++)
		{
			if((readBit(KEYPAD_PIN, cols)) == 0)
			{
				key = (cols - 4) + (rows * 4) + 1;		//eqn fto get key
				while((readBit(KEYPAD_PIN, cols)) == 0);
			}
		}
		//set row back to high
		setBit(KEYPAD_PORT, rows);
	}
	//return key
	return key;
}

//this function waits for key to be pressed and return, not just return 0 when nothing pressed
uint8 key_wait_pressed()
{
	uint8 keyPressed = 0;
	//wait until a key is pressed then return pressed key
	while( !( keyPressed = keypad_getKey() ) );
	return keyPressed;
}