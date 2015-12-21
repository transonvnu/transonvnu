/*
 * main.h
 *
 * Created: 26/01/2015 5:03:39 PM
 *  Author: Administrator
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <compat/deprecated.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/wdt.h>

#define MAX_NODE	99
#define KEY_LEFT	key[0]
#define KEY_RIGHT	key[1]
#define KEY_UP		key[2]
#define KEY_DOWN	key[3]
#define KEY_OK		key[4]
#define KEY_BACK	key[5]

extern char dateCompiler[];
extern uint8_t		scheduleTasks[5][5] EEMEM;
extern uint32_t		phaseActiveEnergy[3] EEMEM;
extern uint16_t	EEMEM	GSMmodemTimeOut ;
extern uint16_t	EEMEM	logDataTime ;
extern uint8_t		reserved[7] EEMEM;
extern uint16_t		parTICoef1 EEMEM,parTICoef2 EEMEM;
extern uint32_t		energyCalPar[3] EEMEM;
extern uint16_t		voltageCalPar[3] EEMEM;
extern uint16_t		ampereCalPar[3] EEMEM;
extern uint16_t	EEMEM	TiCoefPar1 ,EEMEM TiCoefPar2 ;
extern uint16_t		_sendStateTime[5] EEMEM;
extern uint8_t		centerServerPort[5] EEMEM;	// PORT	02013
extern uint8_t		slc100ID[5]	EEMEM;			// ID		000xx
extern uint8_t		serverStaticIP[4][3] EEMEM;	// IP		113.160.251.175
extern uint8_t		slcPhoneNum[11]	EEMEM;		// Phone No 00915888888
extern uint8_t		phaseCalib[3]	EEMEM;		//
extern uint8_t		_Uvolt[3] EEMEM;
extern uint8_t		_h1m1h2m2h3m3[6] EEMEM;
extern uint8_t		decoTasksTable[14][3] EEMEM;
extern uint8_t		nodesScheduleTable[4][27] EEMEM;
extern uint8_t		nodesScheduleEnb[] EEMEM;

extern uint8_t		groupNodeScheduleEnb[];

#endif /* MAIN_H_ */