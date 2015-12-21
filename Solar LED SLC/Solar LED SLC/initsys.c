/*
 * initsys.c
 *
 * Created: 19/07/2013 5:30:37 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "initsys.h"
#include "driver/relay.h"
#include "driver/ks0108.h"
#include "driver/uart.h"
#include "driver/ds1307.h"
#include "driver/ctrkey.h"
#include "driver/ADC.h"
#include "timer.h"
/*
void externalMemoryInit(void){
	DDRC = 0xFF;
	PORTC = 0x00;
	MCUCR = _BV(SRW10) | _BV(SRE);   // External SRAM/XMEM Enable
	XMCRA = _BV(SRW11);  			// Wait one cycle during read/write strobe
	XMCRB = _BV(XMBK) | _BV(XMM0);	//	0x81;  				// 32kB released PC7 
}*/
void pwon_init(){
	//externalMemoryInit();
	wdt_enable(WDTO_2S);
	cli();
	Timer1Init();
	Timer0Init();	// NiMH Charger PWM
	initKeyboard();
	init_sel_source_pw();
	init_enb_psu();
	init_sw_lamp();
	TWIInit();
	ExtINT4Init();
	initUART();
	ADCInit();
	InitialLcd();
	sei();			// global interrupt enable
	wdt_reset();
}
