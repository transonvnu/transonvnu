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
#include "ADC.h"
#include "relays.h"
//#include "SOAP.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
//consts

#define CMD_STRING_LENGTH	7 
//#define sendSOAPtimeout		180 //seconds

#define BIT(x) (1<<x)

static char commandString[]  = "MEMSLAB";
//static char feedbackString[] = "MINHEFT";
//vars 
volatile unsigned int  Seconds = 0,sendStateTime;
volatile unsigned int  modemTimeout,logTime,SendStateTime;
volatile unsigned char backLightTime,energyTime,nextSecond,nextSecond2,sdCardTime;
static volatile unsigned char stateEightLamps;//,sendingSOAPtime = 0;//,receivFile,recFileTime;
extern char autoResetModem,resetWatchDog;
extern volatile unsigned char modemConnected;

void externalMemoryInit(void){
	DDRC = 0xFF;
	PORTC = 0x00;
	MCUCR |= BIT(6) + BIT(7);   // External SRAM/XMEM Enable
	XMCRA = 0x00;  				// Wait one cycle during read/write strobe
	XMCRB = 0x81;  				// 32kB released PC7 
	
	nextSecond = 0;energyTime=0;sdCardTime = 0;
	sendStateTime = 0;
	modemTimeout = 300;logTime = 30;SendStateTime = 30;
	backLightTime = 0;modemConnected = 0;
	stateEightLamps = 0;
}
void interrupt_init(void){
// INT0: On
// INT0 Mode: Falling Edge
// INT1: On
// INT1 Mode: Falling Edge
// INT2: Off
// INT3: Off
// INT4: Off       
// INT5: Off
// INT6: Off
// INT7: Off
        //EICRA=0x0A;
        //EIMSK=0x03;
        //EIFR=0x03;

		EICRA=0x02;
		EICRB=0x00;
		EIMSK=0x01;
		EIFR=0x01;
// Analog Comparator initialization
// Analog Comparator: Off     
}
//---------------------------------
void initModemWatchDog(void){   //Used timer3 for 1.0Hz interrupt
	char buffer[2]; 	
	readEEPROM(buffer,37,38);	
	modemTimeout = (unsigned char)buffer[0];
	modemTimeout <<= 8;
	modemTimeout |= (unsigned char)buffer[1];  
	if((modemTimeout < 60)||(modemTimeout > 600)){//Neu Modem timeout < 30 giay hoac > 600 giay
		setModemTimeout(30);						  //Add 06/08/2010	
	}	
	Seconds = 0;
}	

//--------------------------------
void resetModemWatchDog(void){
	Seconds = 0;
	modemConnected = 1;
}
void resetModem(void){
	Seconds = modemTimeout + 1;
}
//-------------------------------- 
unsigned char isModemTimeout(void){
	if (Seconds >= modemTimeout) {
		Seconds = 0;
		modemConnected = 0;
		return 1;
	}
	else return 0;
}
unsigned char getModemConnected(void){
	return modemConnected;
}	
//--------------------------------
void setModemTimeout(unsigned int time){//modem time out = time*10 seconds
	char buffer[2];	
	modemTimeout = ((int) time*10);
	buffer[0] = (char)(modemTimeout >> 8);
	buffer[1] = (char)(modemTimeout & 0x00ff);
	writeEEPROM(buffer,37,38);
	Seconds = 0;
}	
//--------------------------------
unsigned int getModemTimeout(void){
	return modemTimeout;
}

//--------------------------------
unsigned int getModemTime(void){
	return Seconds;
}

