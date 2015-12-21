/*
 * ADC.c
 *
 * Created: 10/09/2013 6:42:57 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define FIRST_ADC_INPUT 0
#define LAST_ADC_INPUT 7
unsigned int adc_data[LAST_ADC_INPUT-FIRST_ADC_INPUT+1];
#define ADC_VREF_TYPE 0x00

// ADC interrupt service routine
// with auto input scanning
// start ADC convert in "timer.c" timer 0 interrupt (10ms)
ISR (ADC_vect)
{
static unsigned char input_index=0;
// Read the AD conversion result
adc_data[input_index] = ADC;
// Select next ADC input
if (++input_index > (LAST_ADC_INPUT-FIRST_ADC_INPUT))
   input_index=0;
ADMUX=(FIRST_ADC_INPUT | (ADC_VREF_TYPE & 0xff))+input_index;
// Delay needed for the stabilization of the ADC input voltage
// Start the AD conversion
//ADCSRA|=0x40;
}

void ADCInit(void){
	// ADC initialization
	// ADC Clock frequency: 921,600 kHz
	// ADC Voltage Reference: AVCC pin
	ADMUX=FIRST_ADC_INPUT | (ADC_VREF_TYPE & 0xff);
	ADCSRA=0xCC;
}