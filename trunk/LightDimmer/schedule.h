/*
schedule.h
Library for executing tasks
Date created: 01/29/2008
*/

#ifndef _SCHEDULE_INCLUDED_
#define _SCHEDULE_INCLUDED_     

#include "ds1307.h"
void initSchedule();
unsigned char setSchedule(signed char tasks[9][5],unsigned int sendTime[9]);//added 4/3/2009
void setSafelySchedule(void);
void getSchedule(signed char tasks[9][5],unsigned int SendTime[9]);
unsigned char isSchedule(TimeStruct * Time); //return 0 if no task, at current task 
unsigned char isInTimeSlot(TimeStruct* a, TimeStruct* b, TimeStruct* c);
unsigned char checkCurrentTask();	        //no of phase on
//unsigned char getCurrentTask();		        //check task no 
void setCurrentTask(unsigned char task);        //set task in case change time
void jumpNextTask(void);  
unsigned char checkSchedule(unsigned char tTable[9][5]);//checkSchedule 13/06/2011

#endif
 