//-------------------------------- 
unsigned char readCommand(char* args, char * cmd){
	char str[100];//,str1[30];
	unsigned char temp;//,receiveTimeout = 0,numOfByteEchelonCmd = 0;
	signed char tTasksTable[5][5];
	unsigned char j;
	unsigned char cmdCode = 254;
	unsigned int i,tempSendTime[5];
	TimeStruct currentTime1, time1, time2;
	
	if (queueLengthUART2() > 0){
		resetTime();
		i = 0;
		while (i< CMD_STRING_LENGTH){
			while (queueLengthUART2() == 0){
				if (getTime() > COMM_TIMEOUT) return 255;
			}
			temp = readUART2();			
			if (toUpper(temp) == commandString[i]) i++;
			else i = 0;
		}				
		//get the making string, now parse the command number 
		for(i=0;i<22;i++){
			while (queueLengthUART2() == 0){
		    	if (getTime() > COMM_TIMEOUT) return 255;
			}
			str[i] = readUART2();
			cmd[i] = str[i];
			if(!isHexCharacter(str[i])) return 255;		
		}
		cmd[i] = 0;//End of string cmd	
		cmdCode = parseHexString(str[0],str[1]);
		i = 0;	
		//parse the arguments 	
		if(cmdCode == 96){
			for(i=0;i<2;i++){
		        while (queueLengthUART2() == 0){                
					if (getTime() > COMM_TIMEOUT) return 255; 
				}                 
		        str[i] = readUART2();
				if(!isHexCharacter(str[i])) return 255;
			}
			cmdCode = parseHexString(str[0],str[1]);			
		}		
		switch (cmdCode){  		
			case 0:
		    	//lenh dong cat - 1 byte
				for(i=0;i<2;i++){
		        	while (queueLengthUART2() == 0){                
						if (getTime() > COMM_TIMEOUT) return 255; 
					}                 
		        	str[i] = readUART2();
					if(!isHexCharacter(str[i])) return 255;
				}
				args[0] = parseHexString(str[0],str[1]);
				resetModemWatchDog();
		        break; 
		    case 1:
		        //Yeu cau truyen trang thai - khong co noi dung
				resetModemWatchDog();
		        break; 
		    case 2:
  		      	//Dat thoi gian he thong - 6 bytes
		        for(i=0;i<12;i++){      
		        	while (queueLengthUART2() == 0){                
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					str[i] = readUART2();  
					if(!isHexCharacter(str[i])) return 255; 
		        } 
				for(i = 0;i<6;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				}       
		        currentTime1.hour = args[0];
				currentTime1.minute = args[1];
			    currentTime1.date = args[2]; 
				currentTime1.month = args[3];
				currentTime1.year = args[4];
				currentTime1.day = args[5];//06/12/2010
				currentTime1.second = 0;
				//writeRealTime(&currentTime1);
				getSchedule(tTasksTable,tempSendTime);
				for (i = 0; i< 5; i++){
			    	time1.hour = tTasksTable[i][0];
			        time1.minute = tTasksTable[i][1];
			        time2.hour = tTasksTable[i][2];
			        time2.minute = tTasksTable[i][3];
			        if (isInTimeSlot(&currentTime1, &time1, &time2)){
			        	setCurrentTask(i);
			            break;
		            }
	            }
				resetModemWatchDog();
		        break;		
		    case 3: 
		        //Dat che do tu dong - 25 bytes
				//Modified 04/03/2009
		        for(i=0;i<50;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }
					str[i] = readUART2(); 
					if(!isHexCharacter(str[i])) return 255;   
		            //args[i] = readUART2();
		        }
				for(i = 0;i<25;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				}  
				resetModemWatchDog();  
		        break;
		    case 4:
		        //Yeu cau doc the nho
				for(i=0;i<14;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }  
					str[i] = readUART2();  
					if(!isHexCharacter(str[i])) return 255;  
		            //args[i] = readUART2();
		        }
				for(i = 0;i<7;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				}   
				resetModemWatchDog(); 
		        break;
		    case 5:
		        //Ping kiem tra ket noi - khong co noi dung
				resetModemWatchDog();
		        break;
		    case 6:
		        //Lenh reset Energy - khong co noi dung
				resetModemWatchDog();
		        break;
		    case 7:
		        //Lenh Set thoi gian reset modem va thoi gian ghi SD Card
		        for(i=0;i<4;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					str[i] = readUART2();   
					if(!isHexCharacter(str[i])) return 255; 
		            //args[i] = readUART2();
		        }
				for(i = 0;i<2;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				resetModemWatchDog();
		        break;
		    case 8:
		        //Read System Log
				for(i=0;i<4;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					str[i] = readUART2();   
					if(!isHexCharacter(str[i])) return 255; 
		        }
				for(i = 0;i<2;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				resetModemWatchDog();
				break;
		    case 9:
				//Read current schedule - khong co noi dung
				resetModemWatchDog();
		        break; 
		        //break;  
			case 10://0x0A
				//Thay doi trang thai den trang tri 06/12/2010
				for(i=0;i<2;i++){
		        	while (queueLengthUART2() == 0){                
						if (getTime() > COMM_TIMEOUT) return 255; 
					}                 
		        	str[i] = readUART2();
					if(!isHexCharacter(str[i])) return 255;
				}
				args[0] = parseHexString(str[0],str[1]);
				resetModemWatchDog();
				break;
			case 11://0x0B
				// Thay doi schedule den trang tri - 06/12/2010
				for(i=0;i<70;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					str[i] = readUART2();   
					if(!isHexCharacter(str[i])) return 255; 
		        }
				for(i = 0;i<35;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				resetModemWatchDog();
				break;
			case 12://0x0C
				// Yeu cau truyen trang thai den trang tri - 06/12/2010 - Khong co noi dung
				resetModemWatchDog();
				break;
			case 13://0x0D
				// Yeu cau truyen schedule den trang tri - 07/12/2010 - Khong co noi dung
				resetModemWatchDog();
				break;
			case 14://0x0E
				
				resetModemWatchDog();
				break;
			case 15://0x0F
				// Reset lai he thong - 16/06/2011 - Khong co noi dung
				resetModemWatchDog();
				break;
			case 16://0x10
				
				resetModemWatchDog();
				break;
			case 17://0x11
				
				resetModemWatchDog();
				break;
			case 18://0x12
				
				resetModemWatchDog();
				break;
			case 19://0x13
				
				resetModemWatchDog();
				break;
			case 20://0x14
				
				resetModemWatchDog();
				break;
			case 21://0x15
				//Doc trang thai the nho SD Card
				resetModemWatchDog();
				break;
			case 22://0x16 Doc cac thong so calib cua he thong - khong co noi dung
				readEEPROM(str,48,71);//cE,cU,cI
				for(i = 0;i<4;i++){
					args[i] = str[3-i];//cE1 = xx xx xx xx MSBs first
				} 
				for(i = 4;i<8;i++){
					args[i] = str[11-i];//cE2 = xx xx xx xx MSBs first
				}
				for(i = 8;i<12;i++){
					args[i] = str[19-i];//cE3 = xx xx xx xx MSBs first
				}
				for(i = 12;i<14;i++){
					args[i] = str[25-i];//cU1 = xx xx  MSBs first
				}
				for(i = 14;i<16;i++){
					args[i] = str[29-i];//cU2 = xx xx  MSBs first
				}
				for(i = 16;i<18;i++){
					args[i] = str[33-i];//cU3 = xx xx  MSBs first
				}
				for(i = 18;i<20;i++){
					args[i] = str[37-i];//cI1 = xx xx  MSBs first
				}
				for(i = 20;i<22;i++){
					args[i] = str[41-i];//cI2 = xx xx  MSBs first
				}
				for(i = 22;i<24;i++){
					args[i] = str[45-i];//cI3 = xx xx  MSBs first
				}	
				readEEPROM(str,72,75);//TiCoeff1/TiCoeff2
				for(i = 24;i<26;i++){
					args[i] = str[25-i];//TiCoeff1 = xx xx  MSBs first
				}
				for(i = 26;i<28;i++){
					args[i] = str[29-i];//TiCoeff2 = xx xx  MSBs first
				}
				readEEPROM(str,37,40);//modemTimeOut
				for(i = 28;i<30;i++){
					args[i] = str[i-28];//modemTimeOut = xx xx  MSBs first
				}
				//args[0..29]
				resetModemWatchDog();
				break;
			case 23://0x17 Doc SystemLog month, year
				for(i=0;i<4;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }  
					str[i] = readUART2();  
					if(!isHexCharacter(str[i])) return 255;  
		        }
				for(i = 0;i<2;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				}   
				resetModemWatchDog(); 
		        break;	
			case 24://0x18 Reset Modem tu SMS - 07_05_2009
				// -> khong co noi dung
				break;			
			case 25://0x19 : 29-07-2009 lenh SET DIMMING
		        for(i=0;i<18;i++){  //U1,U2,U3,h1,m1,h2,m2,h3,m3: 1 1 1 1 1 1 1 1 1 = 9 Bytes    
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    } 
					str[i] = readUART2();  
					if(!isHexCharacter(str[i])) return 255; 
		        } 
				for(i = 0;i<9;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				}       
				resetModemWatchDog();
		        break;		
			case 26:
				resetModemWatchDog();
				break;
			case 27://0x1B : 28-01-2010 End Of File
				resetModemWatchDog();
		        break;	
			case 30://0x1E : 23-08-2011 Lenh dieu khien LED bo ho
				j = 0;
				do{
					for(i=0;i<2;i++){
						while (queueLengthUART2() == 0){               
							if (getTime() > COMM_TIMEOUT){
								return 255;	
							}
					    } 
						str[i] = readUART2();
						if(!isHexCharacter(str[i])) return 255; 
					}
					args[j] = parseHexString(str[0],str[1]);
					j++;
				}while(args[j-1] != '#');
				args[j-1] = 0;				
				resetModemWatchDog();
				break;		
			case 31://0x1F : 10-09-2011 Lenh dieu khien LED bo ho bang SMS
			/*
				j = 0;
				do{					
					while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255;	
						}
				    } 
					args[j] = readUART2();
					j++;
				}while(args[j-1] != '#');
				args[j] = 0;	 */						
				resetModemWatchDog();
				break;	
			case 32://0x20 : 10-09-2011 Lenh dieu khien LED bo ho bang SMS
			/*
				j = 0;
				do{					
					while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255;	
						}
				    } 
					args[j] = readUART2();
					j++;
				}while(args[j-1] != '#');
				args[j] = 0;	*/						
				resetModemWatchDog();
				break;	
			case 48:
				resetModemWatchDog();   
		        break;	
			//New Echelon commands 10/08/2011
			case 51://0x33 : Lenh set ID cho Node:	[0][0][New ID][6 Bytes Neuron ID]
				for(i=0;i<18;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<9;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 	
				resetModemWatchDog();
				break;		
			case 52://0x34 lenh Dim bang tay:  [0][1][ID][1 Byte Gia tri DIM]			
				for(i=0;i<8;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<4;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 	
				resetModemWatchDog();
				break;	
			case 53://0x35 lenh set thoi gian cho node:[0][2][ID][0][1 Byte phut][1 Byte gio][1 byte thu][1 Byte ngay][1 Byte thang][1 Byte nam]
				for(i=0;i<20;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<10;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				resetModemWatchDog();
				break;
			case 54://0x36 lenh set schedule: [0][3][ID=255][1 Byte so moc][1 Byte h1][1 Byte p1][DIM VALUE 1][1 Byte h2][1 Byte p2]
					//[DIM VALUE 2][1 Byte h3][1 Byte p3][DIM VALUE 3][1 Byte h4][1 Byte p4][DIM VALUE 4][1 Byte h5] 
					//[1 Byte p5][DIM VALUE 5][1 Byte h6][1 Byte p6][DIM VALUE 6][1 Byte h7][1 Byte p7][DIM VALUE 7] 
					//[1 Byte h8][1 Byte p8][DIM VALUE 8][1 Byte h9][1 Byte p9][DIM VALUE 9] 
				for(i=0;i<8;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<4;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				for(i=8;i<args[3]*6+8;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 4;i<args[3]*3+4;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 
				resetModemWatchDog();
		        break;
			case 55://0x37
				resetModemWatchDog();
		        break;
			case 56://0x38: Doc trang thai cua Node	: 	[2] [1] [ID]
				for(i=0;i<6;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<3;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 	
				resetModemWatchDog();
				break;
			case 57://0x39 : Set Polling rate: [0] [6] [ID] [2 Byte Polling Rate]
				for(i=0;i<10;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<5;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 	
				resetModemWatchDog();
		        break;
			case 58://0x3A : Read Node's schedule: [2] [3] [ID]
				for(i=0;i<6;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT){
							return 255; 
						}
				    } 	
					str[i] = readUART2();	
					if(!isHexCharacter(str[i])) return 255; 		
		        }
				for(i = 0;i<3;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);
				} 	
				resetModemWatchDog();
		        break;
			case 59://0x3B
				resetModemWatchDog();
		        break;

			case 60://0x3C

				resetModemWatchDog();
				break;
		    case 69://0x45
		        //Dat cac he so calib E,U,I
		        for(i=0;i<48;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }  
		            str[i] = readUART2(); 
					if(!isHexCharacter(str[i])) return 255;
		        }
				for(i = 0;i<24;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);//parse byte
				} 
				resetModemWatchDog();              
		        break;
			case 70://0x46: Dat gia tri dien nang cho tung Phase
				for(i=0;i<24;i++){
		        	while (queueLengthUART2() == 0){               
						if (getTime() > COMM_TIMEOUT) return 255; 
				    }  
		            str[i] = readUART2(); 
					if(!isHexCharacter(str[i])) return 255;
		        }
				for(i = 0;i<12;i++){
					args[i] = parseHexString(str[2*i],str[2*i+1]);//parse byte
				} 
				resetModemWatchDog();   
				break;
		    default :
		        return 255;
		}//End switch(cmdCode)... 

		//flushUART2RX();//Nhan OK la clear bo dem

		return cmdCode;
	}// End if(queueLengthUART2() > 0)		
	return 254; //nothing --> return 254
}//End readCommand()...  

//string buider functions
void prepareLightStatus(char lightStatus, char * buffer){
	//arguments
	byteToHexString(lightStatus, buffer);
	buffer[2] = 0;
}

//----------------------------------------------------------
void prepareScheduleStatus(signed char scheduleStatus[5][5],unsigned int SendTime[5], char* buffer){
	unsigned char i, j;	
	i = 0;
	//arguments
	for (j = 0; j< 5; j++){
		byteToHexString(scheduleStatus[j][0], buffer + i);					//hour
		i+=2;
		byteToHexString(scheduleStatus[j][1], buffer + i);					//minute
		i+=2;
		byteToHexString(scheduleStatus[j][4], buffer + i);					//light status
		i+=2;
		wordToHexString(SendTime[j], buffer + i);							//Send State Time: Modified 04/03/2009
		i+=4;
	}
	buffer[i] = 0;
}

void prepareSchedule1Status(char * str,char* buffer){
	unsigned char i, j;	
	i = 0;
	//arguments
	for (j = 0; j< 35; j++){
		byteToHexString(str[j], buffer + i);					//hour
		i+=2;
	}
	buffer[i] = 0;
}
//----------------------------------------------------------	
void prepareRealtimeStatus(TimeStruct* timeStatus, char* buffer){
	unsigned char i;
	i = 0;	
	//arguments
	byteToHexString((*timeStatus).hour, buffer + i);	
	i+=2;
	byteToHexString((*timeStatus).minute, buffer + i);
	i+=2;
	byteToHexString((*timeStatus).second, buffer + i);
	i+=2;
	byteToHexString((*timeStatus).date, buffer + i);
	i+=2;
	byteToHexString((*timeStatus).month, buffer + i);
	i+=2;
	byteToHexString((*timeStatus).year, buffer + i);
	i+=2;
	buffer[i] = 0;
}	

//----------------------------------------------------------
void prepareTransducerStatus(unsigned int* voltage, unsigned int* current, unsigned char * phase,EnergySruct energy, unsigned int * voltageDim, char* buffer){
	unsigned char i, j;
	i = 0;
	//arguments
	for (j = 0; j < 3; j++){
		wordToHexString(voltage[j], buffer + i);
		i+=4;
	}
	for (j = 0; j < 3; j++){
		wordToHexString(current[j], buffer + i);
		i+=4;
	}
	
	for (j = 0; j < 3; j++){
		byteToHexString(phase[j], buffer + i);
		i+=2;
	}	
	for (j = 0; j < 3; j++){
		threeByteToHexString(energy.energy[j], buffer + i);
		i+=6;
	}
	for (j = 0; j < 3; j++){
		wordToHexString(voltageDim[j], buffer + i);
		i+=4;
	}
	buffer[i] = 0;
}	

//---------------------------

void feedBackCmd(char * cmd,char * Buffer){
	writeStringUART2("AT+MYCMD=\"");
	threeByteToHexString((((unsigned long)cmd[0])<<16)+(((unsigned long)cmd[1])<<8)+((unsigned long)cmd[2]),Buffer);
	writeMStringUART2(Buffer);
	writeStringUART2("\"\r\n");
}
	
void led1(unsigned char state){      //Control LED1   
    DDRF |= BIT(2); 
    if(state) PORTF |= BIT(2);
    else      PORTF &= (~BIT(2));
}
void led2(unsigned char state){      //Control LED2   
    DDRF |= BIT(1); 
    if(state) PORTF |= BIT(1);
    else      PORTF &= (~BIT(1));  
}        
 
void back_ligh(unsigned char state){    //Back_ligh LCD 
    DDRA |= BIT(7);    
    if(state) PORTA |= BIT(7);
    else      PORTA &= (~BIT(7));
}  

unsigned char isEnergyTime(void){ 
    if(energyTime >= 1){  
		energyTime = 0;
	    return 1; 
    }        
    else return 0;
} 
unsigned char isSdCardTime(void){ 
    if(sdCardTime >= logTime){  
		sdCardTime = 0;
	    return 1; 
    }        
    else return 0;
} 
void writeLogTime(unsigned int time){//write log time = time*10 seconds
	char buffer[2];	
	logTime = ((int) time*10);
	buffer[0] = (char)(logTime >> 8);
	buffer[1] = (char)(logTime & 0x00ff);
	writeEEPROM(buffer,39,40);
}
void setLogTime(unsigned int time){
	logTime = time;
}
unsigned char isNextSecond(void){ // readTime 1 lan tren giay
    if(nextSecond >= 1){
	    nextSecond = 0;
	    return 1;
    }       
    else return 0;
}
unsigned char isNextSecond2(void){ // readTime 1 lan tren giay
    if(nextSecond2 >= 1){
	    nextSecond2 = 0;
	    return 1;
    }       
    else return 0;
}
unsigned char isSendStateTime(void){
	if(sendStateTime >= SendStateTime){  
		sendStateTime = 0;
	    return 1; 
    }        
    else return 0;
}
void setSendStateTime(unsigned int time){
	SendStateTime = time;
}

unsigned char getBackLightTime(void){
	return backLightTime;
}
void resetBackLightTime(void){
	backLightTime = 0;
}
unsigned char getStateEightLamps(void){
	return stateEightLamps;
}

void send_SMS(char * number,char * str){
	//send SMS
	writeStringUART2("AT+CMGS=\"");
	writeMStringUART2(number);
	writeStringUART2("\"");
	writeUART2(0x0D);
	_delay_ms(30);
	writeMStringUART2(str);
	writeUART2(0x1A);
	//end of send SMS
}
//------------------------------------------   
ISR(SIG_INTERRUPT0){ 
	//TimeStruct currentTime1;
	nextSecond++;  // var of read Realtime	
	nextSecond2++;
	if(Seconds < 1200){
		Seconds++;       // var of reset Modem
	}
	energyTime++;    // var of read Energy
	sdCardTime++;	 // Thoi gian ghi SD Card dinh ky
	backLightTime++; 
	sendStateTime++; // Thoi gian truyen trang thai ve server dinh ky
	if(autoResetModem == -1){
		Seconds = 0;
	}
	resetWatchDog++;
	if(resetWatchDog < 25){
		wdt_reset(); //Reset Watch Timer before expire time	
	}
	//PORTE ^= BIT(4)+BIT(5);
}

// External Interrupt 1 service routine

//ISR(SIG_INTERRUPT1){ 
// Place your code here
//} 
