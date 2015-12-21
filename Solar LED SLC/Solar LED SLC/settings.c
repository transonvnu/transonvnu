/*
 * settings.c
 *
 * Created: 14/11/2013 11:12:56 SA
 *  Author: SCEC
 */ 

#include <avr/eeprom.h>
#include "driver/ds1307.h"
#include "settings.h"
#include "screen.h"
/*
void writeEEPROM(char* data,unsigned int startPos,unsigned int endPos){
	unsigned int i;
	//cli();            // Global disable interrupts
	for (i = startPos; i <= endPos; i++){
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)i, data[i-startPos]);//Nguyen ban: data[i-startPos]
	}
	//sei();					 //  Global enable interrupts
}
void readEEPROM(char* data,unsigned int startPos,unsigned int endPos){
	unsigned int i;
	//cli();            // Global disable interrupts
	for (i = startPos; i <= endPos; i++){
		eeprom_busy_wait();
		data[i-startPos] = eeprom_read_byte((uint8_t *)i);
	}
	//sei();					 //  Global enable interrupts
}*/
void countWriteEEMEMcycle(void){
	unsigned int _valtemp;
	Read_NVRAM(&_valtemp,COUNT_WR_EEMEM_ADDR,2);
	_valtemp++;
	Write_NVRAM(&_valtemp,COUNT_WR_EEMEM_ADDR,2);
}