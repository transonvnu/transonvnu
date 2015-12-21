/*
 * Solar_LED_SLC.c
 *
 * Created: 19/07/2013 11:49:29 SA
 *  Author: SCEC
 */ 

//#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdlib.h>
//#include <float.h>

#include "base.h"
#include "driver/uart.h"
#include "initsys.h"
#include "driver/relay.h"
#include "driver/uart.h"
#include "driver/ks0108.h"
#include "driver/ctrkey.h"
#include "driver/ds1307.h"
#include "timer.h"
#include "schedule.h"
#include "screen.h"
#include "main.h"
#include "sysComm.h"
#include "settings.h"


#define ADDR_EBAT	0x0A	// address store Ebat in NVRAM ds1307

//extern unsigned char Dis_Buf[];
//extern char ds1307_addr[];
extern unsigned int adc_data[];
prog_char confirmStr[]	= "Yes?           No?\0";
unsigned char EEMEM backupYear = 15;	// 2015
unsigned char EEMEM fail_RTC = 0;
unsigned char EEMEM node_ID;

char dateCompiler[] PROGMEM = __DATE__;
//char NRID_EEP[] EEMEM = "0502xxxxxxxx\0";
char NRID_RAM[]	= {0x05,0x02,0x00,0x00,0x00,0x00};

unsigned char	supply_source = USE_BAT,OldSupply_source = 0xFF;
unsigned int	NiMH_voltage;
unsigned int	Umain; 
unsigned int	Uvp,Ubi,Umi,Iacin,Ivp,Ub1,Ub2;
unsigned int	Ich1, Ich2;
long			Ebat, Etemp = 0;

unsigned char	confirm_OK = 0,displayClockBar = 1;
unsigned char	keyboardTimeout = 0;
unsigned char	refreshSCR = 0x00;
unsigned char	cntTimer_EnergySum = 0;
#define REFRESH_ENB	0x0F
#define REFRESH_FLAG	0xF0
#define NOT_REFRESH 0x00

/*
 * Mirror of the MCUCSR register, taken early during startup.
 */
unsigned char mcucsr __attribute__((section(".noinit")));

void handle_mcucsr(void)
__attribute__((section(".init3")))
__attribute__((naked));
void handle_mcucsr(void)
{
	mcucsr = MCUCSR;
	MCUCSR = 0;
}

unsigned char getRefreshFlag(void){
	if(refreshSCR==REFRESH_FLAG){
		refreshSCR = NOT_REFRESH;
		return 1;
	}else
		return 0;
}
void setRefreshable(){
	refreshSCR = REFRESH_ENB;
}

void enbClockBar(){
	displayClockBar = 1;
}
void disClockBar(){
	displayClockBar = 0;
}
//extern unsigned int tempvalue;
void processEcsKey(void){
	if(!confirm_OK){
		toPreviousScreen();
	}else{
		enbClockBar();
		confirm_OK = 0;
	}
}
void processOkKey(void){
	//_delay_ms(500);
	if(!confirm_OK){
		disClockBar();
		clearROW(7);
		f5x7_putsf(0,7,confirmStr);
		confirm_OK = 1;
	}else{
		saveVarInScr(getCurrentScreen());
		toPreviousScreen();
		//_delay_ms(200);
		enbClockBar();
		confirm_OK = 0;
	}
}
void keyboardTask(void){
	unsigned char i;
	switch(getCurrentScreen()){
		case MAINSCR:
			switch(getButton()){
				case UP_PRESSED:
					changeCurrentScreen(MCTRSCR);
					break;
				case DOWN_PRESSED:
					changeCurrentScreen(INFOSCR);
					break;
				case OK_PRESSED:
					changeCurrentScreen(MENUSCR);
					break;
				case LEFT_PRESSED:
					changeCurrentScreen(SCHESCR);
					break;
				case RIGHT_PRESSED:
					changeCurrentScreen(CALBSCR);
					break;
			}
			break;
		case MENUSCR:
			switch(getButton()){
				case UP_PRESSED:
					selectMovUp();
					break;
				case DOWN_PRESSED:
					selectMovDw();
					break;
				case ESC_PRESSED:
					toPreviousScreen();
					break;
				case OK_PRESSED:
					i = getSelectedObj();
					switch(i){
						case TERMOBJ:
							changeCurrentScreen(TERMSCR);
							break;
						case INFOOBJ:
							changeCurrentScreen(INFOSCR);
							break;
						default:
							changeCurrentScreen(i);
							break;
					}
				default:
					break;
			}
			break;
		case CHG1SCR:
		case CHG2SCR:
		case ILAMSCR:
		case UB01SCR:
		case UBINSCR:
		case MCTRSCR:
		case SETTSCR:
		case SCHESCR:
		case RLTCSCR:
			switch(getButton()){
				case UP_PRESSED:
					selectMovUp();
					break;
				case DOWN_PRESSED:
					selectMovDw();
					break;
				case RIGHT_PRESSED:
					incValSelObj(1);
					//putchar1(tempvalue);
					break;
				case LEFT_PRESSED:
					desValSelObj(1);
					//putchar1(tempvalue);
					break;
				case ESC_PRESSED:
					processEcsKey();					
					break;
				case OK_PRESSED:
					processOkKey();
					break;
				default:
					break;
			}
			break;
		case TERMSCR:
		case INFOSCR:
			switch(getButton()){
				case ESC_PRESSED:
					enbClockBar();
					processEcsKey();
					break;
				default:
					break;
			}
			break;
		case CALBSCR:
			switch(getButton()){
				case UP_PRESSED:
				selectMovUp();
				break;
				case DOWN_PRESSED:
				selectMovDw();
				break;
				case ESC_PRESSED:
				toPreviousScreen();
				break;
				case OK_PRESSED:
				i = getSelectedObj();
				switch(i){
					case CHG1OBJ:
					changeCurrentScreen(CHG1SCR);
					break;
					case CHG2OBJ:
					changeCurrentScreen(CHG2SCR);
					break;
					case ILAMOBJ:
					changeCurrentScreen(ILAMSCR);
					break;
					case UBINOBJ:
					changeCurrentScreen(UBINSCR);
					break;
					case UB01OBJ:
					changeCurrentScreen(UB01SCR);
					break;
					default:
					changeCurrentScreen(i);
					break;
				}
				default:
				break;
			}
			break;
		default:
			break;
	}
}

