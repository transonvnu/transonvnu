/*
 * relay.c
 *
 * Created: 19/07/2013 5:13:54 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include "../base.h"
#include "../main.h"
#include "relay.h"

volatile unsigned char lampState = 0,lampOldState = 0xFF;
unsigned char psu_status = PSU_OFF;
unsigned char sel_source = SEL_BATTERY;


void init_sel_source_pw(){
	// init by selecting Pb BATTERY
	cbi(PORT_RL_SEL_SOURCE,RL_SEL_SOURCE);
	sbi(DDR_RL_SEL_SOURCE, RL_SEL_SOURCE);
	return;
}
void sel_source_pw(unsigned	char Pwtype){
	switch(Pwtype){
		case SEL_BATTERY:	// Switch to use Pb Battery
		cbi(PORT_RL_SEL_SOURCE, RL_SEL_SOURCE);
		sbi(DDR_RL_SEL_SOURCE, RL_SEL_SOURCE);
		sel_source = SEL_BATTERY;
		break;
		case SEL_MAIN: 
		sbi(PORT_RL_SEL_SOURCE, RL_SEL_SOURCE);
		sbi(DDR_RL_SEL_SOURCE, RL_SEL_SOURCE);
		sel_source = SEL_MAIN;
		break;
	}
	return;
}

void init_enb_psu(){
	// When init, 600W PSU off.
	cbi(PORT_ENB_PSU,RL_ENB_PSU);
	sbi(DDR_ENB_PSU,RL_ENB_PSU);
	return;
}

void sw_psu(unsigned char sts_psu){
	if(sts_psu){	// PSU = ON
		sbi(PORT_ENB_PSU,RL_ENB_PSU);
		sbi(DDR_ENB_PSU,RL_ENB_PSU);
		psu_status = PSU_ON;
	}else{			// PSU = OFF
		psu_status = PSU_OFF;
		cbi(PORT_ENB_PSU,RL_ENB_PSU);
		sbi(DDR_ENB_PSU,RL_ENB_PSU);
	}
	return;
}

void changeLampState(unsigned char	_state){
	switch(_state)
	{
		case 50:
		case 60:
		case 70:
		case 80:
		case 90:
		case 100:
		case 55:
		case 65:
		case 75:
		case 85:
		case 95:
		case 255:
			lampState = _state;
			break;
		default:
			lampState = 0;
			break;
	}
}
unsigned char getLampState(){
	return lampState;
}

void taskManageLampState(void){
	if(lampOldState != lampState){
		switch_Lamp(lampState);
		lampOldState = lampState;
	}else{
		if(getChangedSupplySrc()){
			switch_Lamp(lampState);
		}
	}
}

void init_sw_lamp(void){
	cbi(PORT_SW_LAMP,RL_SW_LAMP);
	sbi(DDR_SW_LAMP,RL_SW_LAMP);	
}

void switch_Lamp(unsigned char _val){
	if(_val >= LAMP_ON)
	{
		switch(_val)
		{
			case 50:
			case 60:
			case 70:
			case 80:
			case 90:
			case 100:
			case 55:
			case 65:
			case 75:
			case 85:
			case 95:
			case 255:
				sbi(PORT_SW_LAMP,RL_SW_LAMP);
				switch(getSupplySrc()){
					case USE_BAT:			// 100% USE BATTERY
						sw_psu(PSU_OFF);
						sel_source_pw(SEL_BATTERY);
						break;
					case USE_MAIN:			// 100% USE MAIN
						sw_psu(PSU_ON);
						sel_source_pw(SEL_MAIN);
						break;
				}
				return;
				break;
			default:
				break;
		}
	}
	cbi(PORT_SW_LAMP,RL_SW_LAMP);
	sw_psu(PSU_OFF);
	sel_source_pw(SEL_MAIN);
}