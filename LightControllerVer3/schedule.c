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
#include "relays.h"

//static vars
unsigned char tasksTable[5][5];// __attribute__ ((section (".init1"))); 
//static volatile unsigned char dimmingTimeTable[3][2]; //29-07-2009
static volatile unsigned int sendStateTime[5]; // added 4/3/2009
static volatile unsigned char currentTask,currentTask1;
unsigned char taskDim[3];// = {0,0,0};	     // 30/07/2009
//unsigned char Udim[3];     // Voltage for dimming
unsigned char tasksTable1[14][3];//24/11/2010 decorate schedule
extern char scheduleError;
extern char schedule1Error;

//private funtions
void initSchedule(){
	unsigned char i, j,tempTask[5][5];
	char buffer[30];
	signed char tempSchedule[5][5];
	unsigned int tempSendTime[5];
	TimeStruct currentTime, time1, time2; 	
	//read backup schedule
	for (i = 0; i< 5; i++){
		for (j = 0; j< 5; j++){
			tasksTable[i][j] = 0;
		}
	}
	readEEPROM(buffer,0,24);
	for (i = 0; i< 5; i++){
		for (j = 0; j< 5; j++){
			tempTask[i][j] = buffer[i*5 + j];
		}
	}
	if(checkSchedule(tempTask)){
		for (i = 0; i< 5; i++){
			for (j = 0; j< 5; j++){
				tasksTable[i][j] = buffer[i*5 + j];
			}
		}
		readEEPROM(buffer,76,85);// added 4/3/2009	
		for(i = 0;i<5;i++){
			sendStateTime[i] = (unsigned char)buffer[i*2];
			sendStateTime[i] <<= 8;
			sendStateTime[i] |= (unsigned char)buffer[i*2+1]; 
		}
		//check current time
		readRealTime(&currentTime);
		for (i = 0; i< 5; i++){
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
		if(scheduleError < 5){
			scheduleError++;
		}
		if(scheduleError >= 5){
			phase1_on_off(0);
			phase2_on_off(0);
			tempSchedule[0][0] = 18;
			tempSchedule[0][1] = 0;
			tempSchedule[0][2] = 21;
			tempSchedule[0][3] = 0;
			tempSchedule[0][4] = 0;
			tempSchedule[1][0] = 21;
			tempSchedule[1][1] = 0;
			tempSchedule[1][2] = 23;
			tempSchedule[1][3] = 0;
			tempSchedule[1][4] = 0;
			tempSchedule[2][0] = 23;
			tempSchedule[2][1] = 0;
			tempSchedule[2][2] = 2;
			tempSchedule[2][3] = 0;
			tempSchedule[2][4] = 0;
			tempSchedule[3][0] = 2;
			tempSchedule[3][1] = 0;
			tempSchedule[3][2] = 5;
			tempSchedule[3][3] = 45;
			tempSchedule[3][4] = 0;
			tempSchedule[4][0] = 5;
			tempSchedule[4][1] = 45;
			tempSchedule[4][2] = 18;
			tempSchedule[4][3] = 0;
			tempSchedule[4][4] = 0;
			for(i = 0;i<5;i++){
				tempSendTime[i] = 60;
			}
			setSchedule(tempSchedule,tempSendTime);	
		}
	}
	
}
void initDecorateSchedule(void){//Decorate Light, Add 24/11/2010
	unsigned char i, j,k,tempTask1[14][3];
	char buffer1[43];
	signed char tempSchedule1[14][3];
	//read backup schedule
	readEEPROM(buffer1,131,172);
	for (i = 0; i< 14; i++){
		for (j = 0; j< 3; j++){
			tempTask1[i][j] = buffer1[i*3 + j];
		}
	}
	if(checkSchedule1(tempTask1)){
		for (i = 0; i< 14; i++){
			for (j = 0; j< 3; j++){
				tasksTable1[i][j] = buffer1[i*3 + j];
			}
		}
	}else{	
		if(schedule1Error < 5){
			schedule1Error++;
		}
		if(schedule1Error >= 5){
			phase3_on_off(0);	
			for(k=0;k<7;k++){
				tempSchedule1[2*k][0] = 18; 
				tempSchedule1[2*k][1] = 0; 
				tempSchedule1[2*k+1][0] = 5; 
				tempSchedule1[2*k+1][1] = 30; 
				tempSchedule1[2*k][2] = 0; 
				tempSchedule1[2*k+1][2] = 0; 
			}
			setSchedule1(tempSchedule1);	
		}	
	}
}
unsigned char setSchedule1(signed char tasks[14][3]){
	unsigned char i, j,tempTasksTable1[14][3];
	char buffer[43];
	TimeStruct currentTime;	
	for (i = 0; i< 14; i++){
		for (j = 0; j< 3; j++){
			tempTasksTable1[i][j] = tasks[i][j];
		}
	}
	//set schedule
	if(checkSchedule1(tempTasksTable1)){
		for (i = 0; i< 14; i++){
			for (j = 0; j< 3; j++){
				tasksTable1[i][j] = tasks[i][j];
			}
		}
		//check current time
		readRealTime(&currentTime);
		findCurrentTask1(&currentTime);
		//backup schedule
		for (i = 0; i< 14; i++){
			for (j = 0; j< 3; j++){
				buffer[i*3 + j] = tasks[i][j];
			}
		}
		writeEEPROM(buffer,131,172);
		return 1;
	}else{
		return 0;
	}
}
//--------------------------------------
unsigned char setSchedule(signed char tasks[5][5],unsigned int sendTime[5]){
	unsigned char i, j,tempTasksTable[5][5];
	char buffer[30];
	TimeStruct currentTime, time1, time2;	
	//set schedule
	for (i = 0; i< 5; i++){
		for (j = 0; j< 5; j++){
			tempTasksTable[i][j] = tasks[i][j];
		}
	}
	if(checkSchedule(tempTasksTable)){//Schedule is correct
		for (i = 0; i< 5; i++){
			for (j = 0; j< 5; j++){
				tasksTable[i][j] = tasks[i][j];
			}
		}
		//check current time
		readRealTime(&currentTime);
		for (i = 0; i< 5; i++){
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
		for (i = 0; i< 5; i++){
			for (j = 0; j< 5; j++){
				buffer[i*5 + j] = tasks[i][j];
			}
		}
		writeEEPROM(buffer,0,24);
		for(i=0;i<5;i++){         // added 4/3/2009
			sendStateTime[i] = sendTime[i];
			buffer[2*i] = (char)(sendTime[i]>>8);
			buffer[2*i+1] = (char)sendTime[i];
		}
		writeEEPROM(buffer,76,85);// added 4/3/2009
		return 1;
	}else{
		return 0;
	}
}
//--------------------------------------
void getSchedule(signed char tasks[5][5],unsigned int SendTime[5]){//Modified 04/03/2009
	unsigned char i, j;
	if(checkSchedule(tasksTable)){
		for (i = 0; i< 5; i++){
			for (j = 0; j< 5; j++){
				tasks[i][j] = tasksTable[i][j];
			}
		}
		for (i = 0; i< 5; i++){
			SendTime[i] = sendStateTime[i];
		}
	}else{
		initSchedule();
	}
}	
void getSchedule1(signed char tasks[14][3]){//07/12/2010
	unsigned char i, j;
	for (i = 0; i< 14; i++){
		for (j = 0; j< 3; j++){
			tasks[i][j] = tasksTable1[i][j];
		}
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
		setSendStateTime(sendStateTime[currentTask]);// added 4/3/2009
		setLogTime(sendStateTime[currentTask]);
		return 1;
	}
	else return 0;
}
//--------------------------------------
unsigned char checkCurrentTask(){	//number of phase on
	return tasksTable[currentTask][4];
}	
unsigned char checkCurrentTask1(){
	return tasksTable1[currentTask1][2];
}
//--------------------------------------
unsigned char getCurrentTask(){		//check task number
	return currentTask;
}
//--------------------------------------	
void setCurrentTask(unsigned char task){
       currentTask = task;
}
//--------------------------------------
void jumpNextTask(void){
	currentTask++;
	if (currentTask == 5) currentTask = 0;
}
unsigned char getCurrentTask1(){		//check task number
	return currentTask1;
}
unsigned char checkNextTask1(unsigned char task){
	if(task < 13)
		return (task+1);
	else
		return 0;
}
void jumpNextDimmTask(unsigned char phase){
	taskDim[phase]++;
	if (taskDim[phase] == 2) taskDim[phase] = 0;
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
unsigned int numOfminute(TimeStruct* time){
	unsigned int noOfMins;
	noOfMins = ((int) (*time).hour) * 60 + ((int) (*time).minute);
	return noOfMins;
}
void findCurrentTask1(TimeStruct* time){//Decorate Light, Add 24/11/2010
	unsigned char i,temp;
	if(checkSchedule1(tasksTable1)){
		temp = (*time).day;
		if(temp == 1){//monday
			if(numOfminute(time) < ((int)tasksTable1[0][0]*60+(int)tasksTable1[0][1])){
				if(((int)tasksTable1[13][0]*60+(int)tasksTable1[13][1]) > ((int)tasksTable1[12][0]*60+(int)tasksTable1[12][1])){
					currentTask1 = 13;
				}else{
					if(numOfminute(time)<((int)tasksTable1[13][0]*60+(int)tasksTable1[13][1])){
						currentTask1 = 12;
					}else 
						currentTask1 = 13;
				}
			}else{
				if(((int)tasksTable1[0][0]*60+(int)tasksTable1[0][1])>((int)tasksTable1[1][0]*60+(int)tasksTable1[1][1])){
						currentTask1 = 0;		
				}else{
					if(numOfminute(time) < ((int)tasksTable1[1][0]*60+(int)tasksTable1[1][1])){
						currentTask1 = 0;
					}else{
						currentTask1 = 1;
					}
				}
			}		
		}//end monday
		else{
			for(i = 2;i<8;i++){
				if(temp == i){
					if(numOfminute(time) < ((int)tasksTable1[(temp-1)*2][0]*60+(int)tasksTable1[(temp-1)*2][1])){
						if(((int)tasksTable1[(temp-1)*2-1][0]*60+(int)tasksTable1[(temp-1)*2-1][1]) > ((int)tasksTable1[(temp-1)*2-2][0]*60+(int)tasksTable1[(temp-1)*2-2][1])){
							currentTask1 = (temp-1)*2-1;
						}else{
							if(numOfminute(time)<((int)tasksTable1[(temp-1)*2-1][0]*60+(int)tasksTable1[(temp-1)*2-1][1])){
								currentTask1 = (temp-1)*2-2;
							}else 
								currentTask1 = (temp-1)*2-1;
						}
					}else{
						if(((int)tasksTable1[(temp-1)*2][0]*60+(int)tasksTable1[(temp-1)*2][1])>((int)tasksTable1[(temp-1)*2+1][0]*60+(int)tasksTable1[(temp-1)*2+1][1])){
								currentTask1 = (temp-1)*2;		
						}else{
							if(numOfminute(time) < ((int)tasksTable1[(temp-1)*2+1][0]*60+(int)tasksTable1[(temp-1)*2+1][1])){
								currentTask1 = (temp-1)*2;
							}else{
								currentTask1 = (temp-1)*2+1;
							}
						}
					}
					break;		
				}
			}
		}
	}else{
		initDecorateSchedule();
	}
	//DisplayNumber(40,117,(long)currentTask1);
}
unsigned char checkSchedule(unsigned char tTable[5][5]){
	unsigned int numOfminute1 = 0,numOfminute2 = 0;
	unsigned char i,nextDay = 0;
	for (i = 0; i< 5; i++){		
		if((tTable[i][0] > 23)||(tTable[i][2] > 23)){
			return 0;
		}
		if((tTable[i][1] > 59)||(tTable[i][3] > 59)){
			return 0;
		}
		if(tTable[i][4] > 3){
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
unsigned char checkSchedule1(unsigned char tTable1[14][3]){
	unsigned int numOfminute1 = 0,numOfminute2 = 0,numOfminute3 = 0;
	unsigned char i;
	
	if((tTable1[12][0] > 23)||(tTable1[12][1] > 59)){
		return 0;
	}
	if((tTable1[13][0] > 23)||(tTable1[13][1] > 59)){
		return 0;
	}
	for(i=0;i<6;i++){
		if((tTable1[2*i][0] > 23)||(tTable1[2*i][1] > 59)){
			return 0;
		}
		if((tTable1[2*i+1][0] > 23)||(tTable1[2*i+1][1] > 59)){
			return 0;
		}
		numOfminute1 = (int)tTable1[2*i][0]*60 + tTable1[2*i][1];
		numOfminute2 = (int)tTable1[2*i+1][0]*60 + tTable1[2*i+1][1];
		numOfminute3 = (int)tTable1[2*i+2][0]*60 + tTable1[2*i+2][1];
		if((numOfminute2 <= numOfminute1)&&(numOfminute3 <= numOfminute2)){
			return 0;
		}
	}
	numOfminute1 = (int)tTable1[12][0]*60 + tTable1[12][1];
	numOfminute2 = (int)tTable1[13][0]*60 + tTable1[13][1];
	numOfminute3 = (int)tTable1[0][0]*60 + tTable1[0][1];
	if((numOfminute2 <= numOfminute1)&&(numOfminute3 <= numOfminute2)){
		return 0;
	}
	return 1;
}
