#include <avr/io.h>
#include "ADC.h" 
#include "timming.h" 

void initADC(void){   
// ADC initialization
// ADC Clock frequency: 115.200 kHz
// ADC Voltage Reference: AVCC pin  
        DDRF &= ~(0x02); //ADC0 input
        ADCSRA = 0x87;   //ADC enable,Fclk = XTAL/128,Single Conversion
        ADMUX |= 0xC0;   //Channel 0, Vref = 2.56 V internal reference
        //delayMs(1);       
}
// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input){  
        unsigned int low,hi;  
        ADMUX |= (0xC0 + adc_input);//Channel x, Vref = 2.56 V internal reference   
        ADCSRA |= 0x40;            // Start the AD conversion
        while ((ADCSRA & 0x10)==0);// Wait for the AD conversion to complete
        ADCSRA |= 0x10;               
        low = ADCL;
        hi = ADCH;       
        return (low + ((unsigned int)hi<<8));
}    

