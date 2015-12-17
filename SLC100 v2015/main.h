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
extern uint8_t scheduleTasks[5][5] EEMEM;


#endif /* MAIN_H_ */