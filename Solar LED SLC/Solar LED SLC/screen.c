/*
 * screen.c
 *
 * Created: 30/10/2013 3:24:59 CH
 *  Author: SCEC
 */ 

/* ---------OBJECT */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "driver/uart.h"
#include "driver/ks0108.h"
#include "driver/ds1307.h"
#include "driver/relay.h"
#include "screen.h"
#include "schedule.h"
#include "base.h"
#include "main.h"
#include "settings.h"

#define	MAXSCREEN	20
#define TITLEIDX	0

static struct _RTCDATA	tempRTCdata;
unsigned int tmpLampPower;
unsigned int tmpNodeID=255;
unsigned int tmplowVpar=1000;
unsigned int EEMEM lowVolt = 1000;
unsigned int tmpchgVpar=1250;		// Vcharged  =  12,5V
unsigned int EEMEM chgVolt = 1250;	// Vcharged = 12,5V
unsigned int tmpEchVpar = 10;		// 
unsigned int EEMEM chgEner = 10;

prog_char sss[]			= "SOLAR SLC Task%u % 3u%%\0";
extern unsigned int adc_data[];
unsigned int tempX1,tempX2,tempY1,tempY2;
unsigned int EEMEM eechg1x1 = 150,eechg1y1 = 0,eechg1x2 = 300,eechg1y2 = 3790;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
unsigned int EEMEM eechg2x1 = 150,eechg2y1 = 0,eechg2x2 = 300,eechg2y2 = 3790;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
unsigned int EEMEM eeilampx1 = 66,eeilampy1 = 0,eeilampx2 = 510,eeilampy2 = 2060;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
unsigned int EEMEM eeUb1x1 = 780, eeUb1y1 = 1190,eeUbinx1 = 819, eeUbiny1 = 2400;

//unsigned char current_version[] PROGMEM = "10/2013";

unsigned char currentScreen = MAINSCR;				//	index of current Screen
unsigned char oldScreen = MAINSCR;

unsigned char selectedOBJ;
unsigned char idxSelectedObjScreen[MAXSCREEN];	//	index of Selected Object in current screen.	
unsigned char changedInScr = 0;

prog_char	mainlistObj[] = {1,MCTROBJ};
prog_char	menuListObj[] = {6,MCTROBJ,SCHEOBJ,RLTCOBJ,TERMOBJ,SETTOBJ,INFOOBJ};
prog_char	manualListObj[]		= {1,POWEOBJ};
prog_char	schedulerListObj[]	= {27,
	TAS0OBJ,TMM0OBJ,TPW0OBJ,
	TAS1OBJ,TMM1OBJ,TPW1OBJ,
	TAS2OBJ,TMM2OBJ,TPW2OBJ,
	TAS3OBJ,TMM3OBJ,TPW3OBJ,
	TAS4OBJ,TMM4OBJ,TPW4OBJ,
	TAS5OBJ,TMM5OBJ,TPW5OBJ,
	TAS6OBJ,TMM6OBJ,TPW6OBJ,
	TAS7OBJ,TMM7OBJ,TPW7OBJ,
	TAS8OBJ,TMM8OBJ,TPW8OBJ
};
prog_char	RTClistObj[]		= {7,HOUROBJ,MINUOBJ,SECOOBJ,DATEOBJ,MONTOBJ,YEAROBJ,DAY_OBJ};
prog_char	settingListObj[]	= {5,ID__OBJ,V124OBJ,LOWVOBJ,CHGVOBJ,ECHGOBJ};
prog_char	calibListObj[]		= {5,CHG1OBJ,CHG2OBJ,ILAMOBJ,UBINOBJ,UB01OBJ};
prog_char	Icharge1ListObj[]	= {4,PTX1OBJ,PTY1OBJ,PTX2OBJ,PTY2OBJ};
prog_char	Icharge2ListObj[]	= {4,PTX1OBJ,PTY1OBJ,PTX2OBJ,PTY2OBJ};
prog_char	IlampListObj[]		= {4,PTX1OBJ,PTY1OBJ,PTX2OBJ,PTY2OBJ};
prog_char	UbatInListObj[]		= {2,PTX1OBJ,PTY1OBJ};
prog_char	Ubat1ListObj[]		= {2,PTX1OBJ,PTY1OBJ};


struct _HISTORYSCR historyScreen;

void * listScreen[] PROGMEM= {
mainlistObj,menuListObj,
manualListObj,schedulerListObj,
RTClistObj,settingListObj,
calibListObj,Icharge1ListObj,
Icharge2ListObj,IlampListObj,UbatInListObj,Ubat1ListObj
};
//#define MAINSCR	0
//#define MENUSCR	1
//#define MCTRSCR	2
//#define SCHESCR	3
//#define RLTCSCR	4
//#define SETTSCR	5
//#define CALBSCR	6
//#define CHG1SCR	7
//#define CHG2SCR	8
//#define ILAMSCR	9
//#define UBINSCR	10
//#define UB01SCR	11
unsigned char listTitleScreen[] PROGMEM = {MAINOBJ,MENUOBJ,MCTROBJ,SCHEOBJ,RLTCOBJ,SETTOBJ,CALIOBJ,CHG1OBJ,CHG2OBJ,ILAMOBJ,UBINOBJ,UB01OBJ};