unsigned char getChangedSupplySrc(){
	if(supply_source != OldSupply_source){
		OldSupply_source = supply_source;
		return 1;
	}
	else 
		return 0;
}
unsigned char getSupplySrc(){
	return supply_source;
}

void sendNODECMD(unsigned char cmdID){
	puts1("NODE");
	putchar1(cmdID);
}

void calculatingProcess(void){
	union longintpak Ptemp;
	//long	Ptemp;
	int tmpX1 = 0,tmpY1 = 0,tmpX2 = 0,tmpY2 = 0;
	NiMH_voltage	= (adc_data[ADC_VCELL]*50)/101;			//	Ds1307 battery
	
	Umain			= (adc_data[ADC_VACIN]*22)/100;			//	VAC input
	//Uvp = (adc_data[2]*122983)/50000;						//
	eeprom_busy_wait();
	tmpX1 = eeprom_read_word(&eeUbinx1);
	eeprom_busy_wait();
	tmpY1 = eeprom_read_word(&eeUbiny1);
	Umi				= ((unsigned long)adc_data[ADC_MAIN]*tmpY1)/tmpX1;		//	Umain in
	
	//Ubi				= (adc_data[ADC_VBATT]*191714)>>16;		//	Vbat(1+2)
	Ubi				= ((unsigned long)adc_data[ADC_VBATT]*tmpY1)/tmpX1;		//	Vbat(1+2)
	eeprom_busy_wait();
	tmpX1 = eeprom_read_word(&eeUb1x1);
	eeprom_busy_wait();
	tmpY1 = eeprom_read_word(&eeUb1y1);
	Ub1				= ((unsigned long)adc_data[ADC_VBAT1]*tmpY1)/tmpX1;		//	Vbat1
	if (Ubi>Ub1)
	{
		Ub2	= Ubi - Ub1;
	}else
		Ub2	= 0;
	
	//Ivp				= ((adc_data[ADC_ILAMP]*113517)>>13);
	
	eeprom_busy_wait(); tmpX1 = eeprom_read_word(&eeilampx1);
	eeprom_busy_wait(); tmpY1 = eeprom_read_word(&eeilampy1);
	eeprom_busy_wait(); tmpX2 = eeprom_read_word(&eeilampx2);
	eeprom_busy_wait(); tmpY2 = eeprom_read_word(&eeilampy2);
	Ivp		= adc_data[ADC_ILAMP];
	if(Ivp>tmpX1)	Ivp	-= tmpX1;
	else			Ivp			= 0;
	tmpY2 = tmpY2 - tmpY1;
	tmpX2 = tmpX2 - tmpX1;
	Ivp = (((unsigned long)Ivp*tmpY2) + ((unsigned long)tmpY1*tmpX2))/tmpX2;

	eeprom_busy_wait();tmpX1 = eeprom_read_word(&eechg1x1);
	eeprom_busy_wait();tmpY1 = eeprom_read_word(&eechg1y1);
	eeprom_busy_wait();tmpX2 = eeprom_read_word(&eechg1x2);
	eeprom_busy_wait();tmpY2 = eeprom_read_word(&eechg1y2);
	Ich1			= adc_data[ADC_ICHR1];
	if (Ich1>tmpX1) Ich1		-= tmpX1;
	else Ich1		= 0;
	tmpY2 = tmpY2 - tmpY1;
	tmpX2 = tmpX2 - tmpX1;
	Ich1 = (((unsigned long)Ich1*tmpY2) + ((unsigned long)tmpY1*tmpX2))/tmpX2;
	
	//Ich2			= ((adc_data[ADC_ICHR2]*171077)>>12);
	eeprom_busy_wait();tmpX1 = eeprom_read_word(&eechg2x1);
	eeprom_busy_wait();tmpY1 = eeprom_read_word(&eechg2y1);
	eeprom_busy_wait();tmpX2 = eeprom_read_word(&eechg2x2);
	eeprom_busy_wait();tmpY2 = eeprom_read_word(&eechg2y2);
	Ich2			= adc_data[ADC_ICHR2];
	if (Ich2>tmpX1)	Ich2	-= tmpX1;
	else	Ich2 = 0;
	tmpY2 = tmpY2 - tmpY1;
	tmpX2 = tmpX2 - tmpX1;
	Ich2 = (((unsigned long)Ich2*tmpY2) + ((unsigned long)tmpY1*tmpX2))/tmpX2;
	
	//supply_source = USE_BAT;
	Ptemp.lval = ((long)Ich2/10)*((long)Ubi/10)/60;		// xx[dV] * xx[cI] =	x,x mW
	Etemp += Ptemp.lval;//(Plamp_old/2) + (Ptemp/2);			//	*,* mWs
	if(supply_source == USE_BAT)
	{		// Use battery as power supply
			// Discharge battery power.
		//if(getLampState()>0)
		{
			if(Ivp> IVP_MINIMUM){
				Ptemp.lval = ((long)Ivp/10)*((long)Ubi/10)/60;
				Etemp -= Ptemp.lval;
				/*if(Etemp > Ptemp.lval){
					Etemp -= Ptemp.lval;
				}else{
					Etemp = 0;
				}*/
			}
			//Plamp_old = Ptemp;
		}
	}
	// ----TASK: 1 MINUTE COUNTER ---------
	cntTimer_EnergySum++;
	if(cntTimer_EnergySum>=60){
		Etemp =  Etemp/60;
		Ebat += Etemp;			// Etemp nang luong ac quy  
		if(Ebat<0){
			Ebat = 0;
		}
		//Ptemp.lval =  Ebat;
		//Write_NVRAM(&(Ptemp.bytes[0]),BAT_ENERGY_ADDR,4);
		writelVal_NVRAM(Ebat,BAT_ENERGY_ADDR);
		Etemp = 0;
		cntTimer_EnergySum = 0;
	}
}
long readlVal_NVRAM(unsigned char _addr){
	union longintpak valtmp;
	Read_NVRAM(&(valtmp.bytes[0]),_addr,4);
	return (valtmp.lval);
}
void writelVal_NVRAM(long _val, unsigned char _addr){
	union longintpak valtmp;
	valtmp.lval = _val;
	Write_NVRAM(&(valtmp.bytes[0]),_addr,4);
}

