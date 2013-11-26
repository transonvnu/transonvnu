/*
schedule.c
Implementation of tasks
Date created: 23/07/2008
*/
#include <avr/io.h>
#include "schedule.h"
#include "ds1307.h"
#include "timming.h"
#include "uart.h"
#include "syscomm.h"

//static vars
unsigned char tasksTable[9][5];
static volatile unsigned int sendStateTime[9]; // added 4/3/2009
static volatile unsigned char currentTask;

extern char scheduleError;

//private funtions
void initSchedule(){
	unsigned char i, j,tempTask[9][5];
	char buffer[50];
	//signed char tempSchedule[9][5];
	//unsigned int tempSendTime[9];
	TimeStruct currentTime, time1, time2; 	
	//read backup schedule
	for (i = 0; i < 9; i++){
		for (j = 0; j < 5; j++){
			tasksTable[i][j] = 0;
		}
	}
	readEEPROM(buffer,0,44);
	for (i = 0; i < 9; i++){
		for (j = 0; j < 5; j++){
			tempTask[i][j] = buffer[i*5 + j];
		}
	}
	if(checkSchedule(tempTask)){
		for (i = 0; i < 9; i++){
			for (j = 0; j < 5; j++){
				tasksTable[i][j] = buffer[i*5 + j];
			}
		}
		readEEPROM(buffer,45,62);// added 4/3/2009	
		for(i = 0;i<9;i++){
			sendStateTime[i] = (unsigned char)buffer[i*2];
			sendStateTime[i] <<= 8;
			sendStateTime[i] |= (unsigned char)buffer[i*2+1]; 
		}
		//check current time
		readRealTime(&currentTime);
		for (i = 0; i < 9; i++){
			time1.hour = tasksTable[i][0];
			time1.minute = tasksTable[i][1];
			time2.hour = tasksTable[i][2];
			time2.minute = tasksTable[i][3];
			if (isInTimeSlot(&currentTime, &time1, &time2)){
				currentTask = i;
				break;
			}
		}					
	}else{
		//Add 05/08/2010: Schedule an toan khi loi EEPROM
		/*
		for(i=0;i<9;i++){
			tempSchedule[i][0] = i+1;
			tempSchedule[i][1] = 0;
			tempSchedule[i][2] = i+2;
			tempSchedule[i][3] = 0;
			tempSchedule[i][4] = 100;
			tempSendTime[i] = 60;
		}
		tempSchedule[8][2] = 1;
		tempSchedule[8][3] = 0;

		setSchedule(tempSchedule,tempSendTime);	
		*/
		setSafelySchedule();
	}
	
}
//--------------------------------------
unsigned char setSchedule(signed char tasks[9][5],unsigned int sendTime[9]){
	unsigned char i, j,tempTasksTable[9][5];
	char buffer[50];
	TimeStruct currentTime, time1, time2;	
	//set schedule
	for (i = 0; i< 9; i++){
		for (j = 0; j< 5; j++){
			tempTasksTable[i][j] = tasks[i][j];
		}
	}
	if(checkSchedule(tempTasksTable)){//Schedule is correct
		for (i = 0; i< 9; i++){
			for (j = 0; j< 5; j++){
				tasksTable[i][j] = tasks[i][j];
			}
		}
		//check current time
		readRealTime(&currentTime);
		for (i = 0; i < 9; i++){
			time1.hour = tasksTable[i][0];
			time1.minute = tasksTable[i][1];
			time2.hour = tasksTable[i][2];
			time2.minute = tasksTable[i][3];
			if (isInTimeSlot(&currentTime, &time1, &time2)){
				currentTask = i;
				break;
			}
		}
		//backup schedule
		for (i = 0; i < 9; i++){
			for (j = 0; j < 5; j++){
				buffer[i*5 + j] = tasks[i][j];
			}
		}
		writeEEPROM(buffer,0,44);
		for(i=0;i<9;i++){         // added 4/3/2009
			sendStateTime[i] = sendTime[i];
			buffer[2*i] = (char)(sendTime[i]>>8);
			buffer[2*i+1] = (char)sendTime[i];
		}
		writeEEPROM(buffer,45,62);// added 4/3/2009
		return 1;
	}else{
		return 0;
	}
}

