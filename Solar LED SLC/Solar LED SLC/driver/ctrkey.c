/*
 * ctrkey.c
 *
 * Created: 06/09/2013 11:18:28 SA
 *  Author: SCEC
 */ 

#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>

#include "../timer.h"
#include "ctrkey.h"
#include "../base.h"

//static unsigned char screen;
volatile static unsigned char pressButton;	// 1 = pressed; 0 = relessed
volatile static char keyPressed;
static volatile struct _BUTTON_MD ButtonSts = {
	0,
	0
};


void initKeyboard(void){
	sbi(KEY_DDR,_KEY_CLK);
	sbi(KEY_DDR,_KEY_RST);	//DDRB |= 0xC0;//PB6 clk, PB7 reset CD4017
	cbi(KEY_DDR,_KEY_INP);//DDRB &= 0xDF;//PB5    in
	//PORTB |= 0x20;
	//DDRC |= 0x80;//LCD backLight 
	//screen = 0;
	//pressButton = 0;
	//keyPressed = 0;
	//currentMenu=0;
	//buttonCmd = 255;
	//resetEnergy = -1;
	//loadDefaul = -1;
	//PHCAL1 = 60;
	//PHCAL2 = 60;
	//PHCAL3 = 60;
	//checkConnection = -1;
	//dimmImmediately = -1;
	//changeTimePoiter=0;
	//changeSchedulePoiter=0;
	//changeTiPoiter = 0;
	//configPoiter = 0;
	//passSecurity = 0;
	//countPassWord = 0;
	//dimPoiter=0;
	//E1=0;E2=0;E3=0;A1=0;A2=0;A3=0;
}

void scanKey(void){
	unsigned char scan;	//,Up=0,Down=0,Right=0,Left=0,Enter=0,Escap=0;
	//scan keyboard
	sbi(KEY_PORT,_KEY_RST);		//PORTB |= 0x80;//reset 4017
	_delay_us(20);
	cbi(KEY_PORT,_KEY_RST);		//PORTB &= 0x7F;
	_delay_us(10);
	for(scan=0;scan<8;scan++){
		if(KEY_PIN&(_BV(_KEY_INP))){
			//_delay_ms(150);
			if(scan == 0){
				//Down = 1;
				ButtonSts.ButtonNum = DOWN_PRESSED;
				ButtonSts.Pressed = 1;
				break;
			}
			if(scan == 1){
				//Escap = 1;
				ButtonSts.ButtonNum = ESC_PRESSED;
				delayScanKey(5);
				ButtonSts.Pressed = 1;
				break;
			}
			if(scan == 2){
				//Right = 1;
				ButtonSts.ButtonNum = RIGHT_PRESSED;
				ButtonSts.Pressed = 1;
				break;
			}
			if(scan == 3){
				//Up = 1;
				ButtonSts.ButtonNum = UP_PRESSED;
				ButtonSts.Pressed = 1;
				break;
			}
			if(scan == 4){
				//Left = 1;
				ButtonSts.ButtonNum = LEFT_PRESSED;
				ButtonSts.Pressed = 1;
				break;
			}
			if(scan == 5){
				//Enter = 1;
				ButtonSts.ButtonNum = OK_PRESSED;
				delayScanKey(5);
				ButtonSts.Pressed = 1;
				break;
			}
	    }
		sbi(KEY_PORT,_KEY_CLK);	//PORTB |= 0x40;//clock 4017
		_delay_us(10);
		cbi(KEY_PORT,_KEY_CLK);	//PORTB &= 0xBF;
		_delay_us(10);
	}
	/*
	if(Left){
		key[0] = 1;
	    pressButton = 1;		
	}
	if(Right){
		key[1] = 1;
	    pressButton = 1;			
	}
	if(Up){
		key[2] = 1;
		pressButton = 1;		
	}
	if(Down){
		key[3] = 1;
		pressButton = 1; 		
	}	
	if(Enter){
		key[4] = 1;
		pressButton = 1;
	}*/
}//End scanKey

unsigned char checkBtPressed(void){
	if(ButtonSts.Pressed){
		ButtonSts.Pressed = 0;
		return 1;
	}else return 0;
}
unsigned char getButton(void){
	return ButtonSts.ButtonNum;
}