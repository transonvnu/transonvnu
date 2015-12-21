/*
 * schedule.h
 *
 * Created: 20/09/2013 3:23:28 CH
 *  Author: SCEC
 */ 


#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "driver/ds1307.h"
#include <avr/eeprom.h>

extern unsigned char EEMEM tasksEEP[9][5];
//extern volatile unsigned char currentTask;

void initSchedule();
void findoutCurrentTask(void);
unsigned char setSchedule(unsigned char tasks[9][5]);//added 4/3/2009 *
void setSafelySchedule(void);
void getSchedule(unsigned char tasks[9][5]);
void getTaskTable(unsigned int tasks[9][3]);
void setTaskTable(unsigned char tasks[9][3]);
unsigned char isSchedule(struct _RTCDATA * Time); //return 0 if no task, at current task 
unsigned char isInTimeSlot(struct _RTCDATA * a, struct _RTCDATA * b, struct _RTCDATA * c);
unsigned char checkCurrentTask();	        //no of phase on
unsigned char getCurrentTask();		        //check task No. 
void setCurrentTask(unsigned char task);        //set task in case change time
void jumpNextTask(void);  
unsigned char checkSchedule(unsigned char tTable[9][5]);//checkSchedule 13/06/2011



#endif /* SCHEDULE_H_ */