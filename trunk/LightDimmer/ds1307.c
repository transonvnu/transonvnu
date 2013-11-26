/*
  CodeVisionAVR C Compiler
  (C) 1998-2005 Pavel Haiduc, HP InfoTech S.R.L.

  Dallas Semiconductors DS1307 I2C Bus Real Time Clock functions
*/

#include <avr/eeprom.h>
#include "manual_i2c.h" 
#include "ds1307.h"
#include "schedule.h"

char realTimeError = 0;

void rtc_init(unsigned char rs,unsigned char sqwe,unsigned char out){
        rs &= 3;
        if (sqwe) rs|=0x10;
        if (out) rs|=0x80;
        i2cStart();
        i2cWrite(0xd0);
        i2cWrite(7);
        i2cWrite(rs);
        i2cStop();
}

unsigned char readRealTime(TimeStruct* time){      
        char temp,tempSecond;
		TimeStruct ttime;
        i2cStart();
        i2cWrite(0xd0);
        i2cWrite(0);
        i2cStart();
        i2cWrite(0xd1);  
        temp = i2cRead(1);			//seconds
		ttime.second = (temp >> 4) * 10 + (temp & 0x0f);
		tempSecond = (*time).second;
		temp = i2cRead(1);			//minutes
		ttime.minute = (temp >> 4) * 10 + (temp & 0x0f);
		temp = i2cRead(1);			//hours
		ttime.hour = (temp >> 4) * 10 + (temp & 0x0f);
		temp = i2cRead(1);			//day	
		ttime.day = (temp >> 4) * 10 + (temp & 0x0f);
		temp = i2cRead(1);			//date
		ttime.date = (temp >> 4) * 10 + (temp & 0x0f);
		temp = i2cRead(1);			//month
		ttime.month = (temp >> 4) * 10 + (temp & 0x0f);
		temp = i2cRead(0);			//year
		ttime.year = (temp >> 4) * 10 + (temp & 0x0f);
        i2cStop();
		if(checkTime(&ttime)){ //Add 13/08/2010
			(*time) = ttime;
			return 1;
		}else{
			if(realTimeError < 1){
				realTimeError++;
			}
			if(realTimeError >= 1){
				rtc_init(0,1,0);
				ttime.second = 0; // Reset Real Time
				ttime.minute = 0;
				ttime.hour = 18;
				ttime.day = 1;
				ttime.date = 1;
				ttime.month = 1;
				ttime.year  = 11;  // Thu 2/ 0 gio/0 phut /1/1/2001
				writeRealTime(&ttime);
				setSafelySchedule();
			}
			return 0;
		}
}
unsigned char writeRealTime(TimeStruct* time){
        char temp;
		if(checkTime(time)){
	        i2cStart();
	        i2cWrite(0xd0);
	        i2cWrite(0);
	        temp = (((*time).second / 10) << 4) + ((*time).second % 10);			//seconds
			i2cWrite(temp);
			temp = (((*time).minute / 10) << 4) + ((*time).minute % 10);			//minutes
			i2cWrite(temp);
			temp = (((*time).hour / 10) << 4) + ((*time).hour % 10);				//hour
			i2cWrite(temp);
			temp = (((*time).day / 10) << 4) + ((*time).day % 10);
			i2cWrite(temp);															//day of week
			temp = (((*time).date / 10) << 4) + ((*time).date % 10);				//date
			i2cWrite(temp);	
			temp = (((*time).month / 10) << 4) + ((*time).month % 10);				//month
			i2cWrite(temp);
			temp = (((*time).year / 10) << 4) + ((*time).year % 10);				//year
			i2cWrite(temp);
	        i2cStop();
			return 1;
		}else{
			return 0;
		}
}
void writeNVRAM(char* data,unsigned char startPos,unsigned char endPos){
        unsigned char i;
        i2cStart();
        i2cWrite(0xd0);
        i2cWrite(0x08 + startPos);
        for (i = 0; i<= endPos - startPos; i++){
			i2cWrite(data[i]);  
		}	
        i2cStop();
}
void readNVRAM(char* data,unsigned char startPos,unsigned char endPos){ 
        unsigned char i;
        i2cStart();
        i2cWrite(0xd0);
        i2cWrite(0x08 + startPos);
        i2cStart();
        i2cWrite(0xd1); 
        for (i = 0; i < endPos - startPos; i++){
			data[i] = i2cRead(1);
		}
		data[endPos - startPos] = i2cRead(0);
        i2cStop();   
} 
void writeEEPROM(char* data,unsigned int startPos,unsigned int endPos){
        unsigned int i;
        for (i = startPos; i <= endPos; i++){
			eeprom_busy_wait();
			eeprom_write_byte((uint8_t *)i, data[i-startPos]);//Nguyen ban: data[i-startPos]			
		}	
} 
void readEEPROM(char* data,unsigned int startPos,unsigned int endPos){ 
        unsigned int i;
        for (i = startPos; i <= endPos; i++){
			eeprom_busy_wait();
			data[i-startPos] = eeprom_read_byte((uint8_t *)i);
		}
}  

unsigned char checkTime(TimeStruct* time){
	if(((*time).second < 0)||((*time).second > 59)){
		return 0;
	}else{
		if(((*time).minute < 0)||((*time).minute > 59)){
			return 0;
		}else{
			if(((*time).hour < 0)||((*time).hour > 23)){
				return 0;
			}else{
				//if(((*time).day < 0)||((*time).day > 7)){
				//	return 0;
				//}else{
					if(((*time).date < 1)||((*time).date > 31)){
						return 0;
					}else{
						if(((*time).month < 1)||((*time).month > 12)){
							return 0;
						}
					}
				//}
			}
		}
	}
	return 1;	
}





