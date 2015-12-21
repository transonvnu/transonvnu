/*
 * ds1307.h
 *
 * Created: 09/09/2013 12:20:38 CH
 *  Author: SCEC
 */ 


#ifndef DS1307_H_
#define DS1307_H_

#define BAT_ENERGY_ADDR		15

struct _RTCDATA {
	unsigned int second;
	unsigned int minute;
	unsigned int hour;
	unsigned int day;
	unsigned int date;
	unsigned int month;
	unsigned int year;
};

void ExtINT4Init(); // interrupt for 1s SQW out

void TWIInit(void);
void Write_DS1307(struct _RTCDATA * RTCdata);
void Read_DS1307(struct _RTCDATA *);
void Write_NVRAM(char * _data, unsigned char _addr, unsigned char _Nbyt);
void Read_NVRAM(char * _data, unsigned char _addr, unsigned char _Nbyt );

char readByte_NVRAM(unsigned char _addr);
void writeByte_NVRAM(char _data,unsigned char _addr);

unsigned char checkSWQ1s(void);
#endif /* DS1307_H_ */