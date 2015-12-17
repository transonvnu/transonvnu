/*
schedule.h
Library for executing tasks
Date created: 01/29/2008
*/

#ifndef _SCHEDULE_INCLUDED_
#define _SCHEDULE_INCLUDED_     


#include "ds1307.h"
void initSchedule();
void initDecorateSchedule(void);//Decorate Light, Add 24/11/2010
void initDimmingPara(void);
//void setSchedule(unsigned char tasks[5][5]);
unsigned char setSchedule(signed char tasks[5][5],unsigned int sendTime[5]);//added 4/3/2009
unsigned char setSchedule1(signed char tasks[14][3]);//Decorate Light, Add 24/11/2010
void setDimmingPara(char * str);
unsigned char isDimmingTime(unsigned char phase,TimeStruct * Time);
//void getSchedule(unsigned char tasks[5][5]);
void getSchedule(signed char tasks[5][5],unsigned int SendTime[5]);
void getSchedule1(signed char tasks[14][3]);//07/12/2010
unsigned char isSchedule(TimeStruct * Time); //return 0 if no task, at current task 
//unsigned char isSchedule1(TimeStruct * Time);//Decorate Light, Add 24/11/2010
unsigned char isInTimeSlot(TimeStruct* a, TimeStruct* b, TimeStruct* c);
unsigned char checkCurrentTask();	        //no of phase on
unsigned char getCurrentTask();		        //check task no 
unsigned char getCurrentTask1();
void setCurrentTask(unsigned char task);        //set task in case change time
unsigned int numOfminute(TimeStruct* time);
void jumpNextTask(void);  
unsigned char checkNextTask1(unsigned char task);
//void jumpNextTask1(void);//Decorate Light, Add 24/11/2010
void jumpNextDimmTask(unsigned char phase);
void dimmingVoltage(unsigned char phase);
void findCurrentTask1(TimeStruct* time);//Decorate Light, Add 24/11/2010
unsigned char checkCurrentTask1();//Decorate Light, Add 24/11/2010
unsigned char checkSchedule(unsigned char tTable[5][5]);//checkSchedule 13/06/2011
unsigned char checkSchedule1(unsigned char tTable1[14][3]);

#endif
 
