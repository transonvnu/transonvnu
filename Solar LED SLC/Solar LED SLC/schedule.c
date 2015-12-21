/*
 * schedule.c
 *
 * Created: 20/09/2013 3:24:54 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include <avr/eeprom.h>
#include "schedule.h"
#include "settings.h"
#include "driver/ds1307.h"
#include "driver/uart.h"

//*******************
//	taskTalbl	0	1	2	3	4 
//	task 0		hh1	mm1	hh2 mm2	
//*******************
unsigned char tasksTable[9][5];
unsigned char EEMEM tasksEEP[9][5] = {
	{1,0,2,0,100},
	{2,0,4,0,100},
	{4,0,17,45,0},
	{17,45,19,0,100},
	{19,0,20,0,100},
	{20,0,21,0,100},
	{21,0,22,0,100},
	{22,0,23,0,100},
	{23,0,1,0,100}
};
#define SIZE_TABLESCH	45

//static volatile unsigned int sendStateTime[9]; // added 4/3/2009
static volatile unsigned char currentTask;

//private funtions
void initSchedule(){
	unsigned char i, j,tempTask[9][5];
	unsigned char * buffer;
	//signed char tempSchedule[9][5];
	//unsigned int tempSendTime[9];
	struct _RTCDATA currentTime, time1, time2; 	
	//read backup schedule
	buffer = (unsigned char *)tempTask;
	for (i = 0; i < 9; i++){
		for (j = 0; j < 5; j++){
			tasksTable[i][j] = 0;
		}
	}
	////readEEPROM(buffer,0,44);
	eeprom_read_block(buffer,tasksEEP, SIZE_TABLESCH);
	/*for (i = 0; i < 9; i++){
		for (j = 0; j < 5; j++){
			putchar1(tempTask[i][j]);
		}
	}*/
	
	if(checkSchedule(tempTask)){
		for (i = 0; i < 9; i++){
			for (j = 0; j < 5; j++){
				tasksTable[i][j] = tempTask[i][j];
			}
		}
		//check current time	
		Read_DS1307(&currentTime);
		//find out currentTask index, corresponding to currentTime
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
		for(i=0;i<9;i++){
			tempTask[i][0] = i+1;
			tempTask[i][1] = 0;
			tempTask[i][2] = i+2;
			tempTask[i][3] = 0;
			tempTask[i][4] = 100;
			//tempSendTime[i] = 60;
		}
		tempTask[8][2] = 1;
		tempTask[8][3] = 0;
		setSchedule(tempTask);	
	}
	
}
void findoutCurrentTask(void){
	unsigned char i;
	struct _RTCDATA currentTime, time1, time2;
	Read_DS1307(&currentTime);
	for (i = 0; i < 9; i++){
		time1.hour = tasksTable[i][0];
		time1.minute = tasksTable[i][1];
		time2.hour = tasksTable[i][2];
		time2.minute = tasksTable[i][3];
		if (isInTimeSlot(&currentTime, &time1, &time2)){
			currentTask = i;
		}
	}
}
//--------------------------------------
unsigned char setSchedule(unsigned char tasks[9][5]){
	unsigned char i, j,tempTasksTable[9][5];
	char buffer[50];
	struct _RTCDATA currentTime, time1, time2;	
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
		Read_DS1307(&currentTime);
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
		//currentTask = findoutCurrentTask();
		//backup schedule
		for (i = 0; i < 9; i++){
			for (j = 0; j < 5; j++){
				buffer[i*5 + j] = tasks[i][j];
			}
		}
		eeprom_write_block(buffer,tasksEEP,SIZE_TABLESCH);
		countWriteEEMEMcycle();
		//writeEEPROM(buffer,0,44);
		/*
		for(i=0;i<9;i++){         // added 4/3/2009	//---------------------
			sendStateTime[i] = sendTime[i];
			buffer[2*i] = (char)(sendTime[i]>>8);
			buffer[2*i+1] = (char)sendTime[i];
		}*/
		////writeEEPROM(buffer,45,62);// added 4/3/2009
		return 1;
	}else{
		return 0;
	}
}

// Back up scheduler, if there is error in tasktable
void setSafelySchedule(void){
	unsigned char i;
	unsigned char tempSchedule[9][5];
	unsigned int tempSendTime[9];
	//	0	1	2	3	4	5	6	7	8	
	//	100	100	100	100	100	100	100	100	100
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

	setSchedule(tempSchedule);	
}

