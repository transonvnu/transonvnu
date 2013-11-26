#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "t6963c.h"
#include "ds1307.h"
#include <stdio.h>
#include "timming.h"
#include "relays.h"
#include "sysComm.h"
#include "schedule.h"
#include "ade7758.h"
#include "string1.h"
#include "spi.h"
#include "uart.h"
#include "sdcard.h"
//==================================Khai bao FONT ===============================================
#include "font.h"
#include "font1.c"
//#include "font2.c"
#include "font3.c"
#include "font4.c"
#include "icon.c"
//#include "SOAP.h"
#include "tff.h"
#include "MBAT128.h"
//===============================================================================================
#define BackLighOn  PORTC |= 0x80
#define BackLighOff PORTC &= ~0x80
     
#define backLightTimeout    90

#define mainScreen 			        0 
#define menuScreen 			        1
#define settingScreen 		        2
#define scheduleScreen 		        3
#define timeScreen	 		        4
#define gameScreen	 		        5
#define sdcardScreen	 	        6
#define terminalScreen	 	        7
#define lightControlScreen	        8
#define streetLightScheduleScreen   9   //add 11/11/2010
#define	decorateLightScheduleScreen 10	//add 11/11/2010

extern unsigned char RegBytes[10000] __attribute__ ((section (".init1")));
signed char schedule1[14][3];//  __attribute__ ((section (".init1")));
char autoResetModem = 1;
static unsigned char screen,pressButton;
static unsigned char lightStatus,lastlightStatus,currentMenu,lastMenu;
static char key[6];
static unsigned char buttonCmd,argsButtonCmd[2];
TimeStruct   time;
//static char resetE[3];
signed char resetEnergy,loadDefaul,PHCAL1,PHCAL2,PHCAL3,checkConnection,dimmImmediately;
signed char schedule[5][5];
unsigned char changeTimePoiter,changeSchedulePoiter,changeTiPoiter,configPoiter,passSecurity,countPassWord;
unsigned int  TICoef1,TICoef2;
int cU[3],cI[3],pageDisplay;
char dimPoiter,config[36];// __attribute__ ((section (".init1")));
static long  cE[3] __attribute__ ((section (".init1")));
//long  cE[3];
int  tM,tL;
long  E1,E2,E3,A1,A2,A3;// __attribute__ ((section (".init1")));

extern char SERVER_NUMBERS[12];
char realTimeError;
char scheduleError;
char schedule1Error;
volatile unsigned char modemConnected;

unsigned int   time1 = 0;
unsigned int voltageDim[3];

char   measure = 0;

unsigned char dPF[3];//Display Power Factor
unsigned int dU[3],dI[3];//Display U,I,E
unsigned long dE[3];

void initKeyboard(void){
	DDRB |= 0xC0;//PB6 clk, PB7 reset CD4017
	DDRB &= 0xDF;//PB5    in
	//PORTB |= 0x20;
	DDRC |= 0x80;//LCD backLight 
	screen = 0;
	pressButton = 0;
	key[0]=0;key[1]=0;key[2]=0;key[3]=0;key[4]=0;key[5]=0;
	currentMenu=0;
	buttonCmd = 255;
	resetEnergy = -1;
	loadDefaul = -1;
	PHCAL1 = 60;
	PHCAL2 = 60;
	PHCAL3 = 60;
	checkConnection = -1;
	dimmImmediately = -1;
	changeTimePoiter=0;
	changeSchedulePoiter=0;
	changeTiPoiter = 0;
	configPoiter = 0;
	passSecurity = 0;
	countPassWord = 0;
	dimPoiter=0;
	E1=0;E2=0;E3=0;A1=0;A2=0;A3=0;
}
void calibADE(void){
	unsigned char i=0;
	char tBuffer[20];
	//long E10 = 0,E20 = 0,E30 = 0,A10 = 0,A20 = 0,A30 = 0;
	lcd_clear_graph();    	 // clear graphics memory of LCD
	lcd_clear_text();  		 // clear text memory of LCD
	readOffsetVol(1);
	for(i=0;i<3;i++){
		numberToString(readOffsetVol(i+1),tBuffer);
		lcd_print3(5,5+15*i,tBuffer,&f1,0,0);
		numberToString(readVRMS(i+1),tBuffer);
		lcd_print3(65,5+15*i,tBuffer,&f1,0,0);
		numberToString(readOffsetAmp(i+1),tBuffer);
		lcd_print3(125,5+15*i,tBuffer,&f1,0,0);
		numberToString(readIRMS(i+1),tBuffer);
		lcd_print3(185,5+15*i,tBuffer,&f1,0,0);
	}
 	_delay_ms(500);
		
}
void DisplayU_I_P(unsigned int * voltage,unsigned int * current,unsigned char * phase){
    char buffer[12];
	unsigned char i;
	for(i=0;i<3;i++){
		dU[i] = voltage[i]/100;
		dI[i] = current[i]/100;
		dPF[i] = phase[i];
		sprintf_P(buffer,PSTR("%03u.%02u"),voltage[i]/100,voltage[i]%100);
		lcd_print3(34+48*i,17,buffer,&f1,0,0);
		if(current[i] > 0){
			floatToString(current[i],buffer);
			lcd_print3(40+48*i,32,buffer,&f1,0,0);
		}
		else{
			buffer[0] = '0';
			buffer[1] = 0;
			lcd_print3(40+48*i,32,"        ",&f1,0,0);
			lcd_print3(50+48*i,32,buffer,&f1,0,0);
		}		
		
		if(phase[i] > 0){
			sprintf_P(buffer,PSTR("%01u.%02u"),phase[i]/100,phase[i]%100);
			lcd_print3(40+48*i,47,buffer,&f1,0,0);
		}
		else{
			buffer[0] = '0';
			buffer[1] = 0;
			lcd_print3(40+48*i,47,"   ",&f1,0,0);
			lcd_print3(50+48*i,47,buffer,&f1,0,0);
		}	
		
	}
	lcd_line(77,1,77,59,1);
	lcd_line(126,1,126,59,1);
	lcd_line(175,1,175,59,1);		
}
void DisplayE(EnergySruct tEnergy){
	char buffer[12];
	unsigned char i;
	unsigned long totalEnergy;
	for(i=0;i<3;i++){
		if(tEnergy.energy[i] < 0) tEnergy.energy[i] = 0;
		if((tEnergy.energy[i]/1000) >= 100000){
			resetTotalEnergy();
			break;
		}
	}
	dE[0] = tEnergy.energy[0]/100;
	dE[1] = tEnergy.energy[1]/100;
	dE[2] = tEnergy.energy[2]/100;
	totalEnergy = tEnergy.energy[0] + tEnergy.energy[1] + tEnergy.energy[2];
	//numberToString(totalEnergy,buffer);//number long to character array
	floatToString(totalEnergy,buffer);
	//floatToString(88888,buffer);
	lcd_print3(150,62,buffer,&f1,0,0);
	if(getModemConnected())
		lcd_print3(213,117,"!$",&f3,0,0); // Modem already connected
	else
		lcd_print3(213,117,"!&",&f3,0,0); // not connected
	//numberToString(rand(),buffer);	  // test: display a random number in range 0 --> 32768 
	//lcd_print3(100,117,buffer,&f3,0,0); 
    lcd_line(239,1,239,127,1);
}
void WriteArr(char x,char y, char* Arr){
	lcd_print3(x,y,Arr,&f3,0,0);
}
void DisplayNumber(char x,char y, long Number){
	char buffer[12];
	numberToString(Number,buffer);
	lcd_print3(x,y,buffer,&f3,0,0);
}
void DisplaySchedule(signed char schedule[5][5]){
	unsigned char i=0,j=0;//j is x offset
	char buffer[14];	
	for (i = 0; i< 5; i++){				
		sprintf_P(buffer,PSTR("%02uh%02u"),schedule[i][0],schedule[i][1]);
		lcd_print3(8+48*i,92,buffer,&f3,0,0);
		switch (schedule[i][4]){
			case 0:
				sprintf_P(buffer,PSTR("OFF "));
				j = 12;
				break;
			case 1:
				sprintf(buffer,"ON30%% ");
				j = 7;
				break;
			case 2:
				sprintf(buffer,"ON70%% ");
				j = 7;
				break;
			case 3:
				sprintf(buffer,"ON100%%");
				j = 3;
				break; 
			default:
				break;
		}	
		lcd_print3(j+48*i,105,buffer,&f3,0,0);
		lcd_line(0,0,0,127,1);//redaw schedule box
		lcd_line(48,90,48,116,1);
		lcd_line(96,90,96,116,1);
		lcd_line(144,90,144,116,1);
		lcd_line(192,90,192,116,1);
		lcd_line(239,1,239,127,1);
		
	}

}
void DisplayLightStatus(unsigned char status){
	lightStatus = status;
	switch (status){
		case 0:
			lcd_print3(180,19," OFF ",&f4,0,0);
			lcd_print3(180,37,"        ",&f4,0,0);
			break;
		case 1:
			lcd_print3(185,19," ON ",&f4,0,0);
			lcd_print3(180,37," 30% ",&f4,0,0);
			break;
		case 2:
			lcd_print3(185,19," ON ",&f4,0,0);
			lcd_print3(180,37," 70% ",&f4,0,0);
			break;
		case 3:
			lcd_print3(185,19," ON ",&f4,0,0);
			lcd_print3(180,37,"100% ",&f4,0,0);
			break;
		default:
			break;
	}
	lcd_line(239,1,239,127,1);
}
void DisplayDecorateLightStatus(unsigned char status){
	if(status == 1){
		lcd_print3(190,77," ON ",&f1,0,0);
		//lcd_print3(180,37,"        ",&f4,0,0);
		//lcd_print3(4,77
	}else{
		lcd_print3(190,77," OFF",&f1,0,0);
		//lcd_print3(180,37,"        ",&f4,0,0);
	}
}
void DisplayTime(TimeStruct * time){
	char buffer[16];
	sprintf_P(buffer,PSTR("%02u : %02u : %02u"),time->hour,time->minute,time->second);
	lcd_print3(180,3,buffer,&f3,0,0);
	lcd_line(239,1,239,127,1);
}
unsigned char getScreen(void){
	return screen;
}