void setSafelySchedule(void){
	unsigned char i;
	signed char tempSchedule[9][5];
	unsigned int tempSendTime[9];

	for(i=0;i<9;i++){
		tempSchedule[i][0] = i+1;
		tempSchedule[i][1] = 0;
		tempSchedule[i][2] = i+2;
		tempSchedule[i][3] = 0;
		tempSchedule[i][4] = 100;
		tempSendTime[i] = 60;
	}
	tempSchedule[8][2] = 1;
	tempSchedule[8][3] = 0;

	setSchedule(tempSchedule,tempSendTime);	
}
//--------------------------------------
void getSchedule(signed char tasks[9][5],unsigned int SendTime[9]){//Modified 04/03/2009
	unsigned char i, j;
	if(checkSchedule(tasksTable)){
		for (i = 0; i< 9; i++){
			for (j = 0; j< 5; j++){
				tasks[i][j] = tasksTable[i][j];
			}
		}
		for (i = 0; i< 9; i++){
			SendTime[i] = sendStateTime[i];
		}
	}else{
		initSchedule();
	}
}	

//--------------------------------------
unsigned char isSchedule(TimeStruct * Time){	//return 0 if number of task at current task
	TimeStruct time1, time2;	
	time1.hour = tasksTable[currentTask][0];
	time1.minute = tasksTable[currentTask][1];
	time2.hour = tasksTable[currentTask][2];
	time2.minute = tasksTable[currentTask][3];
	if (isInTimeSlot(Time, &time1, &time2)){
		//setSendStateTime(sendStateTime[currentTask]);// added 4/3/2009
		return 1;
	}
	else return 0;
}
//--------------------------------------
unsigned char checkCurrentTask(){	//number of phase on
	return tasksTable[currentTask][4];
}	

//--------------------------------------
/*
unsigned char getCurrentTask(){		//check task number
	return currentTask;
}*/
//--------------------------------------	
void setCurrentTask(unsigned char task){
       currentTask = task;
}
//--------------------------------------
void jumpNextTask(void){
	currentTask++;
	if (currentTask == 9) currentTask = 0;
}
//===========================================
//private functions
unsigned char isInTimeSlot(TimeStruct* a, TimeStruct* b, TimeStruct* c){			//check if a is between b and c or not
	int noOfMins1, noOfMins2, noOfMins3;
	noOfMins1 = ((int) (*a).hour) * 60 + ((int) (*a).minute);
	noOfMins2 = ((int) (*b).hour) * 60 + ((int) (*b).minute);
	noOfMins3 = ((int) (*c).hour) * 60 + ((int) (*c).minute);		
	if (noOfMins2 <= noOfMins3){//chua sang ngay moi
		if ((noOfMins1 >= noOfMins2) && (noOfMins1 <= noOfMins3)) return 1;//can co dau bang
		else return 0;
	} 
	else{  //da sang ngay moi
		if ((noOfMins1 >= noOfMins2) || (noOfMins1 <= noOfMins3)) return 1;//can co dau bang
		else return 0;		
	}
}	

unsigned char checkSchedule(unsigned char tTable[9][5]){
	unsigned int numOfminute1 = 0,numOfminute2 = 0;
	unsigned char i,nextDay = 0;
	for (i = 0; i< 9; i++){		
		if((tTable[i][0] > 23)||(tTable[i][2] > 23)){
			return 0;
		}
		if((tTable[i][1] > 59)||(tTable[i][3] > 59)){
			return 0;
		}
		numOfminute1 = (int)tTable[i][0]*60 + tTable[i][1];
		numOfminute2 = (int)tTable[i][2]*60 + tTable[i][3];
		if(numOfminute1 >= numOfminute2){
			nextDay++;	
		}
	}
	if(nextDay > 1) return 0;
	else return 1;
}