// -----------Object ----------------
// order as Object ID.
prog_char mainstr[]  = "Main Screen\0";
prog_char menustr[]  = "Menu\0";
prog_char mctrstr[]  = "Manual Control\0";
prog_char schestr[]  = "Scheduler\0";
prog_char rltcstr[]  = "Realtime Clock\0";
prog_char settstr[]  = "Setting\0";
prog_char powestr[]  = "Lamp Pwr: % 3u%%\0";
prog_char tas0str[]	 = "Task0|hh| %02u |\0";
prog_char tas1str[]  = "Task1|hh| %02u |\0";
prog_char tas2str[]  = "Task2|hh| %02u |\0";
prog_char tas3str[]  = "Task3|hh| %02u |\0";
prog_char tas4str[]  = "Task4|hh| %02u |\0";
prog_char tas5str[]  = "Task5|hh| %02u |\0";
prog_char tas6str[]  = "Task6|hh| %02u |\0";
prog_char tas7str[]  = "Task7|hh| %02u |\0";
prog_char tas8str[]  = "Task8|hh| %02u |\0";
prog_char secostr[]  = "Sec :%02u\0";
prog_char minustr[]  = "Min :%02u\0";
prog_char hourstr[]  = "Hour:%02u\0";
prog_char day_str[]  = "Day :%S\0";
prog_char datestr[]  = "Date:%02u\0";
prog_char montstr[]  = "Mont:%02u\0";
prog_char yearstr[]  = "Year:20%02u\0";
prog_char ID__str[]  = "ID node:% 3u\0";
prog_char NRIDstr[]  = "Neuron ID:%s\0";
prog_char V124str[]  = "Pb Bat:%02uV\0";
prog_char lowVstr[]  = "Vlow: %u,%01u V";//.%02uV\0";
prog_char chgVstr[]  = "Vchg: %u,%01u V";//.%02uV\0";			46
prog_char tmm0str[]	 = "     |mm| %02u |\0";		//TMM0OBJ	27
//prog_char tmm1str[]  = "     |mm| %02u |\0";		//TMM0OBJ	28
//prog_char tmm2str[]  = "     |mm| %02u |\0";		//TMM0OBJ	29
//prog_char tmm3str[]  = "     |mm| %02u |\0";		//TMM0OBJ	20
//prog_char tmm4str[]  = "     |mm| %02u |\0";		//TMM0OBJ	31
//prog_char tmm5str[]  = "     |mm| %02u |\0";		//TMM0OBJ	32
//prog_char tmm6str[]  = "     |mm| %02u |\0";		//TMM0OBJ	33
//prog_char tmm7str[]  = "     |mm| %02u |\0";		//TMM0OBJ	34
//prog_char tmm8str[]  = "     |mm| %02u |\0";		//TMM0OBJ	35 

prog_char tpw0str[]	 = "     |Pw|%3u%%|\0";		//TPW0OBJ	36
//prog_char tpw1str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	37
//prog_char tpw2str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	38
//prog_char tpw3str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	39
//prog_char tpw4str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	40
//prog_char tpw5str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	41
//prog_char tpw6str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	42
//prog_char tpw7str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	43
//prog_char tpw8str[]  = "     |Pw|%3u%%|\0";		//TPW0OBJ	44
prog_char termstr[]  = "Terminal\0";			//TERMOBJ	45
prog_char infostr[]  = "About\0";				//INFOOBJ	47
prog_char calbstr[]  = "Calibration\0";			//CALIOBJ	48
prog_char chg1str[]  = "Calib Icharge1\0";		//CHG1OBJ	49
prog_char chg2str[]  = "Calib Icharge2\0";		//CHG2OBJ	50
prog_char ilamstr[]  = "Calib Ilamp\0";			//ILAMOBJ	51
prog_char ptx1str[]  = "x1:%u";					//PTX1OBJ	52
prog_char pty1str[]  = "y1:%u";					//PTY1OBJ	53
prog_char ptx2str[]  = "x2:%u";					//PTX2OBJ	54
prog_char pty2str[]  = "y2:%u";					//PTY2OBJ	55
prog_char ubinstr[]  = "Calib Ubat_in\0";		//UBINOBJ	56
prog_char ub01str[]  = "Calib Ubat_01\0";		//UB01OBJ	57
prog_char Echgstr[]  = "Echg: % 3u0 Wh";			//.%02uV\0";	58

