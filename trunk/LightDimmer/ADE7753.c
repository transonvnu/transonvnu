/*
transducerADE.c
Library for interfacing ADE7753 power meter
Date created: 01/15/2007
*/

#include <avr/io.h>
#include <util/delay.h>
//#include <math.h>
#include "ADE7753.h"
#include "spi.h"
#include "timming.h"
#include "ds1307.h"
//register commands
//#define TEMP_READ_CMD		0x26
//#define MODE_READ_CMD		0x09
//#define MODE_WRITE_CMD		0x89
#define IRMS_READ_CMD		0x16
#define VRMS_READ_CMD		0x17
#define AENERGY_READ_CMD	0x02
//#define AENERGY_WRITE_CMD	0x82
#define RAENERGY_READ_CMD	0x03
#define RVAENERGY_READ_CMD	0x06
#define	LAENERGY_READ_CMD	0x04
#define	LVAENERGY_READ_CMD	0x07
//#define GAIN_WRITE_CMD		0x8f

//#define IRMSOS_WRITE_CMD	0x98
#define VRMSOS_WRITE_CMD	0x99
//#define CH1OS_WRITE_CMD		0x8D
#define CH2OS_WRITE_CMD		0x8E
#define VAGAIN_WRITE_CMD	0x9A

#define I_THRESHOLD 100
#define U_THRESHOLD 2000
	
#define BIT(x) (1<<x)

//private vars
static unsigned long accEnergy;
static unsigned long accKWH;
static char buffer[15];
static unsigned long KWHCal;				//calibration for KWH
static unsigned int voltCal;				//calibration for voltage
static unsigned int ampereCal;				//calibration for current
static int 			phase = 100;
static unsigned int lastVoltage[] = {22000, 22000, 22000, 22000, 22000, 22000, 22000, 22000};
static int 			lastCurrent;

void resetADE(void){
	DDRB |= BIT(1);
	PORTB &= (~BIT(1));
	_delay_ms(50);
	PORTB |= BIT(1);
	_delay_ms(50);
}
void chip_select(unsigned char state){
	DDRB |= BIT(2);
	if(state == 1){
		PORTB &= (~BIT(2));
	}else {
		PORTB |= BIT(2);
	}
	//delay(200);
	//_delay_ms(1);			
}
void initTransducer(void){	
	chip_select(1);
	resetADE();
	initSPI();
	readRAENERGY();	//flush energy register
	accEnergy = 0;
	//lastVoltage = 2200;
	lastCurrent = 0;

	readNVRAM(buffer,0,3);
	accKWH = (((long)buffer[3]) << 24) + (((long)buffer[2]) << 16) + (((long)buffer[1]) << 8) + ((long)buffer[0]);
	
	readEEPROM(buffer,63,70);
	KWHCal = (((long) buffer[3]) << 24) + (((long) buffer[2]) << 16) + (((long) buffer[1]) << 8) + ((long) buffer[0]);
	voltCal = (((int) buffer[5]) << 8) + ((int) buffer[4]);
	ampereCal = (((int) buffer[7]) << 8) + ((int) buffer[6]);

	if((KWHCal<0)||(voltCal<0)||(ampereCal<0)||(KWHCal>40000)||(voltCal>1000)||(ampereCal>2000)){
		KWHCal = 1000;
		voltCal = 52;
		ampereCal = 400;
		setCalibrationParam(KWHCal,voltCal,voltCal);
	}
	chip_select(0);
}	
/*
//--------------------------------------------
unsigned char readTemperature(void){
	unsigned char temp;
	//delayMs(2);
	spi(MODE_WRITE_CMD);
	spi(0x00);
	spi(0x28);         //Request read Temperature
	delayMs(2);	
	spi(TEMP_READ_CMD);
	temp = spi(0x00);
	return temp;	
}	
*/
//--------------------------------------------
long readVRMS(void){
	unsigned char temp;
	long vrms = 0;
	chip_select(1);
	spi(VRMS_READ_CMD);
	temp = spi(0x00);
	vrms += ((long)temp) << 16;
	temp = spi(0x00);
	vrms += ((long)temp) << 8;
	temp = spi(0x00);
	vrms += temp;	
	chip_select(0);
	return vrms;
}	

void writeVOffset(unsigned int value){
	chip_select(1);
	spi(VRMSOS_WRITE_CMD);
	spi((value>>8)&0xFF);
	spi(value&0xFF);
	chip_select(0);
}
void writeCH2Offset(unsigned char value){
	chip_select(1);
	spi(CH2OS_WRITE_CMD);
	spi(value&0x3F);
	chip_select(0);
}
//--------------------------------------------
long readIRMS(void){
	unsigned char temp;
	long irms = 0;
	chip_select(1);
	spi(IRMS_READ_CMD);
	temp = spi(0x00);
	irms += ((long)temp) << 16;
	temp = spi(0x00);
	irms += ((long)temp) << 8;
	temp = spi(0x00);
	irms += temp;	
	chip_select(0);
	return irms;
}	
/*
void writeIOffset(unsigned int value){
	spi(IRMSOS_WRITE_CMD);
	spi((value>>8)&0xFF);
	spi(value&0xFF);
}
void writeCH1Offset(unsigned char value){
	spi(CH1OS_WRITE_CMD);
	spi(value&0x3F);
}*/
//--------------------------------------------
/*long readAENERGY(void){
	unsigned char temp;
	long aEnergy = 0;
	chip_select(1);
	spi(AENERGY_READ_CMD);
	temp = spi(0x00);
	aEnergy += ((long)temp) << 16; //MSB First
	temp = spi(0x00);
	aEnergy += ((long)temp) << 8;
	temp = spi(0x00);
	aEnergy += temp;
	chip_select(0);		
	return aEnergy;	
}

//--------------------------------------------

void writeAENERGY(long newEnergy){
	unsigned char temp;
	spi(AENERGY_WRITE_CMD);
	temp = newEnergy & 0xff;
	spi(temp);
	newEnergy >>= 8;
	temp = newEnergy & 0xff;
	spi(temp);
	newEnergy >>= 8;
	temp = newEnergy & 0xff;
	spi(temp);	
}*/	

