/*
	NVRAM memory map:
	Total  56 Bytes
		
	0  --> 24 : Reserved
	25 --> 36 : Active Energy
	37 --> 38 : Reserved
	39 --> 40 : Reserved
	41 --> 47 : Reserved
	48 --> 55 : Reserved	  

	EEPROM memory map

	0  --> 24 : Schedule
	25 --> 36 : Reserved
	37 --> 38 : MODEM timeout
	39 --> 40 : Write Log's time
	41 --> 47 : Reserved
	48 --> 71 : E,U,I Coeff Calibration
	72 --> 75 : TI Coef	
	// Added 04/03/2009
	76 --> 77 : send state time 1
	78 --> 79 : send state time 2
	80 --> 81 : send state time 3
	82 --> 83 : send state time 4
	84 --> 85 : send state time 5
	86 --> 107: 22 config bytes
	108 --> 118: 11 sever's numbers
	119 --> 121: 3 bytes PHACALIB
	// Added 30/07/2009
	122 --> 124: 3 Bytes U1,U2,U3
	125 --> 130: 6 Bytes h1,m1,h2,m2,h3,m3
	131 --> 172: 42 Bytes decorate schedule //22/11/2010
*/

#ifndef _DS1307_INCLUDED_
#define _DS1307_INCLUDED_
#include "integer.h"

//extern char realTimeError;

typedef struct {
	signed char year;
	signed char month;
	signed char date;
	signed char day;
	signed char hour;
	signed char minute;
	signed char second;
} TimeStruct; 

void rtc_init(unsigned char rs,unsigned char sqwe,unsigned char out);
unsigned char readRealTime(TimeStruct* time);
unsigned char writeRealTime(TimeStruct* time);
void readNVRAM(char* data,unsigned char startPos,unsigned char endPos);
void writeNVRAM(char* data,unsigned char startPos,unsigned char endPos);
void writeEEPROM(char* data,unsigned int startPos,unsigned int endPos);
void readEEPROM(char* data,unsigned int startPos,unsigned int endPos);
unsigned char checkTime(TimeStruct* time);
DWORD get_fattime (void);

#endif