#define MENUTYPE	1
#define INT1TYPE	2		// var type interger with minimum val and maximum val
#define INT2TYPE	3		// object have 3 variable type interger
#define INT3TYPE	4		// object have 3 variable type interger
#define FLOATTYPE	5		//
#define LOOKINTTYPE	6		// var type interger with lookup table
#define LOOKSTRTYPE 7		// var type string	with lookup table

extern unsigned char EEMEM node_ID;

prog_char	powerlistVal[] = {0,50,60,70,80,90,100,255};

prog_char	battypelist[] = {12,24};
unsigned int tmpBatType=0;
unsigned char EEMEM batType = 1;

prog_char	_sunday[] = "Sun\0";
prog_char	_monday[] = "Mon\0";
prog_char	_tueday[] = "Tue\0";
prog_char	_wenday[] = "Wen\0";
prog_char	_thuday[] = "Thu\0";
prog_char	_friday[] = "Fri\0";
prog_char	_satday[] = "Sat\0";
void * ptrdaylookupStr[] PROGMEM = {_sunday,_sunday,_monday,_tueday,_wenday,_thuday,_friday,_satday};

unsigned int tmpSchedule[9][3];

// All parameter of each Object 
// Order is Object ID
menu_type	mainpar PROGMEM = {MENUTYPE,mainstr};
menu_type	menupar PROGMEM = {MENUTYPE,menustr};
menu_type	mctrpar PROGMEM = {MENUTYPE,mctrstr};
menu_type	schepar PROGMEM = {MENUTYPE,schestr};
menu_type	rltcpar PROGMEM = {MENUTYPE,rltcstr};
menu_type	settpar PROGMEM = {MENUTYPE,settstr};
lookup_type	powepar PROGMEM = {LOOKINTTYPE,powestr,&tmpLampPower,0,6,powerlistVal};
int_type	tas0par PROGMEM = {INT1TYPE,tas0str,&tmpSchedule[0][0],0,23};
int_type	tas1par PROGMEM = {INT1TYPE,tas1str,&tmpSchedule[1][0],0,23};
int_type	tas2par PROGMEM = {INT1TYPE,tas2str,&tmpSchedule[2][0],0,23};
int_type	tas3par PROGMEM = {INT1TYPE,tas3str,&tmpSchedule[3][0],0,23};
int_type	tas4par PROGMEM = {INT1TYPE,tas4str,&tmpSchedule[4][0],0,23};
int_type	tas5par PROGMEM = {INT1TYPE,tas5str,&tmpSchedule[5][0],0,23};
int_type	tas6par PROGMEM = {INT1TYPE,tas6str,&tmpSchedule[6][0],0,23};
int_type	tas7par PROGMEM = {INT1TYPE,tas7str,&tmpSchedule[7][0],0,23};
int_type	tas8par PROGMEM = {INT1TYPE,tas8str,&tmpSchedule[8][0],0,23};
int_type	secopar PROGMEM = {INT1TYPE,secostr,&tempRTCdata.second,0,59};
int_type	minupar PROGMEM = {INT1TYPE,minustr,&tempRTCdata.minute,0,59};
int_type	hourpar PROGMEM = {INT1TYPE,hourstr,&tempRTCdata.hour,0,23};
lookup_type	day_par PROGMEM = {LOOKSTRTYPE,day_str,&tempRTCdata.day,1,7,ptrdaylookupStr};
int_type	datepar PROGMEM = {INT1TYPE,datestr,&tempRTCdata.date,1,31};
int_type	montpar PROGMEM = {INT1TYPE,montstr,&tempRTCdata.month,1,12};
int_type	yearpar PROGMEM = {INT1TYPE,yearstr,&tempRTCdata.year,0,99};
int_type	ID__par PROGMEM = {INT1TYPE,ID__str,&tmpNodeID,0,255};
menu_type	NRIDpar PROGMEM = {MENUTYPE,NRIDstr};
lookup_type	V124par PROGMEM = {LOOKINTTYPE,V124str,&tmpBatType,0,1,battypelist};
int_type	lowVpar PROGMEM = {FLOATTYPE,lowVstr,&tmplowVpar,80,250};
int_type	chgVpar PROGMEM = {FLOATTYPE,chgVstr,&tmpchgVpar,80,300};

int_type	tmm0par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[0][1],0,59};
int_type	tmm1par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[1][1],0,59};
int_type	tmm2par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[2][1],0,59};
int_type	tmm3par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[3][1],0,59};
int_type	tmm4par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[4][1],0,59};
int_type	tmm5par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[5][1],0,59};
int_type	tmm6par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[6][1],0,59};
int_type	tmm7par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[7][1],0,59};
int_type	tmm8par PROGMEM = {INT1TYPE,tmm0str,&tmpSchedule[8][1],0,59};

