/*
 * timer.c
 *
 * Created: 06/09/2013 2:57:43 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "base.h"
#include "driver/relay.h"
#include "driver/ctrkey.h"

//#define RATESCAN	0b00000011

volatile static unsigned char tickScan;
volatile static unsigned int timerT0;
volatile static	unsigned char tick1s = 10;
volatile unsigned char pwm_value = 0x3F;

void Timer1Init(void){
	TCCR1B	= _BV(WGM12) | _BV(CS11) | _BV(CS10);	// mode CTC top = OCR1A; prescale 1/64
	OCR1A	= 23040;					// OCR1A = 23040 = 10Hz.
	TIMSK	= _BV(OCIE1A);				// Enable Interrupt compare A reg match.
}

void setTimerT0( unsigned int Ptime){
	timerT0  = Ptime;
}

unsigned char checkTimerT0 (void){
	return (!timerT0);
}

void delayScanKey(unsigned char	_tick){
	tickScan = _tick;
}

ISR (TIMER1_COMPA_vect){
	if(timerT0) timerT0--;
	if(tick1s) tick1s--;
	if(tickScan == 0){
		scanKey();
	}else{
		tickScan--;
	}
	// Start the AD conversion
	ADCSRA|=_BV(ADSC);
}
void Timer0Init(void){					// init PWM out for charger NiMH battery ( DS1307 )	
	
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 14,400 kHz
	// Mode: Fast PWM top=0xFF
	// OC0 output: Inverted PWM
	ASSR=0x00;
	TCCR0=0x6F;
	TCNT0=0x00;
	OCR0= pwm_value;
}

void changePWMT0(unsigned char PWMvl){
	OCR0 = PWMvl;
	//pwm_value = PWMvl;
}

unsigned char checkTick1s(void){
	if(tick1s){
		return 0;
	}
	else{
		tick1s = 10;
		return 1;
	}	
}