void terminal(char * command){

	unsigned long Pow[3];
	if(screen != terminalScreen){
		screen = terminalScreen;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		lcd_print3(5,20,"Auto Reset Modem :",&f3,0,0);
		//Add 05/08/2010 ************************
		lcd_print3(50,40,"Power(W)",&f3,0,0);
		lcd_print3(150,40,"Energy(kWh)",&f3,0,0);
		lcd_print3(5,60,"Phase1:",&f3,0,0);
		lcd_print3(5,80,"Phase2:",&f3,0,0);
		lcd_print3(5,100,"Phase3:",&f3,0,0);
		Pow[0] = dU[0]*dI[0];
		Pow[1] = dU[1]*dI[1];
		Pow[2] = dU[2]*dI[2];
		Pow[0] = (Pow[0]*dPF[0])/100;
		Pow[1] = (Pow[1]*dPF[1])/100;
		Pow[2] = (Pow[2]*dPF[2])/100;
		DisplayNumber(50,60,Pow[0]);
		DisplayNumber(50,80,Pow[1]);
		DisplayNumber(50,100,Pow[2]);
		DisplayNumber(150,60,dE[0]);
		DisplayNumber(150,80,dE[1]);
		DisplayNumber(150,100,dE[2]);
		//***************************************
		if(autoResetModem == 1){
			lcd_print4(116,20,"YES",&f3,0,0);
		}
		else{
			lcd_print4(116,20,"NO",&f3,0,0);
		}
	}
	else {
		if(key[0]){
			autoResetModem = -1*autoResetModem;
			key[0] = 0;
		}
		if(key[1]){
			autoResetModem = -1*autoResetModem;
			key[1] = 0;
		}
		if(autoResetModem == 1){
			lcd_print4(116,20,"YES",&f3,0,0);
		}
		else{
			lcd_print4(116,20,"NO  ",&f3,0,0);
		}
		
	}
}
void changeSchedule(char * command){
	char buffer[27];
	if(screen != scheduleScreen){
		screen = scheduleScreen;
		changeSchedulePoiter = 0;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		sprintf_P(buffer,PSTR("CHANGE SCHEDULE"));
		lcd_print3(60,4,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("STREET LIGHT SCHEDULE"));
		lcd_print4(32,32,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("DECORATE LIGHT SCHEDULE"));
		lcd_print3(32,45,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("OK"));
		lcd_print3(210,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("BACK"));
		lcd_print3(10,114,buffer,&f1,0,0);
	}
	else {
		if(key[1]){//Righ
			key[1] = 0;
			if(changeSchedulePoiter == 1) changeSchedulePoiter = 0;
			else changeSchedulePoiter = 1;
		}
		if(key[0]){//Left
			key[0] = 0;
			if(changeSchedulePoiter == 1) changeSchedulePoiter = 0;
			else changeSchedulePoiter = 1;
		}
		if(key[3]){//Down
			key[3] = 0;
			if(changeSchedulePoiter == 1) changeSchedulePoiter = 0;
			else changeSchedulePoiter = 1;
		}
		if(key[2]){//Up
			key[2] = 0;
			if(changeSchedulePoiter == 1) changeSchedulePoiter = 0;
			else changeSchedulePoiter = 1;
		}
		if(changeSchedulePoiter == 0){
			sprintf_P(buffer,PSTR("STREET LIGHT SCHEDULE"));
			lcd_print4(32,32,buffer,&f1,0,0);
			sprintf_P(buffer,PSTR("DECORATE LIGHT SCHEDULE"));
			lcd_print3(32,45,buffer,&f1,0,0);
		}else{
			sprintf_P(buffer,PSTR("STREET LIGHT SCHEDULE"));
			lcd_print3(32,32,buffer,&f1,0,0);
			sprintf_P(buffer,PSTR("DECORATE LIGHT SCHEDULE"));
			lcd_print4(32,45,buffer,&f1,0,0);
		}
		if(key[4]){
			key[4] = 0;
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);
			buttonCmd = 3;
			if(changeSchedulePoiter == 0){
				changeStreetLightSchedule(key);
			}else{
				changeDecorateLightSchedule(key);
			}
		}
	}
}
void changeDecorateLightSchedule(char * command){
	unsigned char i,j;
	char buffer[45];
	if(screen != decorateLightScheduleScreen){
		screen = decorateLightScheduleScreen;
		changeSchedulePoiter = 0;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		sprintf_P(buffer,PSTR("CHANGE DECORATE LIGHT SCHEDULE"));
		lcd_print3(20,4,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("OK"));
		lcd_print3(210,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("BACK"));
		lcd_print3(10,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("Date"));
		lcd_print3(10,20,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("State"));
		lcd_print3(57,20,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("ON"));
		lcd_print3(120,20,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("OFF"));
		lcd_print3(180,20,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("MO"));
		lcd_print3(14,32,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("TU"));
		lcd_print3(14,44,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("WE"));
		lcd_print3(14,56,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("TH"));
		lcd_print3(14,68,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("FR"));
		lcd_print3(14,80,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SA"));
		lcd_print3(14,92,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SU"));
		lcd_print3(14 ,104,buffer,&f3,0,0);
		for (i = 0; i< 9; i++){	
			lcd_line(5,19+12*i,220,19+12*i,1);
		}
		lcd_line(5,19,5,115,1);
		lcd_line(220,19,220,115,1);
		lcd_line(37,19,37,115,1);
		lcd_line(98,19,98,115,1);
		lcd_line(159,19,159,115,1);
		readEEPROM(buffer,131,172);
		for (i = 0; i< 14; i++){
			for (j = 0; j< 3; j++){
				schedule1[i][j] = buffer[i*3 + j];
			}	
		}	
		for (i = 0; i< 7; i++){				
			sprintf(buffer,"%02u",schedule1[2*i][0]);//gio i
			lcd_print3(104,32+12*i,buffer,&f3,0,0);
			lcd_print3(124,32+12*i,"h",&f3,0,0);
			sprintf(buffer,"%02u",schedule1[2*i][1]);//phut i
			lcd_print3(138,32+12*i,buffer,&f3,0,0);

			sprintf(buffer,"%02u",schedule1[2*i+1][0]);//gio i
			lcd_print3(166,32+12*i,buffer,&f3,0,0);
			lcd_print3(186,32+12*i,"h",&f3,0,0);
			sprintf(buffer,"%02u",schedule1[2*i+1][1]);//phut i
			lcd_print3(200,32+12*i,buffer,&f3,0,0);

			switch (schedule1[2*i][2]){
				case 0:
					sprintf_P(buffer,PSTR("OFF "));
					break;
				case 1:
					sprintf(buffer,"ON  ");
					break;
				default:
					break;
			}
			lcd_print3(56,32+12*i,buffer,&f3,0,0);
		}
				
	}
	else {
		if(key[1]){//Righ
			changeSchedulePoiter++;
			key[1] = 0;
			if(changeSchedulePoiter >= 35) changeSchedulePoiter = 0;
		}
		if(key[0]){//Left
			changeSchedulePoiter--;
			key[0] = 0;
			if(changeSchedulePoiter == 255) changeSchedulePoiter = 34;
		}
		if(key[3]){//Down
			for(i=0;i<7;i++){
				if(i*5 == changeSchedulePoiter){
					if(schedule1[i*2][2] == 0)	schedule1[i*2][2] = 1;
					else schedule1[i*2][2] = 0;
				}
				if(i*5+1 == changeSchedulePoiter){
					schedule1[2*i][0] -= 1;
					if(schedule1[2*i][0] <= -1) schedule1[2*i][0] = 23;
				}
				if(i*5+2 == changeSchedulePoiter){
					schedule1[2*i][1] -= 1;
					if(schedule1[2*i][1] <= -1) schedule1[2*i][1] = 59;
				}
				if(i*5+3 == changeSchedulePoiter){
					schedule1[2*i+1][0] -= 1;
					if(schedule1[2*i+1][0] <= -1) schedule1[2*i+1][0] = 23;
				}
				if(i*5+4 == changeSchedulePoiter){
					schedule1[2*i+1][1] -= 1;
					if(schedule1[2*i+1][1] <= -1) schedule1[2*i+1][1] = 59;
				}
			}
			key[3] = 0;
		}
		if(key[2]){//Up
			for(i=0;i<7;i++){
				if(i*5 == changeSchedulePoiter){
					if(schedule1[i*2][2] == 0)	schedule1[i*2][2] = 1;
					else schedule1[i*2][2] = 0;
				}
				if(i*5+1 == changeSchedulePoiter){
					schedule1[2*i][0] += 1;
					if(schedule1[2*i][0] >= 24) schedule1[2*i][0] = 0;
				}
				if(i*5+2 == changeSchedulePoiter){
					schedule1[2*i][1] += 1;
					if(schedule1[2*i][1] >= 60) schedule1[2*i][1] = 0;
				}
				if(i*5+3 == changeSchedulePoiter){
					schedule1[2*i+1][0] += 1;
					if(schedule1[2*i+1][0] >= 24) schedule1[2*i+1][0] = 0;
				}
				if(i*5+4 == changeSchedulePoiter){
					schedule1[2*i+1][1] += 1;
					if(schedule1[2*i+1][1] >= 60) schedule1[2*i+1][1] = 0;
				}
			}
			key[2] = 0;
		}
		for (i = 0; i< 7; i++){				
			sprintf(buffer,"%02u",schedule1[2*i][0]);//gio i
			if((i*5+1) == changeSchedulePoiter) 
				lcd_print4(104,32+12*i,buffer,&f3,0,0);
			else
				lcd_print3(104,32+12*i,buffer,&f3,0,0);
			sprintf(buffer,"%02u",schedule1[2*i][1]);//phut i
			if((i*5+2) == changeSchedulePoiter) 
				lcd_print4(138,32+12*i,buffer,&f3,0,0);
			else  
				lcd_print3(138,32+12*i,buffer,&f3,0,0);
			sprintf(buffer,"%02u",schedule1[2*i+1][0]);//gio i
			if((i*5+3) == changeSchedulePoiter) 
				lcd_print4(166,32+12*i,buffer,&f3,0,0);
			else
				lcd_print3(166,32+12*i,buffer,&f3,0,0);
			sprintf(buffer,"%02u",schedule1[2*i+1][1]);//phut i
			if((i*5+4) == changeSchedulePoiter) 
				lcd_print4(200,32+12*i,buffer,&f3,0,0);
			else  
				lcd_print3(200,32+12*i,buffer,&f3,0,0);
			switch (schedule1[2*i][2]){
				case 0:
					sprintf_P(buffer,PSTR("OFF "));
					break;
				case 1:
					sprintf(buffer,"ON   ");
					break;
				default:
					break;
			}
			if((i*5) == changeSchedulePoiter) 
				lcd_print4(56,32+12*i,buffer,&f3,0,0);
			else
				lcd_print3(56,32+12*i,buffer,&f3,0,0);
				
		}
		if(key[4]){
			key[4] = 0;
			for(i = 0;i<7;i++){
				schedule1[2*i+1][2] = 0;
			}
			if(setSchedule1(schedule1)){
				schedule1Error = 0;
				buttonCmd = 4;
				messageOK();
			}else{
				screen = 251;
				messageErrorSchedule();
			}
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);
		}
	}
}

void changeStreetLightSchedule(char * command){
	unsigned char i;
	char buffer[25];
	unsigned int sendStateTime[5];
	if(screen != streetLightScheduleScreen){
		screen = streetLightScheduleScreen;
		changeSchedulePoiter = 0;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		//lcd_print3(60,4,"CHANGE SCHEDULE",&f1,0,0);
		sprintf_P(buffer,PSTR("CHANGE SCHEDULE"));
		lcd_print3(60,4,buffer,&f1,0,0);
		//lcd_print3(210,114,"OK",&f1,0,0);
		sprintf_P(buffer,PSTR("OK"));
		lcd_print3(210,114,buffer,&f1,0,0);
		//lcd_print3(10,114,"BACK",&f1,0,0);
		sprintf_P(buffer,PSTR("BACK"));
		lcd_print3(10,114,buffer,&f1,0,0);
		//lcd_print3(62,22,"TIME",&f3,0,0);
		sprintf_P(buffer,PSTR("TIME"));
		lcd_print3(62,22,buffer,&f1,0,0);
		//lcd_print3(148,22,"STATE",&f3,0,0);
		sprintf_P(buffer,PSTR("STATE"));
		lcd_print3(148,22,buffer,&f1,0,0);
		getSchedule(schedule,sendStateTime);
		for (i = 0; i< 5; i++){				
			sprintf(buffer,"%02u",schedule[i][0]);//gio i
			lcd_print3(58,38+15*i,buffer,&f3,0,0);
			lcd_print3(73,38+15*i,"h",&f3,0,0);
			sprintf(buffer,"%02u",schedule[i][1]);//phut i
			lcd_print3(82,38+15*i,buffer,&f3,0,0);
			switch (schedule[i][4]){
				case 0:
					sprintf_P(buffer,PSTR("OFF    "));
					break;
				case 1:
					sprintf(buffer,"ON30%% ");
					break;
				case 2:
					sprintf(buffer,"ON70%% ");
					break;
				case 3:
					sprintf(buffer,"ON100%%");
					break; 
				default:
					break;
			}
			lcd_print3(146,38+15*i,buffer,&f3,0,0);
			sprintf_P(buffer,PSTR("%04u"),sendStateTime[i]);
			lcd_print3(200,38+15*i,buffer,&f3,0,0);
		}
		sprintf(buffer,"%02u",schedule[0][0]);//gio 0
		lcd_print4(58,38,buffer,&f3,0,0);
		for(i=0;i<7;i++){
			lcd_line(50,20+15*i,230,20+15*i,1); //Ve Box
			if((i>0)&&(i<6)){//Ve mui ten
				lcd_line(135,24+15*i,140,27+15*i,1);
				lcd_line(100,27+15*i,140,27+15*i,1);
				lcd_line(135,30+15*i,140,27+15*i,1);
			}
		}
		lcd_line(50,20,50,110,1);//Ve Box
		lcd_line(230,20,230,110,1);
	}
	else {
		if(key[1]){//Righ
			changeSchedulePoiter++;
			key[1] = 0;
			if(changeSchedulePoiter >= 15) changeSchedulePoiter = 0;
		}
		if(key[0]){//Left
			changeSchedulePoiter--;
			key[0] = 0;
			if(changeSchedulePoiter == 255) changeSchedulePoiter = 14;
		}
		if(key[3]){//Down
			switch (changeSchedulePoiter){
				case 0:
					schedule[0][0] -= 1;
					if(schedule[0][0] <= -1) schedule[0][0] = 23;
					break;
				case 1:
					schedule[0][1] -= 1;
					if(schedule[0][1] <= -1) schedule[0][1] = 59;
					break;
				case 2:
					schedule[0][4] -= 1;
					if(schedule[0][4] <= -1) schedule[0][4] = 3;
					break;
				case 3:
					schedule[1][0] -= 1;
					if(schedule[1][0] <= -1) schedule[1][0] = 23;
					break;
				case 4:
					schedule[1][1] -= 1;
					if(schedule[1][1] <= -1) schedule[1][1] = 59;
					break;
				case 5:
					schedule[1][4] -= 1;
					if(schedule[1][4] <= -1) schedule[1][4] = 3;
					break;
				case 6:
					schedule[2][0] -= 1;
					if(schedule[2][0] <= -1) schedule[2][0] = 23;
					break;
				case 7:
					schedule[2][1] -= 1;
					if(schedule[2][1] <= -1) schedule[2][1] = 59;
					break;
				case 8:
					schedule[2][4] -= 1;
					if(schedule[2][4] <= -1) schedule[2][4] = 3;
					break;
				case 9:
					schedule[3][0] -= 1;
					if(schedule[3][0] <= -1) schedule[3][0] = 23;
					break;
				case 10:
					schedule[3][1] -= 1;
					if(schedule[3][1] <= -1) schedule[3][1] = 59;
					break;
				case 11:
					schedule[3][4] -= 1;
					if(schedule[3][4] <= -1) schedule[3][4] = 3;
					break;
				case 12:
					schedule[4][0] -= 1;
					if(schedule[4][0] <= -1) schedule[4][0] = 23;
					break;
				case 13:
					schedule[4][1] -= 1;
					if(schedule[4][1] <= -1) schedule[4][1] = 59;
					break;
				case 14:
					schedule[4][4] -= 1;
					if(schedule[4][4] <= -1) schedule[4][4] = 3;
					break;
				default : 	
					break;
			}
			key[3] = 0;
		}
		if(key[2]){//Up
			switch (changeSchedulePoiter){
				case 0:
					schedule[0][0] += 1;
					if(schedule[0][0] >= 24) schedule[0][0] = 0;
					break;
				case 1:
					schedule[0][1] += 1;
					if(schedule[0][1] >= 60) schedule[0][1] = 0;
					break;
				case 2:
					schedule[0][4] += 1;
					if(schedule[0][4] >= 4) schedule[0][4] = 0;
					break;
				case 3:
					schedule[1][0] += 1;
					if(schedule[1][0] >= 24) schedule[1][0] = 0;
					break;
				case 4:
					schedule[1][1] += 1;
					if(schedule[1][1] >= 60) schedule[1][1] = 0;
					break;
				case 5:
					schedule[1][4] += 1;
					if(schedule[1][4] >= 4) schedule[1][4] = 0;
					break;
				case 6:
					schedule[2][0] += 1;
					if(schedule[2][0] >= 24) schedule[2][0] = 0;
					break;
				case 7:
					schedule[2][1] += 1;
					if(schedule[2][1] >= 60) schedule[2][1] = 0;
					break;
				case 8:
					schedule[2][4] += 1;
					if(schedule[2][4] >= 4) schedule[2][4] = 0;
					break;
				case 9:
					schedule[3][0] += 1;
					if(schedule[3][0] >= 24) schedule[3][0] = 0;
					break;
				case 10:
					schedule[3][1] += 1;
					if(schedule[3][1] >= 60) schedule[3][1] = 0;
					break;
				case 11:
					schedule[3][4] += 1;
					if(schedule[3][4] >= 4) schedule[3][4] = 0;
					break;
				case 12:
					schedule[4][0] += 1;
					if(schedule[4][0] >= 24) schedule[4][0] = 0;
					break;
				case 13:
					schedule[4][1] += 1;
					if(schedule[4][1] >= 60) schedule[4][1] = 0;
					break;
				case 14:
					schedule[4][4] += 1;
					if(schedule[4][4] >= 4) schedule[4][4] = 0;
					break;
				default : 	
					break;
			}
			key[2] = 0;
		}
		for (i = 0; i< 5; i++){				
			sprintf(buffer,"%02u",schedule[i][0]);//gio i
			if((i*3) == changeSchedulePoiter) 
				lcd_print4(58,38+15*i,buffer,&f3,0,0);
			else
				lcd_print3(58,38+15*i,buffer,&f3,0,0);
			sprintf(buffer,"%02u",schedule[i][1]);//phut i
			if((i*3+1) == changeSchedulePoiter) 
				lcd_print4(82,38+15*i,buffer,&f3,0,0);
			else  
				lcd_print3(82,38+15*i,buffer,&f3,0,0);
			switch (schedule[i][4]){
				case 0:
					sprintf_P(buffer,PSTR("OFF       "));
					break;
				case 1:
					sprintf(buffer,"ON30%% ");
					break;
				case 2:
					sprintf(buffer,"ON70%% ");
					break;
				case 3:
					sprintf(buffer,"ON100%%");
					break; 
				default:
					break;
			}
			if((i*3+2) == changeSchedulePoiter) 
				lcd_print4(146,38+15*i,buffer,&f3,0,0);
			else
				lcd_print3(146,38+15*i,buffer,&f3,0,0);
				
		}
		if(key[4]){
			key[4] = 0;
			for(i=0;i<4;i++){
				schedule[i][2] =  schedule[i+1][0];
				schedule[i][3] =  schedule[i+1][1];
			}
			schedule[4][2] =  schedule[0][0];
			schedule[4][3] =  schedule[0][1];
			readEEPROM(buffer,76,85);
			for(i=0;i<5;i++){
				sendStateTime[i] = (unsigned char)buffer[2*i];
				sendStateTime[i] <<= 8;
				sendStateTime[i] |= (unsigned char)buffer[2*i+1];
			}
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);
			if(setSchedule(schedule,sendStateTime)){
				scheduleError = 0;
				buttonCmd = 3;
				messageOK();
			}else{
				screen = 254;
				messageErrorSchedule();
			}
		}
	}
}
void convertDay(char day,char * buffer){
	switch 	(day){
		case 0:
			sprintf_P(buffer,PSTR("Ko"));
			break;
		case 1:
			sprintf_P(buffer,PSTR("MO"));
			break;
		case 2:
			sprintf_P(buffer,PSTR("TU"));
			break;
		case 3:
			sprintf_P(buffer,PSTR("WE"));
			break;
		case 4:
			sprintf_P(buffer,PSTR("TH"));
			break;
		case 5:
			sprintf_P(buffer,PSTR("FR"));
			break;
		case 6:
			sprintf_P(buffer,PSTR("SA"));
			break;
		case 7:
			sprintf_P(buffer,PSTR("SU"));
			break;
		default:
			break;
	}
}
void changeTimeDate(char * command){//Edit 22/11/2010 add day
	char buffer[32];
	signed char tTasksTable[5][5];
	unsigned char i;
	unsigned int tempSendTime[5];
	TimeStruct time1, time2;
	if(screen != timeScreen){
		screen = timeScreen;
		changeTimePoiter = 0;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		readRealTime(&time);

		sprintf_P(buffer,PSTR("SET TIME/DATE"));
		lcd_print3(80,4,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("OK"));
		lcd_print3(210,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("BACK"));
		lcd_print3(10,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("HOUR : MINUTE"));
		lcd_print3(48,40,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("DAY : DATE : MONTH : YEAR"));
		lcd_print3(48,68,buffer,&f1,0,0);		

		sprintf(buffer,"%02u",time.hour);
		lcd_print4(56,52,buffer,&f1,0,0);
		lcd_print3(72,52," : ",&f1,0,0);
		sprintf(buffer,"%02u",time.minute);
		lcd_print3(89,52,buffer,&f1,0,0);
		convertDay(time.day,buffer);
		//sprintf(buffer,"%02u",time.day);
		lcd_print3(57,80,buffer,&f1,0,0);
		lcd_print3(80,80," / ",&f1,0,0);
		sprintf(buffer,"%02u",time.date);
		lcd_print3(97,80,buffer,&f1,0,0);
		lcd_print3(112,80," / ",&f1,0,0);
		sprintf(buffer,"%02u",time.month);
		lcd_print3(130,80,buffer,&f1,0,0);
		lcd_print3(145,80," / ",&f1,0,0);
		sprintf(buffer,"%02u",time.year);
		lcd_print3(162,80,buffer,&f1,0,0);
	}
	else {
		if(key[1]){//Righ
			changeTimePoiter++;
			key[1] = 0;
			if(changeTimePoiter >= 6) changeTimePoiter = 0;
		}
		if(key[0]){//Left
			changeTimePoiter--;
			key[0] = 0;
			if(changeTimePoiter == 255) changeTimePoiter = 5;
		}
		if(key[3]){//Down
			switch (changeTimePoiter){
				case 0:
					time.hour -= 1;
					if(time.hour <= -1) time.hour = 23;
					break;
				case 1:
					time.minute -= 1;
					if(time.minute <= -1) time.minute = 59;
					break;
				case 2:
					time.day -= 1;
					if(time.day <= 0) time.day = 7;
					break;
				case 3:
					time.date -= 1;
					if(time.date <= 0) time.date = 31;
					break;
				case 4:
					time.month -= 1;
					if(time.month <= 0) time.month = 12;
					break;
				case 5:
					time.year -= 1;
					if(time.year <= -1) time.year = 50;
					break;
				default : 	
					break;
			}
			key[3] = 0;
		}
		if(key[2]){//Up
			switch (changeTimePoiter){
				case 0:
					time.hour += 1;
					if(time.hour >= 24) time.hour = 0;
					break;
				case 1:
					time.minute += 1;
					if(time.minute >= 60) time.minute = 0;
					break;
				case 2:
					time.day += 1;
					if(time.day >= 8) time.day = 1;
					break;
				case 3:
					time.date += 1;
					if(time.date >= 32) time.date = 1;
					break;
				case 4:
					time.month += 1;
					if(time.month >= 13) time.month = 1;
					break;
				case 5:
					time.year += 1;
					if(time.year >= 51) time.year = 0;
					break;
				default : 	
					break;
			}
			key[2] = 0;
		}
		if(changeTimePoiter==0){//hour
			sprintf(buffer,"%02u",time.year);
			lcd_print3(162,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.minute);
			lcd_print3(89,52,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.hour);
			lcd_print4(56,52,buffer,&f1,0,0);			
		}
		if(changeTimePoiter==1){//minute
			sprintf(buffer,"%02u",time.hour);
			lcd_print3(56,52,buffer,&f1,0,0);
			convertDay(time.day,buffer);
			lcd_print3(57,80,buffer,&f1,0,0);		
			sprintf(buffer,"%02u",time.minute);
			lcd_print4(89,52,buffer,&f1,0,0);
		}
		if(changeTimePoiter==2){//day
			sprintf(buffer,"%02u",time.minute);
			lcd_print3(89,52,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.date);
			lcd_print3(97,80,buffer,&f1,0,0);
			convertDay(time.day,buffer);
			lcd_print4(57,80,buffer,&f1,0,0);
		}
		if(changeTimePoiter==3){//date
			convertDay(time.day,buffer);
			lcd_print3(57,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.month);
			lcd_print3(130,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.date);
			lcd_print4(97,80,buffer,&f1,0,0);
		}
		if(changeTimePoiter==4){//month
			sprintf(buffer,"%02u",time.date);
			lcd_print3(97,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.year);
			lcd_print3(162,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.month);
			lcd_print4(130,80,buffer,&f1,0,0);
		}
		if(changeTimePoiter==5){//year
			sprintf(buffer,"%02u",time.month);
			lcd_print3(130,80,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.hour);
			lcd_print3(56,52,buffer,&f1,0,0);
			sprintf(buffer,"%02u",time.year);
			lcd_print4(162,80,buffer,&f1,0,0);
		}
		if(key[4]){ // OK
			key[4] = 0;
			time.second = 0;
			time.day = 0;
			if(writeRealTime(&time)){
				realTimeError = 0;//Reset Error
				getSchedule(tTasksTable,tempSendTime);
				for (i = 0; i< 5; i++){
				    time1.hour = tTasksTable[i][0];
				    time1.minute = tTasksTable[i][1];
				    time2.hour = tTasksTable[i][2];
				    time2.minute = tTasksTable[i][3];
				    if (isInTimeSlot(&time, &time1, &time2)){
				    	setCurrentTask(i);
				        break;
			        }
		        }
				buttonCmd = 2;
				messageOK();
			}else{
				messageERROR();
			}
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);			
		}
	}

}
void sdCard(char * command){
	int i;
	char buffer[20];
	unsigned int PORT,ID;
	unsigned char temp;
	unsigned long free = 0,total = 0,user = 0;
	if(screen != sdcardScreen){

		screen = sdcardScreen;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		
		sprintf_P(buffer,PSTR("SET PORT:"));
		lcd_print3(5,20,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SET ID:"));
		lcd_print3(5,35,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SET SV IP:"));
		lcd_print3(5,50,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SET SV NUM:"));
		lcd_print3(5,65,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("SD CARD INFO:"));
		lcd_print3(5,80,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("TOTAL:"));
		lcd_print3(50,95,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("FREE:"));
		lcd_print3(50,110,buffer,&f3,0,0);
		total = checkCapacitor(&free,&user);
		if(total < 10){
			sprintf_P(buffer,PSTR("NOT INSERTED"));
			lcd_print3(90,80,buffer,&f3,0,0);	
		}else{
			sprintf_P(buffer,PSTR("INSERTED"));
			lcd_print3(90,80,buffer,&f3,0,0);
		}
		//ltoa(total/1024,buffer,10);
		dwordToString(total/1024,buffer);
		lcd_print3(90,95,buffer,&f3,0,0);
		//ltoa(free/1024,buffer,10);
		dwordToString(free/1024,buffer);
		lcd_print3(90,110,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("kB"));
		lcd_print3(150,95,buffer,&f3,0,0);
		sprintf_P(buffer,PSTR("kB"));
		lcd_print3(150,110,buffer,&f3,0,0);

		readEEPROM(config,86,118);
		for(i=0;i<33;i++){
			if((config[i] < 0)||(config[i] > 9)){
				config[0] = 0;config[1] = 4;config[2] = 0;config[3] = 0;config[4] = 0;//Default PORT: 4000
				config[5] = 0;config[6] = 0;config[7] = 0;config[8] = 1;config[9] = 0;//Default ID: 10
				config[10] = 2;config[11] = 0;config[12] = 3;//203.
				config[13] = 1;config[14] = 1;config[15] = 3;//113.
				config[16] = 1;config[17] = 7;config[18] = 3;//173.
				config[19] = 0;config[20] = 1;config[21] = 2;//012
				config[22] = 0;config[23] = 0;config[24] = 9;config[25] = 1;config[26] = 5;config[27] = 8;
				config[28] = 8;config[29] = 8;config[30] = 8;config[31] = 8;config[32] = 8;//00915888888
				writeEEPROM(config,86,118);
				break;
			}
		}
		for(i=0;i<5;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(80+8*i,20,buffer,&f3,0,0);
		}
		for(i=5;i<10;i++){	
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(80+8*(i-5),35,buffer,&f3,0,0);
		}
		for(i=10;i<13;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(80+8*(i-10),50,buffer,&f3,0,0);
		}
		lcd_print3(80+8*(i-10),50,".",&f3,0,0);
		for(i=13;i<16;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(88+8*(i-10),50,buffer,&f3,0,0);
		}
		lcd_print3(88+8*(i-10),50,".",&f3,0,0);
		for(i=16;i<19;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(96+8*(i-10),50,buffer,&f3,0,0);
		}
		lcd_print3(96+8*(i-10),50,".",&f3,0,0);
		for(i=19;i<22;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(104+8*(i-10),50,buffer,&f3,0,0);
		}	
		for(i=22;i<33;i++){
			sprintf(buffer,"%01u",config[i]);
			lcd_print3(80+8*(i-22),65,buffer,&f3,0,0);
		}	
		lcd_print3(10,114,"BACK",&f1,0,0);
		lcd_print3(210,114,"OK",&f1,0,0);

		configPoiter = 0;
		passSecurity = 0;
		countPassWord = 0;
	}
	else {
		if(passSecurity == 1){
			if(key[0]){//Left
				configPoiter--;
				key[0] = 0;
				if(configPoiter == 255) configPoiter = 32;
			}
			if(key[1]){//Righ
				configPoiter++;
				key[1] = 0;
				if(configPoiter >= 33) configPoiter = 0;
			}
			if(key[3]){//Down
				config[configPoiter] -= 1;
				if((configPoiter==10)||(configPoiter==13)||(configPoiter==16)||(configPoiter==19)){
					if(config[configPoiter] < 0) config[configPoiter] = 2;	
				}
				else{
					if(config[configPoiter] < 0) config[configPoiter] = 9;
				}	
				key[3] = 0;
			}
			if(key[2]){//Up
				config[configPoiter] += 1;
				if((configPoiter==10)||(configPoiter==13)||(configPoiter==16)||(configPoiter==19)){
					if(config[configPoiter] > 2) config[configPoiter] = 0;	
				}
				else{
					if(config[configPoiter] > 9) config[configPoiter] = 0;
				}	
				key[2] = 0;
			}
			for(i=0;i<5;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(80+8*i,20,buffer,&f3,0,0);
				else
					lcd_print3(80+8*i,20,buffer,&f3,0,0);
			}
			for(i=5;i<10;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(80+8*(i-5),35,buffer,&f3,0,0);
				else
					lcd_print3(80+8*(i-5),35,buffer,&f3,0,0);
			}
			for(i=10;i<13;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(80+8*(i-10),50,buffer,&f3,0,0);
				else
					lcd_print3(80+8*(i-10),50,buffer,&f3,0,0);
			}
			for(i=13;i<16;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(88+8*(i-10),50,buffer,&f3,0,0);
				else
					lcd_print3(88+8*(i-10),50,buffer,&f3,0,0);
			}
			for(i=16;i<19;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(96+8*(i-10),50,buffer,&f3,0,0);
				else
					lcd_print3(96+8*(i-10),50,buffer,&f3,0,0);
			}
			for(i=19;i<22;i++){
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(104+8*(i-10),50,buffer,&f3,0,0);
				else
					lcd_print3(104+8*(i-10),50,buffer,&f3,0,0);
			}
			for(i=22;i<33;i++){//server's numbers
				sprintf(buffer,"%01u",config[i]);
				if(i == configPoiter)
					lcd_print4(80+8*(i-22),65,buffer,&f3,0,0);
				else
					lcd_print3(80+8*(i-22),65,buffer,&f3,0,0);
				
			}
			
			if(key[4]){ // OK
				key[4] = 0;
				PORT = (unsigned int)config[0]*10000+(unsigned int)config[1]*1000
			 			+ (unsigned int)config[2]*100  +(unsigned int)config[3]*10 + (unsigned int)config[4];
				ID   = (unsigned int)config[5]*10000+(unsigned int)config[6]*1000
			 			+ (unsigned int)config[7]*100  +(unsigned int)config[8]*10 + (unsigned int)config[9];
				writeEEPROM(config,86,118);

				wordToString(PORT,buffer);
				writeStringUART2("AT+Setsvport=\"");
				writeMStringUART2(buffer);
				writeStringUART2("\"\r\n");	
				lcd_print3(5,80,buffer,&f3,0,0);
				_delay_ms(100);
				wordToString(ID,buffer);
				writeStringUART2("AT+SetID=\"");
				writeMStringUART2(buffer);
				writeStringUART2("\"\r\n");
				_delay_ms(100);	
				writeStringUART2("AT+Connect=\"");
				for(i=0;i<4;i++){
					temp = (unsigned char)config[i*3+10]*100+(unsigned char)config[i*3+11]*10+(unsigned char)config[i*3+12];
					byteToString(temp,buffer);
					writeMStringUART2(buffer);	
					if(i<3)
						writeUART2('.');						
				}
				writeStringUART2("\"\r\n");
				//writeEEPROM(config,86,118);
				if(config[23] == 0){
					for(i=23;i<33;i++){	
						SERVER_NUMBERS[i-23] = config[i] + '0';
					}
					SERVER_NUMBERS[10] = 0;//end of string
				}
				else{
					for(i=22;i<33;i++){	
						SERVER_NUMBERS[i-22] = config[i] + '0';
					}
					SERVER_NUMBERS[11] = 0;//end of string
				}
				_delay_ms(1000);
				lcd_print3(210,114,"     ",&f1,0,0);
				lcd_print3(211,115,"OK",&f1,0,0);
				_delay_ms(100);
				lcd_print3(211,115,"     ",&f1,0,0);
				lcd_print3(210,114,"OK",&f1,0,0);
				modemConnected = 0;
				DDRB |= 0x20;	
				_delay_ms(50);
				modem_on_off(0);
				_delay_ms(1500);
				_delay_ms(1500);
				modem_on_off(1);
				_delay_ms(500);
				DDRB &= 0xDF;	
			}
    	}
		else {//check security password
			// ***** U U D D Enter ******************************
			if(key[2]){ //up
				key[2] = 0;
				if((countPassWord == 0)||(countPassWord == 1))
					countPassWord++;
				else
					countPassWord = 0;
			}
			if(key[3]){ //down
				key[3] = 0;
				if((countPassWord == 2)||(countPassWord == 3))
					countPassWord++;
				else
					countPassWord = 0;
			}
			if(key[0]){ //left
				key[0] = 0;
				countPassWord = 0;
			}
			if(key[1]){ //Right
				key[1] = 0;
				countPassWord = 0;
			}
			if(key[4]){
				key[4] = 0;
				if(countPassWord==4)	
					countPassWord++;
				else	countPassWord = 0;
			}
			if(countPassWord == 5){
				passSecurity = 1;
				countPassWord = 0;	
			}
			// End ***** U U D D Enter ******************************
		}
	}

}//End SD Card
void game(char * command){
	int i,j;
	char buffer[20];
	unsigned char temp;
	if(screen != gameScreen){

		screen = gameScreen;
		pageDisplay = 0;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		sprintf_P(buffer,PSTR("CMD:"));
		lcd_print3(5,0,buffer,&f3,0,0);
		for(i = 1;i<12;i++){
			sprintf_P(buffer,PSTR("N%01u:"),i);
			lcd_print3(5,i*10,buffer,&f3,0,0);
		}
	}
	else {
		if(key[0]){ //left
			key[0] = 0;
			pageDisplay -= 1; 
			if(pageDisplay < 0){
				pageDisplay = 5;
			}
			lcd_clear_graph();    		 // clear graphics memory of LCD
			lcd_clear_text();  		     // clear text memory of LCD
			sprintf_P(buffer,PSTR("CMD:"));
			lcd_print3(5,0,buffer,&f3,0,0);
			for(i = 1;i < 12;i++){
				sprintf_P(buffer,PSTR("N%01u:"),(i + pageDisplay*11));
				lcd_print3(5,i*10,buffer,&f3,0,0);
			}
		}
		if(key[1]){ //Right
			key[1] = 0;
			pageDisplay += 1; 
			if(pageDisplay > 5){
				pageDisplay = 0;
			}
			lcd_clear_graph();    		 // clear graphics memory of LCD
			lcd_clear_text();  		     // clear text memory of LCD
			sprintf_P(buffer,PSTR("CMD:"));
			lcd_print3(5,0,buffer,&f3,0,0);
			for(i = 1;i < 12;i++){
				sprintf_P(buffer,PSTR("N%01u:"),(i + pageDisplay*11));
				lcd_print3(5,i*10,buffer,&f3,0,0);
			}
		}
		for(i = 0;i<12;i++){//Ma lenh truyen cho SmartServer
			byteToHexString(RegBytes[0+i],buffer);
			lcd_print3(32+17*i,0,buffer,&f3,0,0);
		}
		for(i = 0;i<11;i++){//Moi trang hien thi so lieu cua 11 Node
			for(j = 0;j<12;j++){// 12 Byte so lieu cua moi Node
				byteToHexString(RegBytes[64+32*(i+pageDisplay*11)+j],buffer);
				lcd_print3(32+17*j,10+10*i,buffer,&f3,0,0);
			}
		}
	}
}//End game

void settings(char * command){
	unsigned char i=0;
	char buffer[18];
	char str[28];
	unsigned int current;
	unsigned long CF_Scall;
	EnergySruct Energy;
	if(screen != settingScreen){
		screen = settingScreen;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		readEEPROM(str,37,40);	
		tM = (unsigned char)str[0];
		tM <<= 8;
		tM |= (unsigned char)str[1];
		//tL = str[2]*256 + str[3];
		readEEPROM(str,48,71);
		
		cE[0]  = (unsigned char)str[3];
		cE[0] <<= 8;
		cE[0] |= (unsigned char)str[2];
		cE[0] <<= 8;
		cE[0] |= (unsigned char)str[1];
		cE[0] <<= 8;
		cE[0] |= (unsigned char)str[0];	
		cE[1]  = (unsigned char)str[7];
		cE[1] <<= 8;
		cE[1] |= (unsigned char)str[6];
		cE[1] <<= 8;
		cE[1] |= (unsigned char)str[5];
		cE[1] <<= 8;
		cE[1] |= (unsigned char)str[4];	
		cE[2]  = (unsigned char)str[11];
		cE[2] <<= 8;
		cE[2] |= (unsigned char)str[10];
		cE[2] <<= 8;
		cE[2] |= (unsigned char)str[9];
		cE[2] <<= 8;
		cE[2] |= (unsigned char)str[8];	
		
		cU[0]  = (unsigned char)str[13];
		cU[0] <<= 8;
		cU[0] |= (unsigned char)str[12];
		cU[1]  = (unsigned char)str[15];
		cU[1] <<= 8;
		cU[1] |= (unsigned char)str[14];
		cU[2]  = (unsigned char)str[17];
		cU[2] <<= 8;
		cU[2] |= (unsigned char)str[16];

		cI[0]  = (unsigned char)str[19];
		cI[0] <<= 8;
		cI[0] |= (unsigned char)str[18];
		cI[1]  = (unsigned char)str[21];
		cI[1] <<= 8;
		cI[1] |= (unsigned char)str[20];
		cI[2]  = (unsigned char)str[23];
		cI[2] <<= 8;
		cI[2] |= (unsigned char)str[22];

		lcd_print3(5,20,"E1:",&f3,0,0);
		lcd_print3(5,35,"E2:",&f3,0,0);
		lcd_print3(5,50,"E3:",&f3,0,0);
		lcd_print3(100,5,"cE",&f3,0,0);
		lcd_print3(160,5,"cU",&f3,0,0);
		lcd_print3(210,5,"cI",&f3,0,0);

		readEEPROM(str,72,75);
		sprintf_P(buffer,PSTR("He so TI:"));
		lcd_print3(5,65,buffer,&f3,0,0);
		TICoef1 = (unsigned char)str[1];
		TICoef1 <<= 8;
		TICoef1 |= (unsigned char)str[0];  
		TICoef2 = (unsigned char)str[3];
		TICoef2 <<= 8;
		TICoef2 |= (unsigned char)str[2]; 
		if((TICoef1 > 500)||(TICoef2 > 200)){//Add 06/08/2010
			TICoef1 = 200;
			TICoef2 = 5;
			buffer[0] = TICoef1 & 0xFF; 
			buffer[1] = (TICoef1>>8) & 0xFF; 
			buffer[2] = TICoef2 & 0xFF; 
			buffer[3] = (TICoef2>>8) & 0xFF; 
			writeEEPROM(buffer,72,75);
		} 
                                                                                                         
		for(i=0;i<3;i++){//Add 06/08/2010
			if((cE[i]<0)||(cU[i]<0)||(cI[i]<0)||(cE[i]>40000)||(cU[i]>1000)||(cI[i]>1000)){
				cE[0] = 22500;
				cE[1] = 22500;
				cE[2] = 22500;
				cU[0] = 540;
				cU[1] = 540;
				cU[2] = 540;
				cI[0] = 555;//Vref = 125mVAC
				cI[1] = 555;
				cI[2] = 555;	
				setCalibrationParam(cE,cU,cI);
				break;
			}
		}
		for(i=0;i<3;i++){
			numberToString(cE[i],buffer);
			lcd_print3(100,20+15*i,buffer,&f3,0,0);
			numberToString(cU[i],buffer);
			lcd_print3(160,20+15*i,buffer,&f3,0,0);
			numberToString(cI[i],buffer);
			lcd_print3(210,20+15*i,buffer,&f3,0,0);
		}
	
		wordToString(TICoef1,buffer);
		lcd_print3(100,65,buffer,&f3,0,0);
		wordToString(TICoef2,buffer);
		lcd_print3(160,65,buffer,&f3,0,0);	

		sprintf_P(buffer,PSTR("Thoi gian RSMD :"));
		lcd_print3(5,80,buffer,&f3,0,0);

		wordToString(tM,buffer);
		lcd_print3(100,80,buffer,&f3,0,0);

		sprintf_P(buffer,PSTR("Reset Energy:"));
		lcd_print3(5,95,buffer,&f3,0,0);
	
		wordToString(tL,buffer);
		lcd_print3(100,95,"NO",&f3,0,0);

		sprintf_P(buffer,PSTR("Load defaul:"));
		lcd_print3(130,95,buffer,&f3,0,0);
		lcd_print3(210,95,"NO",&f3,0,0);	

		lcd_print3(10,114,"BACK",&f1,0,0);
		lcd_print3(210,114,"OK",&f1,0,0);

		readEEPROM(str,119,121);
		PHCAL1 = str[0];
		PHCAL2 = str[1];
		PHCAL3 = str[2];
		if((PHCAL1<0)||(PHCAL2<0)||(PHCAL3<0)){//Add 06/08/2010
			PHCAL1 = 0;
			PHCAL2 = 0;
			PHCAL3 = 0;
			str[0] = 0;
			str[1] = 0;
			str[2] = 0;
			writeEEPROM(str,119,121);
		}
		wordToString(PHCAL1,buffer);	
		lcd_print3(100,110,buffer,&f3,0,0);
		wordToString(PHCAL2,buffer);	
		lcd_print3(160,110,buffer,&f3,0,0);
		wordToString(PHCAL3,buffer);	
		lcd_print3(210,110,buffer,&f3,0,0);

		changeTiPoiter = 0;
		passSecurity = 0;
		countPassWord = 0;
		resetEnergy = -1;
		loadDefaul = -1;

	}
	else {		
		if(isNextSecond2()){	
			Energy = readTotalEnergy();
			for(i=0;i<3;i++){
				if(Energy.energy[i]<0){
					resetTotalEnergy();
					break;
				}
			    current = readCurrent(i + 1);
				floatToString(current,buffer);
				lcd_print3(25,20+15*i,buffer,&f3,0,0);
			}
		}
		if(passSecurity == 1){
			if(key[0]){//Left
				changeTiPoiter--;
				key[0] = 0;
				if(changeTiPoiter == 255) changeTiPoiter = 16;
			}
			if(key[1]){//Righ
				changeTiPoiter++;
				key[1] = 0;
				if(changeTiPoiter >= 17) changeTiPoiter = 0;
			}
			if(key[3]){//Down
				switch (changeTiPoiter){
					case 0:
						cE[0] -= 50;
						if(cE[0] <= 17999) cE[0] = 25000;
						break;
					case 1:
						cU[0] -= 1;
						if(cU[0] <= 449) cU[0] = 650;
						break;
					case 2:
						cI[0] -= 1;
						if(cI[0] <= 449) cI[0] = 650;
						testCurrentCalib(cI);
						break;
					case 3:
						cE[1] -= 50;
						if(cE[1] <= 17999) cE[1] = 25000;
						break;
					case 4:
						cU[1] -= 1;
						if(cU[1] <= 449) cU[1] = 650;
						break;					
					case 5:
						cI[1] -= 1;
						if(cI[1] <= 449) cI[1] = 650;
						testCurrentCalib(cI);
						break;
					case 6:
						cE[2] -= 50;
						if(cE[2] <= 17999) cE[2] = 25000;
						break;
					case 7:
						cU[2] -= 1;
						if(cU[2] <= 449) cU[2] = 650;
						break;
					case 8:
						cI[2] -= 1;
						if(cI[2] <= 449) cI[2] = 650;
						testCurrentCalib(cI);
						break;
					case 9:
						TICoef1 -= 5;
						if(TICoef1 >= 505) TICoef1 = 500;
						break;
					case 10:
						TICoef2 -= 1;
						if(TICoef2 >= 501) TICoef2 = 100;
						break;
					case 11:
						tM -= 10;
						if(tM <= 0) tM = 600;
						break;
					case 12:
						resetEnergy = -1*resetEnergy;
						break;
					case 13:
						PHCAL1 -= 1;
						if(PHCAL1 < 0) PHCAL1 = 127;
						break;
					case 14:
						PHCAL2 -= 1;
						if(PHCAL2 < 0) PHCAL2 = 127;
						break;
					case 15:
						PHCAL3 -= 1;
						if(PHCAL3 < 0) PHCAL3 = 127;
						break;
					case 16:
						loadDefaul = -1*loadDefaul;
						break;
					default : 	
						break;
				}
				key[3] = 0;
			}
			if(key[2]){//Up
				switch (changeTiPoiter){
					case 0:
						cE[0] += 50;
						if(cE[0] >= 25050) cE[0] = 18000;
						break;
					case 1:
						cU[0] += 1;
						if(cU[0] >= 651) cU[0] = 450;
						break;
					case 2:
						cI[0] += 1;
						if(cI[0] >= 651) cI[0] = 450;
						testCurrentCalib(cI);
						break;
					case 3:
						cE[1] += 50;
						if(cE[1] >= 25050) cE[1] = 18000;
						break;
					case 4:
						cU[1] += 1;
						if(cU[1] >= 651) cU[1] = 450;
						break;					
					case 5:
						cI[1] += 1;
						if(cI[1] >= 651) cI[1] = 450;
						testCurrentCalib(cI);
						break;
					case 6:
						cE[2] += 50;
						if(cE[2] >= 25050) cE[2] = 18000;
						break;
					case 7:
						cU[2] += 1;
						if(cU[2] >= 651) cU[2] = 450;
						break;
					case 8:
						cI[2] += 1;
						if(cI[2] >= 651) cI[2] = 450;
						testCurrentCalib(cI);
						break;
					case 9:
						TICoef1 += 5;
						if(TICoef1 >= 505) TICoef1 = 5;
						break;
					case 10:
						TICoef2 += 1;
						if(TICoef2 >= 101) TICoef2 = 1;
						break;
					case 11:
						tM += 10;
						if(tM >= 610) tM = 10;
						break;
					case 12:
						resetEnergy = -1*resetEnergy;
						break;
					case 13:
						PHCAL1 += 1;
						if(PHCAL1 < 0) PHCAL1 = 0;
						break;
					case 14:
						PHCAL2 += 1;
						if(PHCAL2 < 0) PHCAL2 = 0;
						break;
					case 15:
						PHCAL3 += 1;
						if(PHCAL3 < 0) PHCAL3 = 0;
						break;
					case 16:
						loadDefaul = -1*loadDefaul;
						break;
					default : 	
						break;
				}
				key[2] = 0;
			}
			for(i=0;i<3;i++){
				numberToString(cE[i],buffer);
				if((3*i) == changeTiPoiter)
					lcd_print4(100,20+15*i,buffer,&f3,0,0);
				else
					lcd_print3(100,20+15*i,buffer,&f3,0,0);
				numberToString(cU[i],buffer);
				if((3*i+1) == changeTiPoiter)
					lcd_print4(160,20+15*i,buffer,&f3,0,0);
				else
					lcd_print3(160,20+15*i,buffer,&f3,0,0);
				numberToString(cI[i],buffer);
				if((3*i+2) == changeTiPoiter)
					lcd_print4(210,20+15*i,buffer,&f3,0,0);
				else
					lcd_print3(210,20+15*i,buffer,&f3,0,0);
			}	
			
			wordToString(TICoef1,buffer);
			if(changeTiPoiter == 9)
				lcd_print4(100,65,buffer,&f3,0,0);
			else
				lcd_print3(100,65,buffer,&f3,0,0);
			wordToString(TICoef2,buffer);
			if(changeTiPoiter == 10)
				lcd_print4(160,65,buffer,&f3,0,0);
			else
				lcd_print3(160,65,buffer,&f3,0,0);
			wordToString(tM,buffer);
			if(changeTiPoiter == 11)
				lcd_print4(100,80,buffer,&f3,0,0);
			else
				lcd_print3(100,80,buffer,&f3,0,0);
			if(changeTiPoiter == 12){
				if(resetEnergy == 1)
					lcd_print4(100,95,"YES",&f3,0,0);
				else
					lcd_print4(100,95,"NO  ",&f3,0,0);
			}
			else{
				if(resetEnergy == 1)
					lcd_print3(100,95,"YES",&f3,0,0);
				else
					lcd_print3(100,95,"NO  ",&f3,0,0);
			}
			if(changeTiPoiter == 16){
				if(loadDefaul == 1)
					lcd_print4(210,95,"YES",&f3,0,0);
				else
					lcd_print4(210,95,"NO  ",&f3,0,0);
			}
			else{
				if(loadDefaul == 1)
					lcd_print3(210,95,"YES",&f3,0,0);
				else
					lcd_print3(210,95,"NO  ",&f3,0,0);
			}
			wordToString(PHCAL1,buffer);
			if(changeTiPoiter == 13)
				lcd_print4(100,110,buffer,&f3,0,0);
			else
				lcd_print3(100,110,buffer,&f3,0,0);
			wordToString(PHCAL2,buffer);
			if(changeTiPoiter == 14)
				lcd_print4(160,110,buffer,&f3,0,0);
			else
				lcd_print3(160,110,buffer,&f3,0,0);
			wordToString(PHCAL3,buffer);
			if(changeTiPoiter == 15)
				lcd_print4(210,110,buffer,&f3,0,0);
			else
				lcd_print3(210,110,buffer,&f3,0,0);
				
			if(key[4]){ // OK
				key[4] = 0;
				buffer[0] = TICoef1 & 0xFF; 
				buffer[1] = (TICoef1>>8) & 0xFF; 
				buffer[2] = TICoef2 & 0xFF; 
				buffer[3] = (TICoef2>>8) & 0xFF; 
				writeEEPROM(buffer,72,75);
				_delay_ms(50);
				setCalibrationParam(cE,cU,cI);
				CF_Scall = 7000/TICoef1;//Vref = 125mVAC
				CF_Scall *= TICoef2;	// CF_Scall = 175*(200/5)/(TICoef1/TICoef2)
				writeActivePowerScalling((unsigned int)CF_Scall);
				writeReactivePowerScalling((unsigned int)CF_Scall);
				buffer[0] = PHCAL1;
				buffer[1] = PHCAL2;
				buffer[2] = PHCAL3;
				writeEEPROM(buffer,119,121);
				writePhaseCorrection(buffer[0],1);
				writePhaseCorrection(buffer[1],2);
				writePhaseCorrection(buffer[2],3);
				//writeLogTime(tL/10);
				setModemTimeout(tM/10);
				if(resetEnergy == 1)
					resetTotalEnergy();
				if(loadDefaul == 1){//Add 6/9/2010
					TICoef1 = 200;
					TICoef2 = 5;
					buffer[0] = TICoef1 & 0xFF; 
					buffer[1] = (TICoef1>>8) & 0xFF; 
					buffer[2] = TICoef2 & 0xFF; 
					buffer[3] = (TICoef2>>8) & 0xFF; 
					writeEEPROM(buffer,72,75);

					cE[0] = 22500;
					cE[1] = 22500;
					cE[2] = 22500;
					cU[0] = 540;
					cU[1] = 540;
					cU[2] = 540;
					cI[0] = 555;
					cI[1] = 555;
					cI[2] = 555;	
					setCalibrationParam(cE,cU,cI);
					
					CF_Scall = 175;//Vref = 125mVAC
					writeActivePowerScalling((unsigned int)CF_Scall);
					writeReactivePowerScalling((unsigned int)CF_Scall);
					buffer[0] = PHCAL1;
					buffer[1] = PHCAL2;
					buffer[2] = PHCAL3;
					writeEEPROM(buffer,119,121);
					writePhaseCorrection(buffer[0],1);
					writePhaseCorrection(buffer[1],2);
					writePhaseCorrection(buffer[2],3);
				}
				lcd_print3(210,114,"     ",&f1,0,0);
				lcd_print3(211,115,"OK",&f1,0,0);
				_delay_ms(100);
				lcd_print3(211,115,"     ",&f1,0,0);
				lcd_print3(210,114,"OK",&f1,0,0);
				messageOK();
			}
    	}
		else {//check security password 
			// ***** U U D D Enter ******************************
			if(key[2]){ //up
				key[2] = 0;
				if((countPassWord == 0)||(countPassWord == 1))
					countPassWord++;
				else
					countPassWord = 0;
			}
			if(key[3]){ //down
				key[3] = 0;
				if((countPassWord == 2)||(countPassWord == 3))
					countPassWord++;
				else
					countPassWord = 0;
			}
			if(key[0]){ //left
				key[0] = 0;
				countPassWord = 0;
			}
			if(key[1]){ //Right
				key[1] = 0;
				countPassWord = 0;
			}
			if(key[4]){
				key[4] = 0;
				if(countPassWord==4)	
					countPassWord++;
				else	countPassWord = 0;
			}
			if(countPassWord == 5){
				passSecurity = 1;
				countPassWord = 0;	
			}
			// End ***** U U D D Enter ******************************
		}
	}

}
void lightControl(char * command){
	char buffer[28];
	if(screen != lightControlScreen){
		screen = lightControlScreen;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		sprintf_P(buffer,PSTR("LIGHT CONTROL TABLE"));
		lcd_print3(40,15,buffer,&f1,0,0);
		lcd_print3(90,40,"  OFF",&f1,0,0);
		lcd_print3(90,60,"ON 30%",&f1,0,0);
		lcd_print3(90,80,"ON 70%",&f1,0,0);
		lcd_print3(90,100,"ON 100%",&f1,0,0);
		lcd_print3(10,114,"BACK",&f1,0,0);
		lcd_print3(210,114,"OK",&f1,0,0);
		switch (lightStatus){
			case 0:
				lcd_box(85, 35, 145,55,1);
				break;
			case 1:
				lcd_box(85, 55, 145,75,1);
				break;
			case 2:
				lcd_box(85, 75, 145,95,1);
				break;
			case 3:
				lcd_box(85, 95, 145,115,1);
				break;
			default :
				break;
		}
	}
	else{
		lastlightStatus = lightStatus;
		if(key[3]){	
			lightStatus++;
			key[3] = 0;
			if(lightStatus == 4) lightStatus = 0;
		}
		if(key[2]){
			lightStatus--;
			key[2] = 0;
			if(lightStatus == 255) lightStatus = 3;
		}
		if(key[4]){
			key[4] = 0;
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);
			//changeLightStatus(lightStatus);
			buttonCmd = 0; 
			argsButtonCmd[0] = lightStatus;
		}
		switch (lastlightStatus){
			case 0:
				lcd_box(85, 35, 145,55,0);
				break;
			case 1:
				lcd_box(85, 55, 145,75,0);
				break;
			case 2:
				lcd_box(85, 75, 145,95,0);
				break;
			case 3:
				lcd_box(85, 95, 145,115,0);
				break;
			default :
				break;	
		}
		switch (lightStatus){
			case 0:
				lcd_box(85, 35, 145,55,1);
				break;
			case 1:
				lcd_box(85, 55, 145,75,1);
				break;
			case 2:
				lcd_box(85, 75, 145,95,1);
				break;
			case 3:
				lcd_box(85, 95, 145,115,1);
				break;
			default :
				break;	
		}
	}
}//End Light Control

void messageBox(char * cmd){
	unsigned char cmdCode;
	char str[3];
	cmdCode = parseHexString(cmd[0],cmd[1]);
	byteToString(cmdCode,str);
	screen = 255;
	lcd_box(47,29,192,86,1);
	lcd_box_fill(6,30,24,85,0);
	resetBackLightTime();
	lcd_print3(60,35,"command       successfull",&f3,0,0);
	lcd_print3(115,35,str,&f3,0,0);
}//End MessageBox

void messageErrorSchedule(void){
	lcd_clear_graph();    		 // clear graphics memory of LCD
	lcd_clear_text();  		     // clear text memory of LCD
	//screen = 254;
	lcd_box(40,20,200,90,1);
	resetBackLightTime();
	lcd_print3(60,35,"Lich dong cat khong hop le",&f3,0,0);
	lcd_print3(60,50,"De nghi dat lai lich dong cat",&f3,0,0);
}//End MessageErrorSchedule

void messageOK(void){
	lcd_clear_graph();    		 // clear graphics memory of LCD
	lcd_clear_text();  		     // clear text memory of LCD
	screen = 253;
	lcd_box(80,40,160,80,1);
	resetBackLightTime();
	lcd_print3(110,53,"OK",&f1,0,0);
	//lcd_print3(60,50,"De nghi dat lai lich dong cat",&f3,0,0);
}//End MessageErrorSchedule

void messageERROR(void){
	lcd_clear_graph();    		 // clear graphics memory of LCD
	lcd_clear_text();  		     // clear text memory of LCD
	screen = 252;
	lcd_box(80,40,160,80,1);
	resetBackLightTime();
	lcd_print3(98,53,"ERROR",&f1,0,0);
	//lcd_print3(60,50,"De nghi dat lai lich dong cat",&f3,0,0);
}

void menu(char * command){
	char buffer[45];
	if(screen != menuScreen){
		screen = menuScreen;
		lcd_clear_graph();    		 // clear graphics memory of LCD
		lcd_clear_text();  		     // clear text memory of LCD
		//draw(option,3,16);
		draw(manualControl,3,16);
		draw(option,8,16);
		draw(iconSchedule,13,16);
		draw(clock,18,16);
		draw(games,23,16);
		draw(floppyDisk,3,56);
		draw(monitor,8,56);
		//currentMenu = 0;
		switch (currentMenu){
			case 0:
				lcd_box(22, 14, 58,48,1);
				sprintf_P(buffer,PSTR("LIGHT CONTROL"));
				lcd_print3(70,114,buffer,&f1,0,0);
				break;
			case 1:
				lcd_box(62, 14, 98,48,1);
				sprintf_P(buffer,PSTR("SETTINGS"));
				lcd_print3(90,114,buffer,&f1,0,0);
				break;
			case 2:
				lcd_box(102, 14, 138,48,1);
				sprintf_P(buffer,PSTR("SCHEDULE ON/OFF"));
				lcd_print3(60,114,buffer,&f1,0,0);
				break;
			case 3:
				lcd_box(142, 14, 178,48,1);
				sprintf_P(buffer,PSTR("TIME AND DATE"));
				lcd_print3(70,114,buffer,&f1,0,0);
				break;
			case 4:
				lcd_box(182, 14, 218,48,1);
				sprintf_P(buffer,PSTR("GAMES"));
				lcd_print3(95,114,buffer,&f1,0,0);
				break;
			case 5:
				lcd_box(22, 54, 58,88,1);
				sprintf_P(buffer,PSTR("SD CARD"));
				lcd_print3(95,114,buffer,&f3,0,0);
				break;
			case 6:
				lcd_box(62, 54, 98,88,1);
				sprintf_P(buffer,PSTR("TERMINAL"));
				lcd_print3(85,114,buffer,&f1,0,0);
				break;
			default :
				break;
		}
		sprintf_P(buffer,PSTR("OK"));
		lcd_print3(210,114,buffer,&f1,0,0);
		sprintf_P(buffer,PSTR("BACK"));
		lcd_print3(10,114,buffer,&f1,0,0);
	}
	else{
		lastMenu = currentMenu;
		if(key[1]){	
			currentMenu++;
			key[1] = 0;
			if(currentMenu >= 7) currentMenu = 0;
		}
		if(key[0]){
			currentMenu--;
			key[0] = 0;
			if(currentMenu == 255) currentMenu = 6;
		}
		if(key[2]){
			if(currentMenu >= 5) currentMenu -= 5;
			key[2] = 0;
		}
		if(key[3]){
			if(currentMenu <= 1) currentMenu += 5;
			key[3] = 0;
		}
		switch (lastMenu){
			case 0:
				lcd_box(22, 14, 58,48,0);
				break;
			case 1:
				lcd_box(62, 14, 98,48,0);
				break;
			case 2:
				lcd_box(102, 14, 138,48,0);
				break;
			case 3:
				lcd_box(142, 14, 178,48,0);
				break;
			case 4:
				lcd_box(182, 14, 218,48,0);
				break;
			case 5:
				lcd_box(22, 54, 58,88,0);
				break;
			case 6:
				lcd_box(62, 54, 98,88,0);
				break;
			default :
				break;
		}
		switch (currentMenu){
			case 0:
				lcd_box(22, 14, 58,48,1);
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(60,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("LIGHT CONTROL"));
				lcd_print3(70,114,buffer,&f1,0,0);
				break;
			case 1:
				lcd_box(62, 14, 98,48,1);
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(60,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("SETTINGS"));
				lcd_print3(90,114,buffer,&f1,0,0);
				break;
			case 2:
				lcd_box(102, 14, 138,48,1);   
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(50,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("SCHEDULE ON/OFF"));
				lcd_print3(60,114,buffer,&f1,0,0);
				break;
			case 3:
				lcd_box(142, 14, 178,48,1);
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(50,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("TIME AND DATE"));
				lcd_print3(70,114,buffer,&f1,0,0);
				break;
			case 4:
				lcd_box(182, 14, 218,48,1);
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(50,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("GAMES"));
				lcd_print3(95,114,buffer,&f1,0,0);
				break;
			case 5:
				lcd_box(22, 54, 58,88,1);
				sprintf_P(buffer,PSTR("                                "));
				lcd_print3(50,114,buffer,&f1,0,0);
				sprintf_P(buffer,PSTR("SD CARD"));
				lcd_print3(95,114,buffer,&f1,0,0);
				break;
			case 6:
				lcd_box(62, 54, 98,88,1);
				sprintf_P(buffer,PSTR("TERMINAL"));
				lcd_print3(85,114,buffer,&f1,0,0);
				break;
			default :
				break;
		}
		if(key[4]){
			key[4] = 0;
			lcd_print3(210,114,"     ",&f1,0,0);
			lcd_print3(211,115,"OK",&f1,0,0);
			_delay_ms(100);
			lcd_print3(211,115,"     ",&f1,0,0);
			lcd_print3(210,114,"OK",&f1,0,0);
			switch (currentMenu){
				case 0:
					lightControl(key);
					break;
				case 1:
					settings(key);
					break;
				case 2:
					changeSchedule(key);
					break;
				case 3:
					changeTimeDate(key);
					break;
				case 4:
					game(key);
					break;
				case 5:
					sdCard(key);
					break;
				case 6:
					terminal(key);
					break;
				default :
					break;
			}
			
		}
	}
}//End Menu
unsigned char readButtonCommand(char * args){
	args[0] = argsButtonCmd[0];
	return buttonCmd;
}
void clearButtonCmd(void){
	buttonCmd = 255;
}
void draw_table(void){
	char buffer[28];
	if(screen == 254){
		changeStreetLightSchedule(key);
		return;
	}
	if(screen == 251){
		changeDecorateLightSchedule(key);
		return;
	}
	key[0]=0;key[1]=0;key[2]=0;key[3]=0;key[4]=0;key[5]=0;
	screen = mainScreen;
	lcd_clear_graph();    		 // clear graphics memory of LCD
	lcd_clear_text();  		     // clear text memory of LCD

	sprintf_P(buffer,PSTR("PHA"));
	lcd_print3(4,3,buffer,&f3,0,0);
	sprintf_P(buffer,PSTR("PHA1"));
	lcd_print3(37,2,buffer,&f1,0,0);
	sprintf_P(buffer,PSTR("PHA2"));
	lcd_print3(86,2,buffer,&f1,0,0);
	sprintf_P(buffer,PSTR("PHA3"));
	lcd_print3(135,2,buffer,&f1,0,0);
	sprintf_P(buffer,PSTR("U(V)"));
	lcd_print3(4,18,buffer,&f3,0,0);
	sprintf_P(buffer,PSTR("I(A)"));
	lcd_print3(6,33,buffer,&f3,0,0);
	sprintf_P(buffer,PSTR("PF"));
	lcd_print3(9,48,buffer,&f3,0,0);
	sprintf_P(buffer,PSTR("  TOTAL ENERGY :"));
	lcd_print3(4,62,buffer,&f1,0,0);
	sprintf_P(buffer,PSTR("kWh"));
	lcd_print3(214,63,buffer,&f3,0,0);
	sprintf_P(buffer,PSTR("  TIME SCHEDULE "));
	lcd_print3(4,77,buffer,&f1,0,0);
	//draw_table
	lcd_line_ver(0,0,29,1);
	lcd_line(0,15,239,15,1); //PHA
	lcd_line(0,30,175,30,1); //U(V)
	lcd_line(0,45,175,45,1); //I(A)
	lcd_line_ver(60,0,29,1); //PF 
	lcd_line_ver(75,0,29,1); //Total Energy
	lcd_line_ver(90,0,29,1); 
	lcd_line_ver(103,0,29,1); 
	lcd_line_ver(116,0,29,1);
	lcd_line_ver(127,0,29,1);
	
	//Draw colum U,I,PF
	lcd_line(0,0,0,127,1); 
	lcd_line(28,0,28,59,1);
	lcd_line(77,1,77,59,1);
	lcd_line(126,1,126,59,1);
	lcd_line(175,1,175,59,1);

	//Draw colum Time Schedule
	lcd_line(48,90,48,116,1);
	lcd_line(96,90,96,116,1);
	lcd_line(144,90,144,116,1);
	lcd_line(192,90,192,116,1);
	//
	lcd_line(239,1,239,127,1);
	if(getModemConnected())
		lcd_print3(213,117,"!$",&f3,0,0); // Modem already connected
	else
		lcd_print3(213,117,"!&",&f3,0,0); // not connected
	lcd_line(239,1,239,127,1);
	DisplayLightStatus(getLightStatus());
}//End Draw Table
void scanKey(void){
	unsigned char scan,Up=0,Down=0,Right=0,Left=0,Enter=0,Escap=0;
	if(getBackLightTime() >= backLightTimeout){
		resetBackLightTime();
		if(screen != mainScreen){
			draw_table();
		}
		//BackLighOff;
	}	
	//scan keyboard
	PORTB |= 0x80;//reset 4017
	Delay(20);
	PORTB &= 0x7F;
	Delay(10);
	for(scan=0;scan<8;scan++){
		if(PINB&0x20){
			_delay_ms(150);
			if(scan == 0){
				Down = 1;
				break;
			}
			if(scan == 1){
				Enter = 1;
				break;
			}
			if(scan == 2){
				Right = 1;
				break;
			}
			if(scan == 3){
				Up = 1;
				break;
			}
			if(scan == 4){
				Left = 1;
				break;
			}
			if(scan == 5){
				Escap = 1;
				break;
			}
	    }
		PORTB |= 0x40;//clock 4017
		Delay(12);
		PORTB &= 0xBF;
		Delay(12);
	}
	if(Left){
		key[0] = 1;
	    pressButton = 1;		
	}
	if(Right){
		key[1] = 1;
	    pressButton = 1;			
	}
	if(Up){
		key[2] = 1;
		pressButton = 1;		
	}
	if(Down){
		key[3] = 1;
		pressButton = 1; 		
	}	
	if(Enter){
		key[4] = 1;
		pressButton = 1;
	}
	if(pressButton){
		pressButton = 0;
		//BackLighOn;
		resetBackLightTime();
		switch (screen){
			case mainScreen:
				key[0]=0;key[1]=0;key[2]=0;key[3]=0;key[5]=0;
				if(key[4] == 1){
					key[4] = 0;
					currentMenu = 0;
					menu(key);
				}
				break;
			case menuScreen:
				menu(key);
				break;
			case lightControlScreen:
				lightControl(key);
				break;
			case timeScreen:
				changeTimeDate(key);
				break;
			case scheduleScreen:
				changeSchedule(key);
				break;
			//case settingScreen:
				//settings(key);
				//break;
			case gameScreen:
				game(key);
				break;
			//case sdcardScreen:
				//sdCard(key);
				//break;
			//case terminalScreen:
				//terminal(key);
				//break;
			case streetLightScheduleScreen:
				changeStreetLightSchedule(key);
				break;
			case decorateLightScheduleScreen:
				changeDecorateLightSchedule(key);
				break;
			default :
				break;
		}	
	}	
	if(Escap){
		key[5] = 1;
		//BackLighOn;
		resetBackLightTime();
		//if(screen == menuScreen) draw_table();
		//if((screen != menuScreen)&&(screen != mainScreen)) menu(key);	
		switch (screen){
			case menuScreen:
				draw_table();
				break;
			case streetLightScheduleScreen:
				changeSchedule(key);
				break;
			case decorateLightScheduleScreen:
				changeSchedule(key);
				break;
			//case gameScreen:
				//menu(key);
				//break;
			default :
				if((screen != menuScreen)&&(screen != mainScreen)) menu(key);	
				break;
		}	
	}
	if(screen == settingScreen) settings(key);
	if(screen == gameScreen) game(key);
	if(screen == sdcardScreen) sdCard(key);
	if(screen == terminalScreen) terminal(key);
	//if(screen == gameScreen) game(key);
}//End scanKey
