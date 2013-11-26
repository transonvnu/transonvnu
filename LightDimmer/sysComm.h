/*
sysComm.h
Library for interfacing with server and hid terminal
Date created: 02/15/2007

*/

#ifndef _SYSCOMM_INCLUDED_
#define _SYSCOMM_INCLUDED_

#include "ds1307.h"
#include "ade7753.h"
//consts
#define COMM_TIMEOUT	40000		//~ 800 milisec, total time on once read command  
// External Interrupt 0 service routine
void interrupt_init(void);
//check link status
//read command from HID or server
unsigned char readCommand(char* args);
//string buider functions
void feedBackCmd(char * cmd,char * Buffer);
unsigned char isEnergyTime(void);
unsigned char isNextSecond(void); 
//unsigned char isSendStateTime(void);
//void setSendStateTime(unsigned int time);
unsigned char isRealtimeFailure(void);


#endif 
