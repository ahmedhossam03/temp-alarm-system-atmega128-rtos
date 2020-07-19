#include "adc.h"

void adc_init()
{
	//result is right adjusted
	clearBit(ADMUX_R, 5);
	//using AVcc
	setBit(ADMUX_R, 6);
	clearBit(ADMUX_R, 7);
	//prescaler to 64
	ADCS_R = 0b110;
	//enable interrupt
	setBit(ADCS_R, 3);
	//off free running mode
	clearBit(ADCS_R, 5);
	//enable adc
	setBit(ADCS_R, 7);
}

static volatile uint16 converted_data = 0;	//static to avoid being used outside this file and volatile so the compiler doesn't optmize it
uint16 adc_get_data(uint8 ch)
{
	uint16 returnedData = 0;
	//get channel from user
	ADMUX_R |= ch;
	//start conversion
	setBit(ADCS_R, 6);
	returnedData = converted_data;
	converted_data = 0;
	return returnedData;
}

//ADC ISR VECTOR
void __vector_21(void)__attribute__((signal,used));
void __vector_21(void)
{
	converted_data = ADCL_R;
	converted_data |= (ADCH_R << 8);
}