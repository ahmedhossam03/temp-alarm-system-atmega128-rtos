#include "lcd.h"
#include "util/delay.h"

void lcd_init()
{
	//initialize data port as output
	setAllBits(LCD_DATA_DIRC);
	//initialize ctrl pins as output
	setBit(LCD_CTRL_DIRC, LCD_RS);
	setBit(LCD_CTRL_DIRC, LCD_RW);
	setBit(LCD_CTRL_DIRC, LCD_EN);
	//wait 30 ms for LCD power up
	_delay_ms(30);
	//set mode
	lcd_send_cmd(LCD_FUNC);
	_delay_ms(1);
	//set display
	lcd_send_cmd(LCD_DISP);
	_delay_ms(1);
	//set entry mode
	lcd_send_cmd(LCD_ENTRY);
	_delay_ms(1);
	//clear screen
	lcd_send_cmd(LCD_CLR);
	_delay_ms(10);
}

void lcd_send_cmd(uint8 cmd)
{
	//set RS pin to low
	clearBit(LCD_CTRL_PORT, LCD_RS);
	//set RW pin to low
	clearBit(LCD_CTRL_PORT, LCD_RW);
	//put command on data port
	LCD_DATA_PORT = cmd;
	//put high on enable
	setBit(LCD_CTRL_PORT, LCD_EN);
	//_delay_ms(1);
	//put low on enable
	clearBit(LCD_CTRL_PORT, LCD_EN);
	_delay_ms(1);
}

void lcd_send_char(uint8 data)
{
	//set RS pin to high
	setBit(LCD_CTRL_PORT, LCD_RS);
	//set RW pin to low
	clearBit(LCD_CTRL_PORT, LCD_RW);
	//put command on data port
	LCD_DATA_PORT = data;
	//put high on enable
	setBit(LCD_CTRL_PORT, LCD_EN);
	//_delay_us(10);
	//put low on enable
	clearBit(LCD_CTRL_PORT, LCD_EN);
	_delay_ms(1);	
}

void lcd_send_str(uint8 str[], uint8 size)
{
	uint8 i = 0;
	while(i < size)
	{
		lcd_send_char(str[i]);
		i++;
	}
}

void lcd_send_itoa(uint8 num)
{
	uint8 int2char[3] = {'0', '0', '0'};
	uint8 i = 2;
	//convert num into array
	while(num)
	{
		int2char[i] = (num % 10) + '0';
		i--;
		num = num / 10;
	}
	//send converted array
	lcd_send_str(int2char, sizeof(int2char));
}

void lcd_goto_xy(uint8 x, uint8 y)
{
	if(y == 0)
	{
		lcd_send_cmd(0x80 + x);
	}
	else if(y == 1)
	{
		lcd_send_cmd(0xC0 + x);
	}
}