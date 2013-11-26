/*
	NVRAM memory map:
	Total  56 Bytes
		
	0 --> 3 : Active Energy 

	EEPROM memory map

	0  --> 44 : Schedule
	45 --> 62 : 9 word send state time

	63 --> 66 : E Coeff Calibration
	67 --> 68 : U Coeff Calibration
	69 --> 70 : I Coeff	Calibration
	
*/

#ifndef _DS1307_INCLUDED_
#define _DS1307_INCLUDED_

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

#endif

