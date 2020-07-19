#include "uart.h"

void uart_init(uint8 uart_sel)
{
	if(uart_sel == 0)
	{
		//enable receiver interrupt
		setBit(UART_0_CSRB, 7);
		//enable transmitter and receiver
		setBit(UART_0_CSRB, 4);
		setBit(UART_0_CSRB, 3);
		//asynchronous mode
		clearBit(UART_0_CSRC, 6);
		//disabled parity
		clearBit(UART_0_CSRC, 5);
		clearBit(UART_0_CSRC, 4);
		//one stop bit
		clearBit(UART_0_CSRC, 3);
		//8 bit data mode
		setBit(UART_0_CSRC, 2);
		setBit(UART_0_CSRC, 1);
		//set baud rate to 9600
		uint16 ubrr = 51;
		UART_0_BRL = (uint8)ubrr;
		UART_0_BRH = 0;
	}
	else if(uart_sel == 1)
	{
		//enable receiver interrupt
		setBit(UART_1_CSRB, 7);
		//enable transmitter and receiver
		setBit(UART_1_CSRB, 4);
		setBit(UART_1_CSRB, 3);
		//asynchronous mode
		clearBit(UART_1_CSRC, 6);
		//disabled parity
		clearBit(UART_1_CSRC, 5);
		clearBit(UART_1_CSRC, 4);
		//one stop bit
		clearBit(UART_1_CSRC, 3);
		//8 bit data mode
		clearBit(UART_1_CSRC, 2);
		clearBit(UART_1_CSRC, 1);
		//set baud rate to 9600
		uint16 ubrr = 51;
		UART_1_BRL = (uint8)ubrr;
		UART_1_BRH = (uint8)(ubrr>>8);
	}
}

void uart_send_byte(uint8 uart_sel, uint8 data_byte)
{
	if(uart_sel == 0)
	{
		while((readBit(UART_0_CSRA, 5)) != 1);
		UART_0_BUFFER = data_byte;
	}
	else if(uart_sel == 1)
	{
		while((readBit(UART_1_CSRA, 5)) != 1);
		UART_1_BUFFER = data_byte;
	}
}

//static to avoid being used outside this file and volatile so the compiler doesn't optmize it
static volatile uint8 uart_0_dataReturned = 0;
static volatile uint8 uart_1_dataReturned = 0;
uint8 uart_rece_byte(uint8 uart_sel)
{
	uint8 data_byte = 0;
	if(uart_sel == 0)
	{
		data_byte = uart_0_dataReturned;
		uart_0_dataReturned = 0;
	}
	else if(uart_sel == 1)
	{
		data_byte = uart_1_dataReturned;
		uart_1_dataReturned = 0;
	}
	return data_byte;
}

uint8 uart_rece_byte_blocking(uint8 uart_sel)
{
	uint8 data = 0;
	if(uart_sel == 0)
	{
		//disable interrupt mode
		clearBit(UART_0_CSRB, 7);
		//receive data
		while((readBit(UART_0_CSRA,7)) == 0 && (readBit(UART_0_CSRA,5)) == 1);
		//return buffer
		data = UART_0_BUFFER;
		//enable interrupt again
		setBit(UART_0_CSRB, 7);
	}
	else if(uart_sel == 1)
	{
		//disable interrupt mode
		clearBit(UART_1_CSRB, 7);
		//receive data
		while((readBit(UART_1_CSRA,7)) == 0 && (readBit(UART_1_CSRA,5)) == 1);
		//return buffer
		data = UART_1_BUFFER;
		//enable interrupt again
		setBit(UART_1_CSRB, 7);
	}
	return data;
}

void uart_send_str(uint8 uart_selc, uint8 str[], uint8 str_size)
{
	uint8 i = 0;
	while(i < str_size)
	{
		uart_send_byte(uart_selc, str[i]);
		i++;
	}
}


//uart 0 and uart 1 ISR vectors
void __vector_18(void)__attribute__((signal,used));
void __vector_18(void)
{
	uart_0_dataReturned = UART_0_BUFFER;
}

void __vector_30(void)__attribute__((signal,used));
void __vector_30(void)
{
	uart_1_dataReturned = UART_1_BUFFER;
}