lookup_type	tpw0par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[0][2],0,6,powerlistVal};
lookup_type	tpw1par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[1][2],0,6,powerlistVal};
lookup_type	tpw2par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[2][2],0,6,powerlistVal};
lookup_type	tpw3par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[3][2],0,6,powerlistVal};
lookup_type	tpw4par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[4][2],0,6,powerlistVal};
lookup_type	tpw5par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[5][2],0,6,powerlistVal};
lookup_type	tpw6par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[6][2],0,6,powerlistVal};
lookup_type	tpw7par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[7][2],0,6,powerlistVal};
lookup_type	tpw8par PROGMEM = {LOOKINTTYPE,tpw0str,&tmpSchedule[8][2],0,6,powerlistVal};
menu_type	termpar PROGMEM = {MENUTYPE,termstr};
menu_type	infopar	PROGMEM = {MENUTYPE,infostr};
menu_type	calbpar PROGMEM = {MENUTYPE,calbstr};										//calibration
menu_type	chg1par PROGMEM = {MENUTYPE,chg1str};
menu_type	chg2par PROGMEM = {MENUTYPE,chg2str};
menu_type	ilampar PROGMEM = {MENUTYPE,ilamstr};
int_type	ptx1par PROGMEM = {INT2TYPE,ptx1str,&tempX1,0,1024};					// I or U value
int_type	pty1par PROGMEM = {INT2TYPE,pty1str,&tempY1,0,15000};					// y* = ADC valule
int_type	ptx2par PROGMEM = {INT2TYPE,ptx2str,&tempX2,0,1024};					
int_type	pty2par PROGMEM = {INT2TYPE,pty2str,&tempY2,0,15000};
menu_type	ubinpar PROGMEM = {MENUTYPE,ubinstr};
menu_type	ub01par PROGMEM = {MENUTYPE,ub01str};
int_type	Echgpar PROGMEM = {INT1TYPE,Echgstr,&tmpEchVpar,1,600};	// 10Wh < Echg<6000Wh	

//Order as Object ID
const void * listObjPar[] PROGMEM= {
	&(mainpar),
	&(menupar),
	&(mctrpar),
	&(schepar),
	&(rltcpar),
	&(settpar),
	&(powepar),
	&(tas0par),
	&(tas1par),
	&(tas2par),
	&(tas3par),
	&(tas4par),
	&(tas5par),
	&(tas6par),
	&(tas7par),
	&(tas8par),
	&(secopar),
	&(minupar),
	&(hourpar),
	&(day_par),
	&(datepar),
	&(montpar),
	&(yearpar),
	&(ID__par),
	&(NRIDpar),
	&(V124par),
	&(lowVpar),
	&(tmm0par),
	&(tmm1par),
	&(tmm2par),
	&(tmm3par),
	&(tmm4par),
	&(tmm5par),
	&(tmm6par),
	&(tmm7par),
	&(tmm8par),
	&(tpw0par),
	&(tpw1par),
	&(tpw2par),
	&(tpw3par),
	&(tpw4par),
	&(tpw5par),
	&(tpw6par),
	&(tpw7par),
	&(tpw8par),
	&(termpar),
	&(chgVpar),
	&(infopar),
	&(calbpar),
	&(chg1par),
	&(chg2par),
	&(ilampar),
	&(ptx1par),
	&(pty1par),
	&(ptx2par),
	&(pty2par),
	&(ubinpar),
	&(ub01par),
	&(Echgpar)
};

void * listPtrObjVar[] PROGMEM = {
	&tasksEEP[0][0],
	&(tempRTCdata.second),
	&(tempRTCdata.minute),
	&(tempRTCdata.hour),
	&(tempRTCdata.day),
	&(tempRTCdata.date),
	&(tempRTCdata.month),
	&(tempRTCdata.year)
};


unsigned char getSelectedObj(){
	char * _ptrlistObjInScr;
	unsigned char _idxObj,_objID;
	_idxObj = idxSelectedObjScreen[currentScreen];
	_ptrlistObjInScr = (char *)pgm_read_word(&listScreen[currentScreen]);
	_objID = pgm_read_byte(&_ptrlistObjInScr[_idxObj + 1]);
	return _objID;
}

void incValSelObj(int _val){
	int_type * _objParAddr;
	int * _varAddr;
	int _MAXVAR;
	//char * _ptrlistObjInScr;
	unsigned char _objID;
	//_idxObj = idxSelectedObjScreen[currentScreen];
	//_ptrlistObjInScr = (char *)pgm_read_word(&listScreen[currentScreen]);
	//_objID = pgm_read_byte(&_ptrlistObjInScr[_idxObj + 1]);
	_objID = getSelectedObj();
	_objParAddr = (int_type *)pgm_read_word(&listObjPar[_objID]);
	_varAddr = (int *)pgm_read_word(&(_objParAddr->varAddr));
	_MAXVAR = pgm_read_word(&(_objParAddr->maxVar));
	if(*(_varAddr)<=(_MAXVAR-_val)){
		*(_varAddr) = *(_varAddr) + _val;
		
	}else{
		*(_varAddr) = _MAXVAR;
	}
	changedInScr = 1;
}
void desValSelObj(int _val){
	int_type * _objParAddr;
	int * _varAddr;
	int _MINVAR;
	char * _ptrlistObjInScr;
	unsigned char _idxObj,_objID;
	_idxObj = idxSelectedObjScreen[currentScreen];
	_ptrlistObjInScr = (char *)pgm_read_word(&listScreen[currentScreen]);
	_objID = pgm_read_byte(&_ptrlistObjInScr[_idxObj + 1]);
	_objParAddr = (int_type *)pgm_read_word(&listObjPar[_objID]);
	_varAddr = (int *)pgm_read_word(&(_objParAddr->varAddr));
	_MINVAR = pgm_read_word(&(_objParAddr->minVar));
	if(*_varAddr>=(_MINVAR + _val)){
		*(_varAddr) = *(_varAddr) - _val;
		
	}else{
		*(_varAddr) = _MINVAR;
	}
	changedInScr = 1;
}

