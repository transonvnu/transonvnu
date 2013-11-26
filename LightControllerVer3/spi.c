/*
  CodeVisionAVR C Compiler
  (C) 1998-2005 Pavel Haiduc, HP InfoTech S.R.L.

  SPI access function
*/

#include <avr/io.h>
#include "spi.h"   
#include "sdcard.h"      
#include "timming.h"  
#ifndef SPIF
#define SPIF 7
#endif 

#define BIT(x) (1<<x)

void initADE(void){
// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 230.400 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: Low
// SPI Data Order: MSB First
//		unsigned char temp;
		deselectSdcard();
        DDRB |= 0x17;      //0b xxx10111
        SPCR=0x56;  //0101 0110: fclk = Fcrystal/64
        SPSR=0x00;
		delay(5);
		PORTB |= 0x01;//ADE Chip select
		delay(5);
		/*
		temp = spi(0x00);         //Wait Ready
		resetTime();
		do{
			temp = spi(0x00);   
		}while((temp != 0xFF)&&(getTime()<500));*/
		 		
}
void deselectADE(void){
	PORTB &= 0xFE;       //ADE Deselect
}

//04/05/2009 add 50ms timeout for spi transceiver
char spi(char data){
	//Delay(20);
	SPDR=data;
	resetTime();
	//while ((SPSR & (1<<SPIF))==0)
	do{
	
	}while (((SPSR & (1<<SPIF))==0)&&(getTime()<50));
	return SPDR;
}


