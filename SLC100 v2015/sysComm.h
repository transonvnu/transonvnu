/*
sysComm.h
Library for interfacing with server and hid terminal
Date created: 02/15/2007

*/

#ifndef _SYSCOMM_INCLUDED_
#define _SYSCOMM_INCLUDED_

#include "ds1307.h"
#include "ade7758.h"

//consts
#define COMM_TIMEOUT	500		//milisec, total time on once read command  
#define COMM_TIMEOUT2	2000		//milisec, total time on once read command  

void externalMemoryInit(void);
//extern unsigned int Seconds;
// External Interrupt 0 service routine
void interrupt_init(void); 
//void system_init(void);
//check link status
void initModemWatchDog(void);
void resetModemWatchDog(void);
void resetModem(void);
unsigned char isModemTimeout(void);
unsigned char getModemConnected(void);
void setModemTimeout(unsigned int time);
unsigned int getModemTime(void);//SC

//read command from HID or server
unsigned char readCommand(char* args, char * cmd);
unsigned char readCommand1(char* args);
//string buider functions
void prepareLightStatus(char lightStatus, char* buffer);
//void prepareScheduleStatus(unsigned char scheduleStatus[5][5], unsigned char* buffer);
void prepareScheduleStatus(signed char scheduleStatus[5][5],unsigned int SendTime[5], char* buffer);//Modified 04/03/2009
void prepareSchedule1Status(char * str,char* buffer);
void prepareRealtimeStatus(TimeStruct* timeStatus, char* buffer);
//void prepareUIThresholdStatus(int UThreshold, int IThreshold, char* buffer);
void prepareTransducerStatus(unsigned int* voltage, unsigned int* current, unsigned char* phase, EnergySruct energy, unsigned int * voltageDim, char* buffer);
//void prepareTimeoutStatus(unsigned int modemT, unsigned int logT, char* buffer); 
void feedBackCmd(char * cmd,char * Buffer);
void led1(unsigned char state);         //Control LED1
void led2(unsigned char state);         //Control LED2  
void back_ligh(unsigned char state);    //Back_ligh LCD 
unsigned char isEnergyTime(void);
unsigned char isSdCardTime(void);
void writeLogTime(unsigned int time);
void setLogTime(unsigned int time);
unsigned char isNextSecond(void); 
unsigned char isNextSecond2(void); 
unsigned char isSendStateTime(void);
void setSendStateTime(unsigned int time);
//unsigned char isLowBattery(void);
unsigned char checkBattery(unsigned char status);
void chargBattery(unsigned char status);
unsigned char getBackLightTime(void);
void resetBackLightTime(void);
unsigned char getStateEightLamps(void);
unsigned char isRealtimeFailure(void);
void send_SMS(char * number,char * str);
//Echelon 19_04_2010
void Addnode(char* data);
void Delnode(char* data);
void AddSchedule(char* data);

#endif 
