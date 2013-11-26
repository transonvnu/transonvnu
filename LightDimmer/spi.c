/*
  CodeVisionAVR C Compiler
  (C) 1998-2005 Pavel Haiduc, HP InfoTech S.R.L.

  SPI access function
*/

#include <avr/io.h>
#include "spi.h"   
#include "timming.h"
 
#ifndef SPIF
#define SPIF 7
#endif 

#define BIT(x) (1<<x)

void initSPI(void){
// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 125.0 kHz
// SPI Clock Phase: Cycle Half
// SPI Clock Polarity: High
// SPI Data Order: MSB First
		//unsigned char temp;
        
		SPCR = 0x57;   
        SPSR = 0x00;
		
		asm("NOP");
		//temp = spi(0x00);          // Wait Ready
		/*resetTime();
		do{
			temp = spi(0x00);   
			asm("NOP");
	
		}while((temp != 0xFF)&&(getTime()<2500));
		 */	
}
unsigned char spi(unsigned char data){
	SPDR = data;
	//delay(100);
	resetTime();
	while ((!(SPSR & (1<<SPIF)))&&(getTime()<3000));
	//while (!(SPSR & (1<<SPIF)));
	return SPDR;
}


