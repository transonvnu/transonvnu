
/*****************************************************
Project : Single Phase Power Meter 
Version : 0.00
Date    : 27/10/2008
Author  : minheft                           
Company : Mems                            
Comments: Test

Chip type           : ATmega8
Program type        : Application
Clock frequency     : 8.000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*****************************************************/

#include <avr/io.h>
//#include <compat/deprecated.h> 
//#include <stdio.h>
//#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h> 

#include "schedule.h"
#include "sysComm.h"  
#include "timming.h"
#include "manual_i2c.h" 
#include "ds1307.h"
#include "uart.h"
#include "ade7753.h"
#include "spi.h"



#define BIT(x)     (1<<x)

#define BTA_ON     				PORTC |= BIT(2)			//03/12/12
#define BTA_OFF    				PORTC &= (~BIT(2))		//03/12/12

#define PF_COMPEN_BTA_ON   		PORTC |= BIT(1)			//03/12/12
#define PF_COMPEN_BTA_OFF 		PORTC &= (~BIT(1))		//03/12/12

#define SAFELY_RELAY_ON 		PORTB |= BIT(0)   
#define SAFELY_RELAY_OFF 		PORTB &= (~BIT(0))

#define DEBUG_MODE	0

#define OSCCAL_ADD 0x00FF
#define NODEID_ADD 0x00FE

int cutphase = 1,DIM = 0,old_DIM = 0,DIMcmd = 0;

char open = 0,ON = 0;
extern unsigned char resetWatchDog;

void init(void){
	cli();            // Global disable interrupts
	wdt_enable(WDTO_2S);

	DDRB |= BIT(0) + BIT(1) + BIT(2) + BIT(3) + BIT(5) + BIT(6) + BIT(7);// Relay,Reset ADE,Chip Select ADE,MOSI
																		 // SCK,DIM control,Capacitor control
	BTA_OFF;	
	_delay_ms(500);
	wdt_reset(); 
	//BTA_ON;
	_delay_ms(100);
	SAFELY_RELAY_OFF;
	_delay_ms(100);
	BTA_OFF;
	//_delay_ms(1000);
	//wdt_reset(); 

	initUART();
	rtc_init(0,1,0);  // ds1307.h

	sei();
	_delay_ms(20);
	initTimer0();
	initTransducer();

	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Falling Edge
	// INT1: On
	// INT1 Mode: Rising Edge
	GICR|=0xC0;
	MCUCR=0x0E;
	GIFR=0xC0;
	
}
void sendNODECMD(unsigned char cmdID){
	writeStringUART("NODE");
	writeUART(cmdID);
}