void selectMovUp(void){
	unsigned char _idxObj;
	char * _ptrlistObjInScr;
	
	_idxObj = idxSelectedObjScreen[currentScreen];
	_ptrlistObjInScr = (char *)pgm_read_word(&listScreen[currentScreen]);
	
	if(idxSelectedObjScreen[currentScreen]>0){
		idxSelectedObjScreen[currentScreen]--;
		selectedOBJ = pgm_read_byte(&_ptrlistObjInScr[_idxObj + 1]);
		changedInScr = 1;	// mark have changed in Scr
	}
}
void selectMovDw(void){
	unsigned char _maxObjInScr,_idxObj;
	char * _ptrlistObjInScr;
	
	_idxObj = idxSelectedObjScreen[currentScreen];
	_ptrlistObjInScr = (char *)pgm_read_word(&listScreen[currentScreen]);
	_maxObjInScr = pgm_read_byte(&_ptrlistObjInScr[0]);
	
	if(idxSelectedObjScreen[currentScreen]<_maxObjInScr - 1){
		idxSelectedObjScreen[currentScreen]++;
		selectedOBJ = pgm_read_byte(&_ptrlistObjInScr[_idxObj + 1]);
		changedInScr = 1;	// mark have changed in Scr
	}
}

unsigned char getCurrentScreen(void){
	return currentScreen;
}



void changeCurrentScreen(unsigned char _IdxScr){
	oldScreen = currentScreen;
	currentScreen = _IdxScr;
	historyScreen.idxHistory++;
	historyScreen.historyQuece[historyScreen.idxHistory] = currentScreen;
	loadPar_Scr(currentScreen);
}

void toPreviousScreen(void){
	historyScreen.historyQuece[historyScreen.idxHistory] = 0;
	historyScreen.idxHistory--;
	oldScreen = currentScreen;
	currentScreen = historyScreen.historyQuece[historyScreen.idxHistory];
}

void toMainScreen(void){
	oldScreen = currentScreen;
	while(historyScreen.idxHistory){
		historyScreen.historyQuece[historyScreen.idxHistory] = 0;
		historyScreen.idxHistory--;
	}
	currentScreen = MAINSCR;
}
void setChangedInScr(){
	changedInScr = 1;
}
unsigned char getChangedInScr(void){
	if(currentScreen!=oldScreen){
		oldScreen = currentScreen;
		return 1;
	}else{
		if(changedInScr){
			changedInScr = 0;
			return 1;
		}
	}
	return 0;
}


//	return type number
//--#define MENUTYPE	1
//--#define INT1TYPE	2		// var type interger with minimum val and maximum val
//--#define LOOKINTTYPE	3		// var type interger with lookup table
//--#define LOOKSTRTYPE 4		// var type string	with lookup table
unsigned char checkObjtype(unsigned char _idxObj){
	return (unsigned char)pgm_read_byte((PGM_P)pgm_read_word(&listObjPar[_idxObj]));
}

