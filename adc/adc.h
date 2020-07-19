#ifndef ADC_H_
#define ADC_H_

#include "../utils/bit_handle.h"
#include "../utils/data_types.h"

#define ADMUX_R (*(volatile uint8*) 0x27)
#define ADCS_R	(*(volatile uint8*) 0x26)
#define ADCH_R	(*(volatile uint8*) 0x25)
#define ADCL_R	(*(volatile uint8*) 0x24)

void adc_init();
uint16 adc_get_data(uint8);

#endif /* ADC_H_ */