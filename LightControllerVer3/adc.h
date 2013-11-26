#ifndef _ADC_INCLUDE_
#define _ADC_INCLUDE_

#define ADC_VREF_TYPE 0x40
void initADC(void);
unsigned int read_adc(unsigned char adc_input);
//#pragma library ADC.c
#endif