//
void printObject(
	unsigned char _line,	// line: 0 - 6 
	unsigned char _idx,		// index in menu
	unsigned char _idxObj	// ID of object
){
	char str[25];
	unsigned char tmpVal;
	void * objParAddr;
	char ** _lkVarAddr;
	objParAddr = (void *)pgm_read_word(&listObjPar[_idxObj]);
	//sprintf(str,"%2u.", _idx);
	switch(checkObjtype(_idxObj)){
		case MENUTYPE:
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type*)objParAddr)->objStr)));
			break;
		case INT1TYPE:
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type*)objParAddr)->objStr)),\
				*((uchar*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr)))\
			);
			break;
		case INT2TYPE:
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type*)objParAddr)->objStr)),\
			*((uint*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr)))\
			);
		break;
		case FLOATTYPE:
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type*)objParAddr)->objStr)),\
			(*((uint*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr))))/10,\
			(*((uint*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr))))%10\
			);
		break;
		case LOOKINTTYPE:
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type *)objParAddr)->objStr)),\
				pgm_read_byte((PGM_P)(pgm_read_word(&(((lookup_type *)objParAddr)->lookupVar)) + *((uchar*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr)))))	\
			);
			break;
		case LOOKSTRTYPE:
			_lkVarAddr = (char **)pgm_read_word(&(((lookup_type *)objParAddr)->lookupVar));
			tmpVal = *((uchar*)pgm_read_word(&(((lookup_type *)objParAddr)->varAddr)));
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type *)objParAddr)->objStr)), pgm_read_word(&_lkVarAddr[tmpVal]) );
			break;
		case INT3TYPE:	
			sprintf_P(str,(PGM_P)pgm_read_word(&(((menu_type *)objParAddr)->objStr)),\
				*((uchar*)pgm_read_word(&(((int3_type *)objParAddr)->varAddr0))), \
				*((uchar*)pgm_read_word(&(((int3_type *)objParAddr)->varAddr1))), \
				*((uchar*)pgm_read_word(&(((int3_type *)objParAddr)->varAddr2)))
			);
			break;
	}
	
	if(_idx != 0){
		f5x7_puts(1,_line,str);
	}else{
		f5x7_puts(4,0,str);
	}		
}


void layoutScreen(void){
	unsigned char _line,_idx,nObj,_pageScr;
	char * _ptrlistObjScreen;
	for(_line = 0;_line<7;_line++){
		clearROW(_line);
	}
	_ptrlistObjScreen = (char *)pgm_read_word(&listScreen[currentScreen]);
	nObj = pgm_read_byte(&_ptrlistObjScreen[0]);
	//putchar1(nObj);
	printObject(0,TITLEIDX,pgm_read_byte(&listTitleScreen[currentScreen]));
	_pageScr = idxSelectedObjScreen[currentScreen] / 6;
	for(_idx =_pageScr*6, _line = 1;(_line<=6)&&(_idx<nObj);_line++,_idx++){
		printObject(_line,_idx+1,pgm_read_byte(&_ptrlistObjScreen[_idx+1]));
		if(_idx == idxSelectedObjScreen[currentScreen]){
			invertROW(_line);
		}
	}
}

void layoutMainScreen(void){
	unsigned char i;
	char str[25];
	// ------------ MAIN SCREEN LAYOUT TASK----------------------
	for(i=0;i<=6;i++){
		clearROW(i);
	}
	sprintf_P(str,sss,getCurrentTask(),getLampState());//checkCurrentTask());
	f5x7_puts(0,0,str);
		
	//Iacin = (adc_data[5]*17)/10;						//
	//Ivp = ((long)adc_data[1]*27803)/1000 - 14047;		//
	//sprintf(str,"NRID:%02X%02X%02X%02X%02X%02X",NRID_RAM[0],NRID_RAM[1],NRID_RAM[2],NRID_RAM[3],NRID_RAM[4],NRID_RAM[5]);
	//f5x7_puts(0,1,str);
	sprintf(str,"Ich:%u.%01uA",Ich2/1000,Ich2%1000/100);
	f5x7_puts(11,2,str);
	sprintf(str,"Umi:%02u.%uV Ubi:%02u.%uV",Umi/100,Umi%100/10,Ubi/100,Ubi%100/10);
	f5x7_puts(0,3,str);
	sprintf(str,"Ivp:%u.%02uA Uac:%uV",Ivp/1000,Ivp%1000/10,Umain);
	f5x7_puts(0,4,str); // 0x02D9 3V608
	sprintf(str,"Ub1:%02u.%uV Ub2:%02u.%uV",Ub1/100,Ub1%100/10,Ub2/100,Ub2%100/10);
	f5x7_puts(0,5,str); // 0x02D9 3V608
	displayEbat(str);
	f5x7_puts(0,2,str); // 0x02D9 3V608
	switch(psu_status){
		case PSU_ON:
		sprintf(str,"PSU_ON");
		break;
		case PSU_OFF:
		sprintf(str,"PSU_OFF");
		break;
	}
	f5x7_puts(0,6,str);
	switch(sel_source){
		case SEL_BATTERY:
		sprintf(str,"SEL_BATTERY");
		break;
		case SEL_MAIN:
		sprintf(str,"SEL_MAIN");
		break;
	}
	f5x7_puts(8,6,str);
	setRefreshable();
}

