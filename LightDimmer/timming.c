/*
timming.c
Implement timming functions
Date created: 04/19/2007
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "timming.h"
#include "sysComm.h"

volatile unsigned int tick=0,count = 0;
extern char open;
extern int cutphase,DIM;
volatile unsigned char BTAisON = 0;
/*
void delay(long time){
	while(time--){
		asm("NOP");
	}
}*/

void initTimer0(){       //Used Timer0		
	
	//ASSR  = 0x00;
	TCCR0 = 0x01;			//Start Timer0, Fclk = Fxtal/1 = 8000.00 kHz
	TCNT0 = 190;
	TIMSK |= 0x01;           //Timer0 overflow interrupt enable  
	resetTime();

}
/*
void initTimer2(void){
	// Timer 2
	TCCR2=0x09;
	TCNT2=0x00;
	OCR2=0x08;
	TIMSK |= 0x80;     //Timer 2 output compare  match interrupt enable  
}
//---------------------------
void delayMs(unsigned int ms){

	tick1 = 0;
	while(tick1 < ms);

}
void delayUs(unsigned int us){

	tick2 = 0;
	while(tick2 < us);

}*/
//---------------------------
unsigned int getTime(){
	return tick;
}
//---------------------------
void resetTime(){
	tick = 0;
}
void forceBTAoff(void){
	count = 0;
	PORTB &= 0xBF;// BTA OFF
	BTAisON = 0;
	open = 0;
}
//--------------------------
ISR(TIMER0_OVF_vect){  // 8.125 us, actual ~ 23.63 us

	//TCNT0= 200;
    tick++;  //for getTime()
	
	if(open == 1){
		count++;
		if(BTAisON == 0){	
			if(count >= cutphase){
				PORTB |= 0x40;   // BTA ON
				BTAisON = 1;
				count = 0;
			}
		}else{
			if(count >= 10){
				count = 0;
				PORTB &= 0xBF;// BTA OFF
				BTAisON = 0;
				open = 0;
			}
		}
	}
	TCNT0= 190;
}
/*
ISR(TIMER2_COMP_vect){ // 1us
	tick2++;
} */