//--------------------------------------
// loading tasktable from 
void getSchedule(unsigned char tasks[9][5]){//Modified 04/03/2009
	unsigned char i, j;
	if(checkSchedule(tasksTable)){
		for (i = 0; i< 9; i++){
			for (j = 0; j< 5; j++){
				tasks[i][j] = tasksTable[i][j];
			}
		}
		/*for (i = 0; i< 9; i++){
			SendTime[i] = sendStateTime[i];
		}*/
	}else{
		initSchedule();
	}
}
void getTaskTable(unsigned int tasks[9][3]){
	unsigned char i;
	eeprom_read_block(tasksTable,tasksEEP, SIZE_TABLESCH);
	for (i = 0; i< 9; i++){
		tasks[i][0] = (unsigned int)tasksTable[i][0];
		tasks[i][1] = (unsigned int)tasksTable[i][1];
		tasks[i][2] = (unsigned int)tasksTable[i][4];
	}
}

void sortTaskTable(unsigned char _tasks[9][3]){
	unsigned char i,j;
	unsigned char _tmparr[3];
	for(i = 8; i>0 ;i--){
		for(j = 0;j < i; j++){
			if(_tasks[j][0] < _tasks[j+1][0]){			// neu gio nho hon ko lam gi ca
				break;
			}else{
				if(_tasks[j][0] == _tasks[j+1][0]){
					if(_tasks[j][1] <= _tasks[j+1][1]){	// neu gio = nhau nhung phut <= ko lam gi ca
						break;
					}
				}
			}
			_tmparr[0]		= _tasks[j][0];
			_tmparr[1]		= _tasks[j][1];
			_tmparr[2]		= _tasks[j][2];
			_tasks[j][0]	= _tasks[j+1][0];
			_tasks[j][1]	= _tasks[j+1][1];
			_tasks[j][2]	= _tasks[j+1][2];
			_tasks[j+1][0]	= _tmparr[0];
			_tasks[j+1][1]	= _tmparr[1];
			_tasks[j+1][2]	= _tmparr[2];
		}
	}
}

void setTaskTable(unsigned char tasks[9][3]){
	unsigned char i;
	unsigned char tempTask[9][5];
	sortTaskTable(tasks);
	for (i = 0; i< 8; i++){
		tempTask[i][0] = tasks[i][0];
		tempTask[i][1] = tasks[i][1];
		tempTask[i][2] = tasks[i+1][0];
		tempTask[i][3] = tasks[i+1][1];
		tempTask[i][4] = tasks[i][2];
	}
	tempTask[8][0] = tasks[8][0];
	tempTask[8][1] = tasks[8][1];
	tempTask[8][2] = tasks[0][0];
	tempTask[8][3] = tasks[0][1];
	tempTask[8][4] = tasks[8][2];
	setSchedule(tempTask);
	//eeprom_write_block(tempTask,tasksEEP,SIZE_TABLESCH);
}

//--------------------------------------
// check input time point is in current task.
// TRUSE: inside current task
// FALSE: outside current task.
 
unsigned char isSchedule(struct _RTCDATA * Time){	//return 0 if number of task at current task
	struct _RTCDATA time1, time2;
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
//return value of lamp power in current task.
//	 = 0% - 100%.
unsigned char checkCurrentTask(){
	return tasksTable[currentTask][4];
}	

//--------------------------------------

unsigned char getCurrentTask(){		//check task number
	return currentTask;
}
//--------------------------------------
//	set global var currentTask: 0 - 8 (task index)
void setCurrentTask(unsigned char task){
       currentTask = task;
}
//--------------------------------------
// increase currentTask (task index): 0 - 8
void jumpNextTask(void){
	currentTask++;
	if (currentTask == 9) currentTask = 0;
}
//===========================================
//private functions
//check if a is between b and c or not
unsigned char isInTimeSlot(struct _RTCDATA * a, struct _RTCDATA * b, struct _RTCDATA * c){			
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


// check schedule talbe is valid or not
// 0: not valid, 1: valid
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
		numOfminute1 = (unsigned int)tTable[i][0]*60 + tTable[i][1];
		numOfminute2 = (unsigned int)tTable[i][2]*60 + tTable[i][3];
		if(numOfminute1 >= numOfminute2){
			nextDay++;
		}
	}
	if(nextDay > 1) {return 0;}
	else return 1;
}