int main(void){ 

	TimeStruct currentTime; 
	char args[120]={0},systemReset = 0;
	unsigned char command=255,i;
	signed char tempSchedule[9][5];
	unsigned int tempSendTime[9];	
	long energy = 123456;
	int voltage = 20123,current = 10456;
	char phase = 96,tempPhase = 0;

	unsigned long  cE;
	unsigned int   cU,cI;	

	//OSCCAL = 0xA5;
	OSCCAL = eeprom_read_byte((const uint8_t *)OSCCAL_ADD);
	init();

#if DEBUG_MODE == 1   
	_delay_ms(20);
	DIM = 50;
	cutphase = 100;
	SAFELY_RELAY_OFF;                                                                                                                                                                                                                                                                                                                                                                             
	while(1){
		//resetWatchDog = 0;
		PORTB |= (BIT(3)+BIT(5));	
		_delay_ms(20);
		PORTB &= ~(BIT(3)+BIT(5));	
		_delay_ms(20);
		writeStringUART("NODE");	
	}	         
#else
	_delay_ms(20);
	writeVOffset(0x9FF);
	writeCH2Offset(1);
	writeVAGAIN(750);
	readRealTime(&currentTime);
	initSchedule();

	while(1){

		if(isNextSecond()){ //Moi giay mot lan doc	
			//resetADE();
			readRealTime(&currentTime);		
				
			if(isSchedule(&currentTime)){	

				old_DIM = DIM;	
				DIMcmd = checkCurrentTask();	
				
				if(DIMcmd >= 100){
					DIM = 100;
				}else{
					if(DIMcmd < 30){							
						PF_COMPEN_BTA_OFF;
						_delay_ms(200);					
						DIM = 0;			
						if(old_DIM == 100){
							forceBTAoff();	
						}	
					}else{

						DIM = DIMcmd;
						cutphase = (100 - DIM)*2;
						if(old_DIM == 100){
							BTA_OFF;
						}				
					}
				}
		        jumpNextTask();
	        }//End if(isSchedule(&currentTime))........	
				
			voltage = readVoltage();
			current = readCurrent();				
			if(isEnergyTime()){
				energy = readEnergy();
				tempPhase = phase;
				phase  = (char)readPhase();
				if((current > 1000)&&(phase > 97)){
					writeVOffset(0x9FF);
					writeCH2Offset(1);
					writeVAGAIN(750);
					phase = tempPhase;
				}						
			}
			
			if(systemReset == 0){
				resetWatchDog = 0;
			}
				
		
		}//End if(isNextSecond())........
				
		command = readCommand(args);			//read command from server

		switch (command){
			
			case 0:	//0x00
			    //Lenh dong cat	tu Server
				//Truyen ve trang thai + schedule table 					
				old_DIM = DIM;
		
				DIMcmd = args[0];
				
				if(DIMcmd >= 100){
					DIM = 100;
				}else{
					if(DIMcmd < 30){							
						PF_COMPEN_BTA_OFF;
						_delay_ms(200);												
						DIM = 0;									
						if(old_DIM == 100){
							forceBTAoff();	
						}						
					}else{
						DIM = DIMcmd;
						cutphase = (100 - DIM)*2;
						if(old_DIM == 100){
							BTA_OFF;
						}				
					}
				}
				sendNODECMD(0);
				break;    
				     	
			case 1:	//0x01
			    //Lenh yeu cau truyen trang thai
							
				energy = voltage;//*current*phase/1000;
				energy *= current;
				energy /= 100000;
				energy *= phase;
				energy /= 100;			


				sendNODECMD(1);

				writeUART(currentTime.hour);
				writeUART(currentTime.minute);
				writeUART(currentTime.date);
				writeUART(currentTime.month);
				writeUART(currentTime.year);

				writeUART((char)DIM);
				
				writeUART(voltage>>8);
				writeUART(voltage);

				writeUART(current>>8);
				writeUART(current);
				
				writeUART(phase);

				//writeUART(energy>>24);
				writeUART(energy>>16);
				writeUART(energy>>8);
				writeUART(energy);				

				break;	
				
		    case 2:	//0x02
		        //Lenh set real time
				
                currentTime.hour = args[0];
				currentTime.minute = args[1];
				currentTime.date = args[2]; 
				currentTime.month = args[3];
				currentTime.year = args[4];
				currentTime.day = args[5];//06/12/2010
				currentTime.second = 0;
				if(writeRealTime(&currentTime)){	
					sendNODECMD(2);
				}else{
					//writeStringUART("Set Real Time Faul ! \r\n");
				}
				break;
				
			case 3:	//0x03
			    //Lenh set schedule
				// 0 1 2 3 4 5 6 7 8 91011				
				for(i=0;i<9;i++){
					tempSchedule[i][0] = args[i*3];//18
					tempSchedule[i][1] = args[i*3+1];//00
					tempSchedule[i][2] = args[i*3+3];//03
					tempSchedule[i][3] = args[i*3+4];//21
					tempSchedule[i][4] = args[i*3+2];//00
					tempSendTime[i] = ((unsigned int)args[27+(i<<1)]<<8) + args[28+(i<<1)];
				}
				tempSchedule[8][2] = args[0];
				tempSchedule[8][3] = args[1];
								
				if(setSchedule(tempSchedule,tempSendTime)){
					sendNODECMD(3);	
				}else{
					//writeStringUART("Set schedule Faul ! \r\n");	
				}
				
				break;	
	
			case 4:	//0x04
			    //Lenh yeu cau truyen schedule
				getSchedule(tempSchedule,tempSendTime);

				sendNODECMD(4);

				for(i=0;i<9;i++){
					writeUART((char)tempSchedule[i][0]);
					writeUART((char)tempSchedule[i][1]);
					writeUART((char)tempSchedule[i][4]);					
				}
				for(i=0;i<9;i++){
					writeUART((char)(tempSendTime[i]>>8));
					writeUART((char)tempSendTime[i]);			
				}

				break;	
			case 5:	//0x05: Reset system
				systemReset = 1;
				writeUART(5);
				break;
				
			case 6:	//0x06
				// Lenh reset Energy - khong co noi dung
				//resetEnergy();
				sendNODECMD(6);
				break;
		
			case 7:	//0x07
				////Dat cac he so calib E,U,I

				cE = (((unsigned long) args[0]) << 24) + (((unsigned long) args[1]) << 16) + (((unsigned long) args[2]) << 8) + ((unsigned long) args[3]);
				cU = (((unsigned int) args[4]) << 8) + (((unsigned int) args[5]));
				cI = (((unsigned int) args[6]) << 8) + (((unsigned int) args[7]));

				setCalibrationParam(cE , cU , cI);

				sendNODECMD(7);
				
				break;
			case 9:	//0x08
				////Lenh doi ID
				eeprom_write_byte((const uint8_t *)NODEID_ADD,(uint8_t)args[0]);
				sendNODECMD(9);
				writeUART(eeprom_read_byte((const uint8_t *)NODEID_ADD));
				
				break;
			case 10:	//0x09
				////Hoi ID cua node

				sendNODECMD(10);
				writeUART(eeprom_read_byte((const uint8_t *)NODEID_ADD));
				
				break;
			
			default :
				break;	
		}//End switch (command){...
		/*
		cutphase = (100 - DIM)*4;
		old_DIM = DIM;
		if(direct == 0){
			DIM += 1;
			if(DIM > 98){
				direct = 1;
			}
		}else{
			DIM -= 1;
			if(DIM < 2){
				direct = 0;
			}
		}
		*/
		_delay_ms(10);

	}//End while(1){....
					
#endif

return 0;

}//End int main();

//***** Interrupt 0 - Zero Crossing *******************

ISR(INT0_vect){

	if((DIM > 0)&&(DIM < 100)){	
		if(old_DIM == 0){
			PF_COMPEN_BTA_ON;
		}
		open = 1;				
	}else{
		if(DIM == 0){			
		
		}else{
			if(old_DIM == 0){
				PF_COMPEN_BTA_ON;
				BTA_ON;
			}else{
				BTA_ON;		
			}	
		}
	}
}