void displayEbat(char * _str){
	//char _str1[15];
	sprintf(_str,"E:%4u,%01u",(unsigned int)(Ebat/1000),(unsigned int)(Ebat%1000/100));//,ltoa(Ebat/*(readlVal_NVRAM(BAT_ENERGY_ADDR))/25*/, _str1, 10));
	return;
}
int main(void){
	char			str[25];
	char			args[120]={0};
	unsigned char	command=255;
	unsigned char	tempSchedule[9][5];
	unsigned int	i;//,j;
	long			power_lamp=123456;
	//unsigned int	temp;
	struct _RTCDATA RtcMain;
	//struct _RTCDATA * RtcPtr = &RtcMain;
	wdt_disable();    // Disable Watch Dog timer
	pwon_init();
	_delay_ms(500);
	initSchedule();
	wdt_reset();
	if(checkBtPressed()){
		keyboardTimeout = 0;
		//writelVal_NVRAM(0,BAT_ENERGY_ADDR);
		supply_source = USE_BAT;
		writeByte_NVRAM(supply_source,SUPPLY_SOURCE_ADDR);
		str[0] = 0; str[1] = 0;
		Write_NVRAM(str,COUNT_WR_EEMEM_ADDR,2);
		keyboardTimeout = 0;
		//}
		}else{
		supply_source = readByte_NVRAM(SUPPLY_SOURCE_ADDR);
	}
	Ebat = readlVal_NVRAM(BAT_ENERGY_ADDR);
	//Init is Finished
	setRefreshable();	// Screen refresh
	wdt_reset();
    while(1){
//Task 0: When(kbhit())		// any keypressed	
		if(checkBtPressed()){
			keyboardTask();
			keyboardTimeout = 0;
		}
		if(keyboardTimeout>60){ // > 1min
			toMainScreen();
			enbClockBar();
		}
		
//Task 1: When(receive_cmd()) ------------------READ COMMAND TASK--------------------------
		//sprintf(str,"%02X",queueLengthUART1());
		//f5x7_puts(0,6,str);
		command = readCommand(args);
		if((command!=254) && (getCurrentScreen() == TERMSCR)){	// command message incoming and update TERMINAL SCREEEN
			setChangedInScr();
		}
		switch (command){
			case 0:	//0x00
			//Lenh dong cat	tu Server
			//Truyen ve trang thai + schedule table
			changeLampState(args[0]);
			//sel_source_pw(SEL_BATTERY);
			sendNODECMD(0);
			break;
			case 1:	//0x01
			for(i = 0;i<6;i++){
				NRID_RAM[i] = args[i];
			}
			//Lenh yeu cau truyen trang thai
			power_lamp = 95;
			
			sendNODECMD(1);

			putchar1(RtcMain.hour);
			putchar1(RtcMain.minute);
			putchar1(RtcMain.date);
			putchar1(RtcMain.month);
			putchar1(RtcMain.year);

			putchar1(getLampState());
			
			putchar1(Ubi>>8);
			putchar1(Ubi);

			putchar1(Ivp>>8);
			putchar1(Ivp);
			
			putchar1(100);

			//putchar1(energy>>24);
			putchar1(power_lamp>>16);
			putchar1(power_lamp>>8);
			putchar1(power_lamp);

			break;
			case 2:	//0x02
			//Lenh set real time
			RtcMain.hour	= args[0];
			RtcMain.minute	= args[1];
			RtcMain.date	= args[2];
			RtcMain.month	= args[3];
			RtcMain.year	= args[4];
			RtcMain.day		= args[5];//06/12/2010
			RtcMain.second	= 0;
			Write_DS1307(&RtcMain);
			sendNODECMD(2);
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
				//tempSendTime[i] = ((unsigned int)args[27+(i<<1)]<<8) + args[28+(i<<1)];
			}
			tempSchedule[8][2] = args[0];
			tempSchedule[8][3] = args[1];
			
			if(setSchedule(tempSchedule)){
				sendNODECMD(3);
				}else{
				//writeStringUART("Set schedule Faul ! \r\n");
			}
			break;
			case 4:	//0x04
			//Lenh yeu cau truyen schedule
			getSchedule(tempSchedule);

			sendNODECMD(4);

			for(i=0;i<9;i++){
				putchar1((char)tempSchedule[i][0]);
				putchar1((char)tempSchedule[i][1]);
				putchar1((char)tempSchedule[i][4]);
			}
			for(i=0;i<9;i++){
				putchar1(0x00);
				putchar1(60);
			}
			break;
			case 5:	//0x05: Reset system
			break;
			case 6:	//0x06
			// Lenh reset Energy - khong co noi dung
			break;
			case 7:	//0x07
			////Dat cac he so calib E,U,I
			break;
			case CMD_CHANGEID:	//0x08
			////Lenh doi ID
				eeprom_busy_wait();eeprom_write_byte(&node_ID,(uint8_t)args[0]);
				countWriteEEMEMcycle();
				sendNODECMD(CMD_CHANGEID);
				putchar1(eeprom_read_byte(&node_ID));
				break;
			case 10:	//0x09
			////Hoi ID cua node
			sendNODECMD(CMD_READID);
			putchar1(eeprom_read_byte(&node_ID));
			break;
			//case 'S':
			//	DDRG |= 1<<4;
			//	PORTG ^= 1<<4;
			break;
		}
//Task : When( 1s tick )
		// Update clock bar
		if(checkTick1s())
		{		// Soft IRQ (1s)

			wdt_reset();
			keyboardTimeout++;	// counter timeout for keyboard
			// FLAG REFRESH SCREEN TASK
			if(refreshSCR == REFRESH_ENB)	refreshSCR = REFRESH_FLAG;
			calculatingProcess();
			
//--------- READ RTC TASK -----------------------------------
			Read_DS1307(&RtcMain);
			if(RtcMain.year == eeprom_read_byte(&backupYear)){
				if(eeprom_read_byte(&fail_RTC)){
					eeprom_busy_wait();eeprom_write_byte(&fail_RTC,0); // flag notfail in RTC
					countWriteEEMEMcycle();
				}
			}
			else
			if(RtcMain.year == (eeprom_read_byte(&backupYear)+1)){	// newYear
				eeprom_busy_wait();eeprom_write_byte(&backupYear,RtcMain.year);
				countWriteEEMEMcycle();
			}else{
				if(RtcMain.year < eeprom_read_byte(&backupYear)){	// Realtime error
					if(eeprom_read_byte(&fail_RTC) == 0){
						eeprom_busy_wait();eeprom_write_byte(&fail_RTC,1); // flag fail in RTC
						countWriteEEMEMcycle();
						// reset Scheduler to default OFF
						for(i=0;i<9;i++){
							tempSchedule[i][0] = i+1;//18
							tempSchedule[i][1] = 0;//00
							tempSchedule[i][2] = i+2;//03
							tempSchedule[i][3] = 0;//21
							tempSchedule[i][4] = 0;//00
							//tempSendTime[i] = ((unsigned int)args[27+(i<<1)]<<8) + args[28+(i<<1)];
						}
						tempSchedule[8][2] = 1;
						tempSchedule[8][3] = 0;
						setSchedule(tempSchedule);
					}
				}
			}
// ---------clock bar display TASK-------------------------------------
			if(displayClockBar){
				clearROW(7);
				sprintf(str,"%02d:%02d:%02d  %02d/%02d/%02d",RtcMain.hour,RtcMain.minute,RtcMain.second/*,getDayStr(RtcMain.day)*/,RtcMain.date,RtcMain.month,RtcMain.year);
				f5x7_puts(0,7,str);
			}			
			//for(i = 0; i<9;i++){
			//	putchar1(historyScreen.historyQuece[i]);
			//}
// ------------SCHEDULE TASK---------------------------------
			if(isSchedule(&RtcMain)){
				changeLampState(checkCurrentTask());
				jumpNextTask();
			}	//End of Schedule Task
//	------------ NIMH BAT CHARGE TASK -------------------------------
			if(NiMH_voltage < 370)
			{
				changePWMT0(0x0E);
				sbi(BATCHG_DDR,BATCHAGER);
			}else
			{
				cbi(BATCHG_DDR,BATCHAGER);
				changePWMT0(0xE0);
			}
// ------- CHECK Pb BATTERY TASK -----------------
			if(getbatType()	== V24BAT){					// use 24 BATTERY.
				if((Ub1<getlowV())||(Ub2<getlowV())){		// battery is low
					supply_source = USE_MAIN;
					writeByte_NVRAM(supply_source,SUPPLY_SOURCE_ADDR);
					Ebat = 0;
					writelVal_NVRAM(&Ebat,BAT_ENERGY_ADDR);
					//sw_psu(PSU_ON);
					//sel_source_pw(SEL_MAIN);
					}else{
					if(((unsigned int)(Ebat/1000)>getEchg())||((Ub1>getchgV())&&(Ub2>getchgV()))){
						supply_source = USE_BAT;
						writeByte_NVRAM(supply_source,SUPPLY_SOURCE_ADDR);
						//sw_psu(PSU_OFF);
						//sel_source_pw(SEL_BATTERY);
					}
				}
			}else{
				if(Ub2<getlowV()){
					supply_source = USE_MAIN;
					writeByte_NVRAM(supply_source,SUPPLY_SOURCE_ADDR);
					//sw_psu(PSU_ON);
					//sel_source_pw(SEL_MAIN);
					}else{
					if(((unsigned int)(Ebat/1000)>getEchg())||(Ub2>getchgV())){
						supply_source = USE_BAT;
						writeByte_NVRAM(supply_source,SUPPLY_SOURCE_ADDR);
						//sw_psu(PSU_OFF);
						//sel_source_pw(SEL_BATTERY);
					}
				}
			}
			wdt_reset();
		}//end of if(checkTick1s())
//------CONTROL LAMP TASK -----------------------------
		taskManageLampState();
//------RE-LAYOUT SCREEN TASK -------------------------	
		if((getChangedInScr())||\
		(getRefreshFlag())){		// Auto Refresh (Polling refresh display)
			printSCREEN(getCurrentScreen());
		}
//------REFRESH LCD TASK--------------------------------
// when( refresh LCD )
		if(checkBufferChanged()){
			DisStartLine(0);
			UpdateDis();
		}					
    }
}