void layoutTermScreen(void){
	unsigned char _line,i,j=0;
	char str[25];
	unsigned char * _uartBuff;
	for(_line = 0;_line<7;_line++){
		clearROW(_line);
	}
	_uartBuff = getAddrUart1Buff();
	for(i=0;i<64;i+=8){
		sprintf(str,"%02X%02X %02X%02X %02X%02X %02X%02X",\
		_uartBuff[i],_uartBuff[i+1],_uartBuff[i+2],_uartBuff[i+3],_uartBuff[i+4],_uartBuff[i+5],_uartBuff[i+6],_uartBuff[i+7]);
		f5x7_puts(0,j++,str);
	}
}
void layoutInfoScreen(void){
	unsigned char _line;
	unsigned int i;
	char str[25];
	//unsigned char * _uartBuff;
	for(_line = 0;_line<7;_line++){
		clearROW(_line);
	}
	printObject(0,TITLEIDX,INFOOBJ);
	
	sprintf(str,"NRID:%02X%02X%02X%02X%02X%02X",NRID_RAM[0],NRID_RAM[1],NRID_RAM[2],NRID_RAM[3],NRID_RAM[4],NRID_RAM[5]);
	f5x7_puts(0,1,str);
	sprintf(str,"Ucel:%u0mV ID:%03u",NiMH_voltage,eeprom_read_byte(&node_ID));
	f5x7_puts(0,2,str);
	sprintf(str,"Version:%S",dateCompiler);
	f5x7_puts(0,3,str);
	if(eeprom_read_byte(&fail_RTC)){
		sprintf(str,"failRTC");
		f5x7_puts(0,4,str);
	}
	Read_NVRAM(&i,COUNT_WR_EEMEM_ADDR,2);
	sprintf(str,"EE:%u R:%d%d%d%d%d%d%d%d",i,BYTETOBINARY(mcucsr));
	//itoa(mcucsr,str+_line,2);
	f5x7_puts(0,5,str);
	setRefreshable();
}

void printSCREEN (unsigned char _idxSCR){
	//ClearDisBuf(0x00);
	char str[6];
	switch(_idxSCR){
		case MENUSCR:
		case MCTRSCR:
		case RLTCSCR:
		case SCHESCR:
		case SETTSCR:
		case CALBSCR:
			layoutScreen();
			break;
		case MAINSCR:
			layoutMainScreen();
			break;
		case TERMSCR:
			layoutTermScreen();
			break;
		case INFOSCR:
			layoutInfoScreen();
			break;
		case CHG1SCR:
			layoutScreen();
			sprintf(str,"%04u",adc_data[ADC_ICHR1]);
			f5x7_puts(0,6,str);
			setRefreshable();
			break;
		case CHG2SCR:
			layoutScreen();
			sprintf(str,"%04u",adc_data[ADC_ICHR2]);
			f5x7_puts(0,6,str);
			setRefreshable();
			break;
		case ILAMSCR:
			layoutScreen();
			sprintf(str,"%04u",adc_data[ADC_ILAMP]);
			f5x7_puts(0,6,str);
			setRefreshable();
			break;
		case UBINSCR:
		layoutScreen();
		sprintf(str,"%04u",adc_data[ADC_VBATT]);
		f5x7_puts(0,6,str);
		setRefreshable();
		break;
		case UB01SCR:
		layoutScreen();
		sprintf(str,"%04u",adc_data[ADC_VBAT1]);
		f5x7_puts(0,6,str);
		setRefreshable();
		break;
		default:
			break;
	}
}


void loadPar_Scr(unsigned char _IdxScr){
	unsigned char _tmpval,i,j;
	
	switch(_IdxScr){
		case MCTRSCR:
		_tmpval = getLampState();
		for(i = 6;i>=0;i--){
			if(_tmpval>=pgm_read_byte(&powerlistVal[i])){
				tmpLampPower = i;
				break;
				return;
			}
		}
		break;
		case SCHESCR:
		getTaskTable(tmpSchedule);
		for(j=0;j<9;j++){
			_tmpval = tmpSchedule[j][2];
			for(i = 6;i>=0;i--){
				if(_tmpval>=pgm_read_byte(&powerlistVal[i])){
					tmpSchedule[j][2] = i;
					break;
				}
			}
		}
		break;
		case RLTCSCR:
			Read_DS1307(&tempRTCdata);
		break;
		case SETTSCR:
			eeprom_busy_wait();
			tmpNodeID = eeprom_read_byte(&node_ID);
			eeprom_busy_wait();
			tmpBatType = eeprom_read_byte(&batType);
			eeprom_busy_wait();
			tmplowVpar = eeprom_read_word(&lowVolt)/10;
			eeprom_busy_wait();
			tmpchgVpar = eeprom_read_word(&chgVolt)/10;
			eeprom_busy_wait();
			tmpEchVpar = eeprom_read_word(&chgEner)/10;
		break;
		case TERMSCR:
			disClockBar();
			break;
		case CHG1SCR:
			eeprom_busy_wait();tempX1 = eeprom_read_word(&eechg1x1);
			eeprom_busy_wait();tempY1 = eeprom_read_word(&eechg1y1);
			eeprom_busy_wait();tempX2 = eeprom_read_word(&eechg1x2);
			eeprom_busy_wait();tempY2 = eeprom_read_word(&eechg1y2);
			break;
		case CHG2SCR:
			eeprom_busy_wait();tempX1 = eeprom_read_word(&eechg2x1);
			eeprom_busy_wait();tempY1 = eeprom_read_word(&eechg2y1);
			eeprom_busy_wait();tempX2 = eeprom_read_word(&eechg2x2);
			eeprom_busy_wait();tempY2 = eeprom_read_word(&eechg2y2);
			break;
		case ILAMSCR:
			eeprom_busy_wait();tempX1 = eeprom_read_word(&eeilampx1);
			eeprom_busy_wait();tempY1 = eeprom_read_word(&eeilampy1);
			eeprom_busy_wait();tempX2 = eeprom_read_word(&eeilampx2);
			eeprom_busy_wait();tempY2 = eeprom_read_word(&eeilampy2);
			break;
		case UBINSCR:
			eeprom_busy_wait();tempX1 = eeprom_read_word(&eeUbinx1);
			eeprom_busy_wait();tempY1 = eeprom_read_word(&eeUbiny1);
			break;
		case UB01SCR:
			eeprom_busy_wait();tempX1 = eeprom_read_word(&eeUb1x1);
			eeprom_busy_wait();tempY1 = eeprom_read_word(&eeUb1y1);
			break;
	}
}


