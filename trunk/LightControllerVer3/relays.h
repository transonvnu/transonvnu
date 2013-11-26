/*
relays.h
Library for control relays
Date created: 01/31/2008
*/

#ifndef _RELAYS_INCLUDED_
#define _RELAYS_INCLUDED_

void initRelays(void);	//turn off all relays
void changeLightStatus(unsigned char noOfPhaseOn);
void changeDecorateLightStatus(unsigned char state);
unsigned char getLightStatus(void);
unsigned char getDecorateLightStatus(void);
void modem_on_off(unsigned char state);      //Relay for MODEM
void relay2(unsigned char state);            //Reserved
void acquy_on_off(unsigned char state);      //Relay for Ac quy
void phase1_on_off(unsigned char state);     //Relay for Lamp1
void phase2_on_off(unsigned char state);     //Relay for Lamp2  
void phase3_on_off(unsigned char state);     //Relay for decorate Lamp

#endif 