//--------------------------------------------
long readRAENERGY(void){
	unsigned char temp;
	long aEnergy = 0, vaEnergy = 0;
	int cosPhi;	
	chip_select(1);
	spi(RAENERGY_READ_CMD);
	temp = spi(0x00);
	aEnergy += ((long)temp) << 16;
	temp = spi(0x00);
	aEnergy += ((long)temp) << 8;
	temp = spi(0x00);
	aEnergy += temp;	
	chip_select(0);
	chip_select(1);
	spi(RVAENERGY_READ_CMD);
	temp = spi(0x00);
	vaEnergy += ((long)temp) << 16;
	temp = spi(0x00);
	vaEnergy += ((long)temp) << 8;
	temp = spi(0x00);
	vaEnergy += temp;
	chip_select(0);
	
	if (vaEnergy < 600) phase = 100;//Load below 3 Watt
	else{
		cosPhi = (int) ((aEnergy * 100) / vaEnergy);
		if (cosPhi < 0) cosPhi = 0;
		if ((cosPhi >= 100)||(lastCurrent < I_THRESHOLD)) cosPhi = 100;
		phase = cosPhi;
	}
	if(aEnergy < 300){//Load below 3 Watt
		return 0;
	}else{
		return aEnergy;
	}	
}
/*
//--------------------------------------------
long readRVAENERGY(void){
	unsigned char temp;
	long aEnergy = 0;
	spi(RVAENERGY_READ_CMD);
	//temp = spi(0x00);
	temp = spi(0x00);
	aEnergy += ((long)temp) * 0x10000;
	temp = spi(0x00);
	aEnergy += ((long)temp) * 0x100;
	temp = spi(0x00);
	aEnergy += temp;	
	return aEnergy;	
}*/
void writeVAGAIN(unsigned int gain){
	chip_select(1);
	spi(VAGAIN_WRITE_CMD);
	spi((gain>>8)&0xFF);
	spi(gain&0xFF);
	chip_select(0);	
}/*
//--------------------------------------------
void writeGAIN(unsigned char gainValue){	
	spi(GAIN_WRITE_CMD);
	spi(gainValue);
}*/	

//====================================================
//high level functions
long readEnergy(void){
	long temp;
	if (KWHCal == 0) return 0;	
	temp = readRAENERGY();
	if ((temp > 100000)||(temp < 0)) temp = 0;
	accEnergy += temp;
	if(accEnergy >= KWHCal){
		accKWH += accEnergy/KWHCal;
		accEnergy %= KWHCal;
	}
	temp = accKWH;
	buffer[3] = temp >> 24;
	temp &= 0xffffff;
	buffer[2] = temp >> 16;
	temp &= 0xffff;
	buffer[1] = temp >> 8;
	temp &= 0xff;
	buffer[0] = temp;
	writeNVRAM(buffer,0,3);
	return accKWH;
}
//-------------------------------
/*void resetEnergy(void){
	int i;
	accKWH = 0;		
	for (i = 0; i< 4; i++){
		buffer[i] = 0;
	}	
	writeNVRAM(buffer,0,3);
}*/	
//-------------------------------
int readVoltage(void){
	long vrms = 0;
	unsigned char i;
	if (voltCal == 0) return 0;
	for(i = 7;i>0;i--){
		lastVoltage[i] = lastVoltage[i-1];	// shift quece
		vrms = vrms + lastVoltage[i];				//
	}
	lastVoltage[i] = (int) (readVRMS() / voltCal);
	vrms = (vrms + lastVoltage[i])>>3;
	if (vrms < U_THRESHOLD) return 0;//20V
	else return vrms;
	
}	
//-------------------------------
int readCurrent(void){
	long irms;
	if (ampereCal == 0) return 0;
	irms = (int) (readIRMS() / ampereCal);
	//irms = (int) (readIRMS() / 500);
	//irms = (lastCurrent + irms) >> 1;
	lastCurrent = (int)irms;
	if (irms < I_THRESHOLD) return 0;//100mA
	else return irms;

}	
//--------------------------------------------
int readPhase(void){
	return phase;
}
//-------------------------------
void setCalibrationParam(unsigned long eParam, unsigned int vParam, unsigned int iParam){
	unsigned int temp;
	unsigned long eTemp;	
	KWHCal = eParam;
	voltCal = vParam;
	ampereCal = iParam;
	eTemp = eParam;
	buffer[0] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[1] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[2] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[3] = eTemp & 0xff;	
	temp = vParam;
	buffer[5] = temp >> 8;
	temp &= 0xff;
	buffer[4] = temp;	
	temp = iParam;
	buffer[7] = temp >> 8;
	temp &= 0xff;
	buffer[6] = temp;		
	writeEEPROM(buffer,63,70);
}