void saveVarInScr(unsigned char _idxSCR){
unsigned char i;
	switch(_idxSCR){
		case RLTCSCR:
			Write_DS1307(&tempRTCdata);
			findoutCurrentTask();
			break;
		case MCTRSCR:
			changeLampState(pgm_read_byte(&powerlistVal[tmpLampPower]));
			break;
		case SCHESCR:
			for(i = 0;i < 9;i++){
				tmpSchedule[i][2] = pgm_read_byte(&powerlistVal[tmpSchedule[i][2]]);
			}
			int2char((unsigned int *)tmpSchedule,(unsigned char *)tmpSchedule,SIZE_TEMPTABLE);
			setTaskTable((unsigned char *)tmpSchedule);
			break;
		case SETTSCR:
			eeprom_busy_wait();eeprom_write_byte(&node_ID,tmpNodeID);
			eeprom_busy_wait();eeprom_write_word(&lowVolt,tmplowVpar*10);
			eeprom_busy_wait();eeprom_write_word(&chgVolt,tmpchgVpar*10);
			eeprom_busy_wait();eeprom_write_byte(&batType,(uchar)tmpBatType);
			eeprom_busy_wait();eeprom_write_word(&chgEner,tmpEchVpar*10);
			countWriteEEMEMcycle();
			break;
		case CHG1SCR:
			if((tempX1<tempX2)&&(tempY1<tempY2)){
			eeprom_busy_wait();eeprom_write_word(&eechg1x1,tempX1);
			eeprom_busy_wait();eeprom_write_word(&eechg1y1,tempY1);
			eeprom_busy_wait();eeprom_write_word(&eechg1x2,tempX2);
			eeprom_busy_wait();eeprom_write_word(&eechg1y2,tempY2);
			countWriteEEMEMcycle();
			}
			break;
		case CHG2SCR:
			if((tempX1<tempX2)&&(tempY1<tempY2)){
				eeprom_busy_wait();eeprom_write_word(&eechg2x1,tempX1);
				eeprom_busy_wait();eeprom_write_word(&eechg2y1,tempY1);
				eeprom_busy_wait();eeprom_write_word(&eechg2x2,tempX2);
				eeprom_busy_wait();eeprom_write_word(&eechg2y2,tempY2);
				countWriteEEMEMcycle();
			}
			break;
		case ILAMSCR:
			if((tempX1<tempX2)&&(tempY1<tempY2)){
				eeprom_busy_wait();eeprom_write_word(&eeilampx1,tempX1);
				eeprom_busy_wait();eeprom_write_word(&eeilampy1,tempY1);
				eeprom_busy_wait();eeprom_write_word(&eeilampx2,tempX2);
				eeprom_busy_wait();eeprom_write_word(&eeilampy2,tempY2);
				countWriteEEMEMcycle();
			}
			break;
		case UBINSCR:
			eeprom_busy_wait();eeprom_write_word(&eeUbinx1,tempX1);
			eeprom_busy_wait();eeprom_write_word(&eeUbiny1,tempY1);
			countWriteEEMEMcycle();
		break;
		case UB01SCR:
			eeprom_busy_wait();eeprom_write_word(&eeUb1x1,tempX1);
			eeprom_busy_wait();eeprom_write_word(&eeUb1y1,tempY1);
			countWriteEEMEMcycle();
		break;
	}
}
void * getDayStr(unsigned char _dayofWeek){
	return((char * )pgm_read_word(&ptrdaylookupStr[_dayofWeek]));
}
uchar getbatType(void){
	return eeprom_read_byte(&batType);
}
unsigned int getlowV(void){		// get low voltage (below threshold) 
	return eeprom_read_word(&lowVolt);
}
unsigned int getchgV(void){		// get charged voltage. (upper threshold)
	return eeprom_read_word(&chgVolt);
}
unsigned int getEchg(void){		// get charged voltage. (upper threshold)
	return eeprom_read_word(&chgEner);
}