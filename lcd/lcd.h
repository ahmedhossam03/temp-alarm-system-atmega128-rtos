#ifndef LCD_H_
#define LCD_H_

#ifndef	F_CPU
	#define F_CPU 8000000UL
#endif

#include "../utils/data_types.h"
#include "../utils/bit_handle.h"
#include "util/delay.h"

#define LCD_DATA_PORT	(*(volatile uint8*) 0x35)
#define LCD_DATA_DIRC	(*(volatile uint8*) 0x34)
#define LCD_CTRL_PORT	(*(volatile uint8*) 0x38)
#define LCD_CTRL_DIRC	(*(volatile uint8*) 0x37)

#define LCD_RS			5
#define LCD_RW			6
#define LCD_EN			7

#define LCD_CLR			0x01
#define LCD_ENTRY		0x06			//increment and no shifting
#define LCD_DISP		0x0E			//display on, cursor on and blinking off
#define LCD_FUNC		0x38			//8bit, 2 lines and 5x8

void lcd_init();
void lcd_send_cmd(uint8);
void lcd_send_char(uint8);
void lcd_send_str(uint8[], uint8);
void lcd_send_itoa(uint8);
void lcd_goto_xy(uint8, uint8);

#endif /* LCD_H_ */