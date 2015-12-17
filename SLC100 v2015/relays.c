/*
relays.c
Implementation of relays control
Date created: 23/07/2008
*/
#include <avr/io.h>
#include "relays.h"
#include "MBAT128.h"

#define BIT(x) (1<<x)

static unsigned char lightStatus=0;
static unsigned char DecoratelightStatus=0;
char realTimeError;
char scheduleError;
char schedule1Error;

void initRelays(void){		   // turn off all relays	 
	DDRE |= 0xF8;			   // PE3..7
	PORTE &= 0x07;             // all relays off
	//modem_on_off(0);
	acquy_on_off(0);
	phase1_on_off(0);
	phase2_on_off(0); 
	//relay2(0);
}	
//------------------------------------------------
void changeLightStatus(unsigned char noOfPhaseOn){
	if((realTimeError < 5)&&(scheduleError < 5)){
		if (noOfPhaseOn == 1){      // On 30%
			phase1_on_off(1);  
			lightStatus = 1;		
			phase2_on_off(0); 
			//dimOneNode(255,50);		
		}
		else if (noOfPhaseOn == 2){ // On 70%
			phase1_on_off(0);
			lightStatus = 2;		
			phase2_on_off(1);		
		}
		else if (noOfPhaseOn == 3){ // On 100%
			phase1_on_off(1);
			lightStatus = 3;
			phase2_on_off(1);
			//dimOneNode(255,100);		
		}
		else {						// OFF
			phase1_on_off(0);
			lightStatus = 0;
			phase2_on_off(0);
			//dimOneNode(255,0);

		}
	}else{
		phase1_on_off(0);
		phase2_on_off(0);
		phase3_on_off(0);
		lightStatus = 0;
	}
}	
void changeDecorateLightStatus(unsigned char state){
	if((realTimeError < 5)&&(schedule1Error < 5)){
		if(state){
			phase3_on_off(1);
			DecoratelightStatus = 1;
		}else{
			phase3_on_off(0);
			DecoratelightStatus = 0;
		}
	}else{
		phase3_on_off(0);
		DecoratelightStatus = 0;
	}
}
void modem_on_off(unsigned char state){      //Relay for MODEM
        //DDRE |= BIT(7);
        if(state) PORTE &= (~BIT(7));
        else      PORTE |= BIT(7);
}
void relay2(unsigned char state){            //Du tru
        //DDRE |= BIT(3);
        if(state) PORTE |= BIT(3);
        else      PORTE &= (~BIT(3));
}
void acquy_on_off(unsigned char state){      //Relay for Ac quy
        //DDRE |= BIT(6);
        if(state) PORTE |= BIT(6);
        else      PORTE &= (~BIT(6));
}
void phase1_on_off(unsigned char state){     //Relay for Lamp1
        //DDRE |= BIT(5);    
        if(state) PORTE |= BIT(5);
        else      PORTE &= (~BIT(5));
}
void phase2_on_off(unsigned char state){     //Relay for Lamp2
        //DDRE |= BIT(4);    
        if(state) PORTE |= BIT(4);
        else      PORTE &= (~BIT(4));

}
void phase3_on_off(unsigned char state){     //05/11/2010 Relay for decorate Lamp
        //DDRE |= BIT(4);    
        if(state) PORTE |= BIT(3);
        else      PORTE &= (~BIT(3));

}
//------------------------------------------------
unsigned char getLightStatus(void){
	return lightStatus;
}	
unsigned char getDecorateLightStatus(void){
	return DecoratelightStatus;
}

