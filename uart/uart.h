#ifndef UART_H_
#define UART_H_

#include "../utils/data_types.h"
#include "../utils/bit_handle.h"

#define UART_0_BUFFER		(*(volatile uint8*) 0x2C)
#define UART_0_CSRA			(*(volatile uint8*) 0x2B)
#define UART_0_CSRB			(*(volatile uint8*) 0x2A)
#define UART_0_CSRC			(*(volatile uint8*) 0x95)
#define UART_0_BRL			(*(volatile uint8*) 0x29)
#define UART_0_BRH			(*(volatile uint8*) 0x90)

#define UART_1_BUFFER		(*(volatile uint8*) 0x9C)
#define UART_1_CSRA			(*(volatile uint8*) 0x9B)
#define UART_1_CSRB			(*(volatile uint8*) 0x9A)
#define UART_1_CSRC			(*(volatile uint8*) 0x9D)
#define UART_1_BRL			(*(volatile uint8*) 0x99)
#define UART_1_BRH			(*(volatile uint8*) 0x98)

void uart_init(uint8);
void uart_send_byte(uint8, uint8);
uint8 uart_rece_byte(uint8);
uint8 uart_rece_byte_blocking(uint8);
void uart_send_str(uint8, uint8[], uint8);

#endif /* UART_H_ */