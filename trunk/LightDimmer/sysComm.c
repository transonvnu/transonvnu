/*
sysComm.c
Implementation of interfacing with server and hid terminal
Date created: 02/08/2008
	03 - 03 - 2009 Them thong so thoi gian gui trang thai len 
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uart.h"
#include "sysComm.h"
#include "timming.h"
#include "string1.h" 
#include "spi.h" 
#include "schedule.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
//consts

#define CMD_STRING_LENGTH	4

#define BIT(x) (1<<x)

static char commandString[]  = "NODE";//"ECHELON"

//vars 
//volatile unsigned int  sendStateTime = 0;
//volatile unsigned int  SendStateTime;
volatile unsigned char energyTime = 0,nextSecond = 0,resetWatchDog = 0;

unsigned char readCommand(char* args){
	//char str[100];
	unsigned char temp;
	signed char tTasksTable[9][5];
	//unsigned char j;
	unsigned char cmdCode = 254;
	unsigned int i,tempSendTime[9];
	TimeStruct currentTime1, time1, time2;
		
	if (queueLengthUART() > 0){
		resetTime();
		i = 0;
		while (i< CMD_STRING_LENGTH){
			while (queueLengthUART() == 0){
				if (getTime() > COMM_TIMEOUT) return 255;
			}
			temp = readUART();			
			if (toUpper(temp) == commandString[i]) i++;
			else i = 0;
		}				
		//get the making string, now parse the command number 	
		while (queueLengthUART() == 0){
		    if (getTime() > COMM_TIMEOUT) return 255;
		}
		cmdCode = readUART();
		i = 0;	
		//parse the arguments 	
		if(cmdCode == 69){
			while (queueLengthUART() == 0){                
				if (getTime() > COMM_TIMEOUT) return 255; 
			}                 
		    cmdCode = readUART();			
		}		
		switch (cmdCode){  		
			case 0:
		    	//lenh dong cat - 1 byte				
	        	while (queueLengthUART() == 0){                
					if (getTime() > COMM_TIMEOUT) return 255; 
				}                 		        	
				args[0] = readUART();
		        break; 
		    case 1:
		        //Yeu cau truyen trang thai - khong co noi dung
		        break; 
		    case 2:
  		      	//Dat thoi gian he thong - 6 bytes
		        for(i=0;i<6;i++){      
		        	while (queueLengthUART() == 0){                
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					args[i] = readUART();  
		        }      
		        currentTime1.hour = args[0];
				currentTime1.minute = args[1];
			    currentTime1.date = args[2]; 
				currentTime1.month = args[3];
				currentTime1.year = args[4];
				currentTime1.day = args[5];
				currentTime1.second = 0;
				//writeRealTime(&currentTime1);
				getSchedule(tTasksTable,tempSendTime);
				for (i = 0; i< 9; i++){
			    	time1.hour = tTasksTable[i][0];
			        time1.minute = tTasksTable[i][1];
			        time2.hour = tTasksTable[i][2];
			        time2.minute = tTasksTable[i][3];
			        if (isInTimeSlot(&currentTime1, &time1, &time2)){
			        	setCurrentTask(i);
			            break;
		            }
	            }
		        break;		
		    case 3: 
		        //Dat che do tu dong - 45 bytes
				//Modified 04/03/2009
		        for(i=0;i<45;i++){
		        	while (queueLengthUART() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }
					args[i] = readUART(); 		
		        }
		        break;
		    case 4:	
				//Lenh yeu cau truyen schedule	        
		        break;
		    case 5:
		        
		        break;
		    case 6:
		        //Lenh reset Energy - khong co noi dung
		        break;				
		    case 7:
		        //Dat cac he so calib E,U,I
		        for(i=0;i<8;i++){
			        while (queueLengthUART() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
					}  
		      		args[i] = readUART(); 
				}			             
		        break;		
						
			case 8://0x46: Dat gia tri dien nang
				for(i=0;i<3;i++){
			        while (queueLengthUART() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
					}  
			        args[i] = readUART();  
				}
				break;
			case 9:
				args[0] = readUART();
				break;
			case 10:
				break;
		    default :
		        return 255;
				
		}//End switch(cmdCode)... 
		return cmdCode;
	}// End if(queueLengthUART() > 0)		
	return 254; //nothing --> return 254
}//End readCommand()... 

unsigned char isEnergyTime(void){ 
    if(energyTime >= 3){  
		energyTime = 0;
	    return 1; 
    }        
    else return 0;
}
unsigned char isNextSecond(void){ // readTime 1 lan tren giay
    if(nextSecond >= 1){
	    nextSecond = 0;
	    return 1;
    }       
    else return 0;
}
/*
unsigned char isSendStateTime(void){
	if(sendStateTime >= SendStateTime){  
		sendStateTime = 0;
	    return 1; 
    }        
    else return 0;
}
void setSendStateTime(unsigned int time){
	SendStateTime = time;
}*/
//---------- ds1307 SQW ----------------------------

ISR(INT1_vect){ 
	nextSecond++;  // var of read Realtime	
	energyTime++;    // var of read Energy
	//sendStateTime++; // Thoi gian truyen trang thai ve server dinh ky
	
	resetWatchDog++;
	if(resetWatchDog < 5){
		wdt_reset(); //Reset Watch Timer before expire time	
	}
}
