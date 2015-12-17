/*
ade7758.c
Library for interfacing ADE7753 power meter
Date created: 01/07/2008
*/

#include <avr/io.h>
#include <avr/eeprom.h>
#include <math.h>
#include <util/delay.h>
#include "spi.h"
#include "ds1307.h"
#include "ade7758.h" 
#include "timming.h"
//#include "lcd_8_bit.h" 
#include "uart.h"
#include "string1.h"
#include "timming.h"

#define BIT(x) (1<<x)

//ade7758 register commands
#define TEMP_READ_CMD		0x11
#define OMODE_READ_CMD		0x13
#define OMODE_WRITE_CMD		0x93
#define MMODE_READ_CMD		0x14
#define MMODE_WRITE_CMD		0x94
#define WMODE_READ_CMD		0x15
#define WMODE_WRITE_CMD		0x95
#define CMODE_READ_CMD		0x16        
#define CMODE_WRITE_CMD		0x96
#define GAIN_WRITE_CMD		0xA3   //write gain for PGA in the current and voltage channel
#define AIRMS_READ_CMD		0x0A   //read channel 1 current register
#define BIRMS_READ_CMD		0x0B   //read channel 2 current register
#define CIRMS_READ_CMD		0x0C   //read channel 3 current register
#define AVRMS_READ_CMD		0x0D   //read channel 1 voltage register 
#define BVRMS_READ_CMD		0x0E   //read channel 2 voltage register 
#define CVRMS_READ_CMD		0x0F   //read channel 3 voltage register 
#define AVRMSOS_WRITE_CMD	0xB3   //write channel 1 offset voltage register 
#define BVRMSOS_WRITE_CMD	0xB4   //write channel 2 offset voltage register 
#define CVRMSOS_WRITE_CMD	0xB5   //write channel 3 offset voltage register 
#define AIRMSOS_WRITE_CMD	0xB6   //write channel 1 offset current register 
#define BIRMSOS_WRITE_CMD	0xB7   //write channel 2 offset current register 
#define CIRMSOS_WRITE_CMD	0xB8   //write channel 3 offset current register 
#define AWATTHR_READ_CMD	0x01   //read channel 1 active energy register 
#define BWATTHR_READ_CMD	0x02   //read channel 2 active energy register
#define CWATTHR_READ_CMD	0x03   //read channel 3 active energy register
#define AVARHR_READ_CMD	    0x04   //read channel 1 reactive energy register 
#define BVARHR_READ_CMD	    0x05   //read channel 2 reactive energy register
#define CVARHR_READ_CMD	    0x06   //read channel 3 reactive energy register
#define AVAHR_READ_CMD	    0x07   //read channel 1 apparent energy register 
#define BVAHR_READ_CMD	    0x08   //read channel 2 apparent energy register
#define CVAHR_READ_CMD	    0x09   //read channel 3 apparent energy register                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
#define AWG_WRITE_CMD		0xAA   //write channel 1 watt gain
#define BWG_WRITE_CMD		0xAB   //write channel 2 watt gain
#define CWG_WRITE_CMD		0xAC   //write channel 3 watt gain 
#define AVARG_WRITE_CMD		0xAD   //write channel 1 reactive energy gain
#define BVARG_WRITE_CMD		0xAE   //write channel 2 reactive energy gain
#define CVARG_WRITE_CMD		0xAF   //write channel 3 reactive energy gain
#define AVAG_WRITE_CMD		0xB0   //write channel 1 apparent energy gain
#define BVAG_WRITE_CMD		0xB1   //write channel 2 apparent energy gain
#define CVAG_WRITE_CMD		0xB2   //write channel 3 apparent energy gain

#define APHCAL_WRITE_CMD	0xBF   //write channel 1 phase calibration
#define BPHCAL_WRITE_CMD	0xC0   //write channel 2 phase calibration
#define CPHCAL_WRITE_CMD	0xC1   //write channel 3 phase calibration
#define APCFDEN_WRITE_CMD	0xC6   //4Active power CF Scalling denominator Register
#define VARCFDEN_WRITE_CMD	0xC8   //Reactive power CF Scalling denominator Register

//consts
#define MAX_ENERGY_COUNT	14910000L		//maximum register count before reset
#define PM_KWH			10650L			//count to achieve 1 KWH
#define VOLT_CAL		818L
#define AMPERE_CAL		10
#define STATIC_VOLTAGE		500                     //unit VOLTAGE ADC of ADE7758
#define STATIC_CURRENT		100                     //unit CURRENT ADC of ADE7758
//private vars
long accEnergy[3] __attribute__ ((section (".init1")));
long accKWH[3]  __attribute__ ((section (".init1")));
unsigned char lastGain = 1;//,buffer[24];
long KWHCal[3] __attribute__ ((section (".init1"))) ;				//calibration for KWH
int voltCal[3] __attribute__ ((section (".init1")));				//calibration for voltage
int ampereCal[3] __attribute__ ((section (".init1")));				//calibration for current
unsigned int phase[3] = {100,100,100};
unsigned int lastVoltage[3] __attribute__ ((section (".init1")));
unsigned int lastCurrent[3] __attribute__ ((section (".init1")));

#define boxNumber   1
 
#define I_THRESHOLD 250

#if boxNumber == 1//
// **** Tu so 1 *********************************   
int offsetVol[3] = {-100,-88,-200};
int offsetAmp[3] = {-130,-130,-130};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 2//
// **** Tu so 2 *********************************   
int offsetVol[3] = {-280,-630,-700};
int offsetAmp[3] = {-135,-150,-150};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 3//
// **** Tu so 3 *********************************   
int offsetVol[3] = {-270,-430,-760};
int offsetAmp[3] = {-200,-150,-240};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 4//
// **** Tu so 4 *********************************   
int offsetVol[3] = {-290,-110,-525};
int offsetAmp[3] = {-160,-160,-170};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 5//
// **** Tu so 5 *********************************   
int offsetVol[3] = {-100,-768,-132};
int offsetAmp[3] = {-145,-145,-145};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 6//
// **** Tu so 6 *********************************   
int offsetVol[3] = {-164,-208,-144};
int offsetAmp[3] = {-140,-140,-135};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 7//
// **** Tu so 7 *********************************   
int offsetVol[3] = {-80,-884,-156};
int offsetAmp[3] = {-124,-138,-120};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 8//
// **** Tu so 8 *********************************   
int offsetVol[3] = {-308,-680,-484};
int offsetAmp[3] = {-153,-140,-145};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 9//
// **** Tu so 9 *********************************   
int offsetVol[3] = {-620,-307,-584};
int offsetAmp[3] = {-150,-160,-161};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 10//
// **** Tu so 10 *********************************   
int offsetVol[3] = {-508,-191,-428};
int offsetAmp[3] = {-147,-193,-215};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 11//
// **** Tu so 11 *********************************   
int offsetVol[3] = {-488,-88,-132};
int offsetAmp[3] = {-135,-135,-130};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 12//
// **** Tu so 12 *********************************   
int offsetVol[3] = {-452,-328,-292};
int offsetAmp[3] = {-135,-140,-130};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 13//
// **** Tu so 13 *********************************   
int offsetVol[3] = {-300,-156,-704};
int offsetAmp[3] = {-263,-210,-240};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 14//
// **** Tu so 14 *********************************   
int offsetVol[3] = {-880,-412,-360};
int offsetAmp[3] = {-148,-142,-140};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 15//
// **** Tu so 15 *********************************   
int offsetVol[3] = {-460,-188,-476};
int offsetAmp[3] = {-145,-150,-145};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 16//
// **** Tu so 16 *********************************   
int offsetVol[3] = {-148,-248,-376};
int offsetAmp[3] = {-130,-135,-137};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 17//
// **** Tu so 17 *********************************   
int offsetVol[3] = {-372,-96,-364};
int offsetAmp[3] = {-188,-190,-196};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 18//
// **** Tu so 18 *********************************   
int offsetVol[3] = {-320,-292,-300};
int offsetAmp[3] = {-149,-145,-148};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 19//
// **** Tu so 19 *********************************   
int offsetVol[3] = {-500,-68,-288};
int offsetAmp[3] = {-133,-135,-140};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 20//
// **** Tu so 20 *********************************   
int offsetVol[3] = {-244,-520,-164};
int offsetAmp[3] = {-130,-133,-129};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 21//
// **** Tu so 21 *********************************   
int offsetVol[3] = {-368,-412,-104};
int offsetAmp[3] = {-148,-140,-138};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 22//
// **** Tu so 22 *********************************   
int offsetVol[3] = {-744,-312,-244};
int offsetAmp[3] = {-140,-135,-134};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 23//
// **** Tu so 23 *********************************   
int offsetVol[3] = {-564,-256,-738};
int offsetAmp[3] = {-134,-135,-140};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 24//
// **** Tu so 24 *********************************   
int offsetVol[3] = {-208,-260,-520};
int offsetAmp[3] = {-133,-135,-141};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 25//
// **** Tu so 25 *********************************   
int offsetVol[3] = {-180,-364,-248};
int offsetAmp[3] = {-131,-142,-135};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 26//
// **** Tu so 26 *********************************   
int offsetVol[3] = {-544,-636,-304};
int offsetAmp[3] = {-149,-145,-240};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 27//
// **** Tu so 27 *********************************   
int offsetVol[3] = {-72,-288,-424};
int offsetAmp[3] = {-148,-143,-146};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 28//
// **** Tu so 28 *********************************   
int offsetVol[3] = {-380,-408,-720};
int offsetAmp[3] = {-160,-160,-149};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 29//
// **** Tu so 29 *********************************   
int offsetVol[3] = {-130,-410,-985};
int offsetAmp[3] = {-140,-140,-140};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 30//
// **** Tu so 30 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 31//
// **** Tu so 31 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 32//
// **** Tu so 32 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 33
// **** Tu so 33 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 34
// **** Tu so 34 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************
#if boxNumber == 35//
// **** Tu so 35 *********************************   
int offsetVol[3] = {0,0,0};
int offsetAmp[3] = {0,0,0};
int wattGain[3]  = {0,0,0};
#endif
//***********************************************

void CS_ADE(unsigned char state){//state = 1, spi is actived
    DDRB |= BIT(0);
    if(state) PORTB |= BIT(0);
    else      PORTB &= (~BIT(0));
}

void initTransducer(void){
	int i; 
	char buffer[24];	
	unsigned long CF_Scall;
	unsigned int TICoef1,TICoef2;
	CS_ADE(0);		//deactive all SPI channel	
	for (i = 0; i< 3; i++){
		accEnergy[i] = 0;
		lastVoltage[i] = 22000;
		lastCurrent[i] = 0;
	}
	_delay_ms(10);	
	readNVRAM(buffer,25,36);//25,26,27,28 --> Phase 1 ActiveEnergy
							//29,30,31,32 --> Phase 2 ActiveEnergy
							//33,34,35,36 --> Phase 3 ActiveEnergy
	
	//accKWH[0] = (((unsigned long)buffer[0]) << 24) + (((unsigned long)buffer[1]) << 16) + (((unsigned long)buffer[2]) << 8) + ((unsigned long)buffer[3]);
	//accKWH[1] = (((unsigned long)buffer[4]) << 24) + (((unsigned long)buffer[5]) << 16) + (((unsigned long)buffer[6]) << 8) + ((unsigned long)buffer[7]);
	//accKWH[2] = (((unsigned long)buffer[8]) << 24) + (((unsigned long)buffer[9]) << 16) + (((unsigned long)buffer[10]) << 8) + ((unsigned long)buffer[11]);
	
	accKWH[0] = (unsigned char)buffer[0];
	accKWH[0] <<= 8;
	accKWH[0] |= (unsigned char)buffer[1];
	accKWH[0] <<= 8;
	accKWH[0] |= (unsigned char)buffer[2];
	accKWH[0] <<= 8;
	accKWH[0] |= (unsigned char)buffer[3];	
	accKWH[1] = (unsigned char)buffer[4];
	accKWH[1] <<= 8;
	accKWH[1] |= (unsigned char)buffer[5];
	accKWH[1] <<= 8;
	accKWH[1] |= (unsigned char)buffer[6];
	accKWH[1] <<= 8;
	accKWH[1] |= (unsigned char)buffer[7];
	accKWH[2] = (unsigned char)buffer[8];
	accKWH[2] <<= 8;
	accKWH[2] |= (unsigned char)buffer[9];
	accKWH[2] <<= 8;
	accKWH[2] |= (unsigned char)buffer[10];
	accKWH[2] <<= 8;
	accKWH[2] |= (unsigned char)buffer[11];

	readEEPROM(buffer,48,71);
	
	//KWHCal[0] = (((unsigned long) buffer[3]) << 24) + (((unsigned long) buffer[2]) << 16) + (((unsigned long) buffer[1]) << 8) + ((unsigned long) buffer[0]);
	//KWHCal[1] = (((unsigned long) buffer[7]) << 24) + (((unsigned long) buffer[6]) << 16) + (((unsigned long) buffer[5]) << 8) + ((unsigned long) buffer[4]);
	//KWHCal[2] = (((unsigned long) buffer[11]) << 24) + (((unsigned long) buffer[10]) << 16) + (((unsigned long) buffer[9]) << 8) + ((unsigned long) buffer[8]);
	KWHCal[0] = eeprom_read_dword(ENERGY_CAL0);
	KWHCal[1] = eeprom_read_dword(ENERGY_CAL1);
	KWHCal[2] = eeprom_read_dword(ENERGY_CAL2);
	
	voltCal[0] = eeprom_read_word(VOL_CAL0);
	voltCal[1] = eeprom_read_word(VOL_CAL1);
	voltCal[2] = eeprom_read_word(VOL_CAL2);
	
	ampereCal[0] = eeprom_read_word(AMP_CAL0);
	ampereCal[1] = eeprom_read_word(AMP_CAL1);
	ampereCal[2] = eeprom_read_word(AMP_CAL2);
	/*
	KWHCal[0] = (unsigned char)buffer[3];
	KWHCal[0] <<= 8;
	KWHCal[0] |= (unsigned char)buffer[2];
	KWHCal[0] <<= 8;
	KWHCal[0] |= (unsigned char)buffer[1];
	KWHCal[0] <<= 8;
	KWHCal[0] |= (unsigned char)buffer[0];
	KWHCal[1] = (unsigned char)buffer[7];
	KWHCal[1] <<= 8;
	KWHCal[1] |= (unsigned char)buffer[6];
	KWHCal[1] <<= 8;
	KWHCal[1] |= (unsigned char)buffer[5];
	KWHCal[1] <<= 8;
	KWHCal[1] |= (unsigned char)buffer[4];
	KWHCal[2] = (unsigned char)buffer[11];
	KWHCal[2] <<= 8;
	KWHCal[2] |= (unsigned char)buffer[10];
	KWHCal[2] <<= 8;
	KWHCal[2] |= (unsigned char)buffer[9];
	KWHCal[2] <<= 8;
	KWHCal[2] |= (unsigned char)buffer[8];

	//voltCal[0] = (((int) buffer[13]) << 8) + ((int) buffer[12]);
	//voltCal[1] = (((int) buffer[15]) << 8) + ((int) buffer[14]);
	//voltCal[2] = (((int) buffer[17]) << 8) + ((int) buffer[16]);
	voltCal[0] = (unsigned char)buffer[13];
	voltCal[0] <<= 8;
	voltCal[0] |= (unsigned char)buffer[12];
	voltCal[1] = (unsigned char)buffer[15];
	voltCal[1] <<= 8;
	voltCal[1] |= (unsigned char)buffer[14];
	voltCal[2] = (unsigned char)buffer[17];
	voltCal[2] <<= 8;
	voltCal[2] |= (unsigned char)buffer[16];

	//ampereCal[0] = (((int) buffer[19]) << 8) + ((int) buffer[18]);
	//ampereCal[1] = (((int) buffer[21]) << 8) + ((int) buffer[20]);
	//ampereCal[2] = (((int) buffer[23]) << 8) + ((int) buffer[22]);
	ampereCal[0] = (unsigned char)buffer[19];
	ampereCal[0] <<= 8;
	ampereCal[0] |= (unsigned char)buffer[18];
	ampereCal[1] = (unsigned char)buffer[21];
	ampereCal[1] <<= 8;
	ampereCal[1] |= (unsigned char)buffer[20];
	ampereCal[2] = (unsigned char)buffer[23];
	ampereCal[2] <<= 8;
	ampereCal[2] |= (unsigned char)buffer[22];
	*/

	for(i=0;i<3;i++){//Add 06/08/2010
		if((KWHCal[i]<0)||(voltCal[i]<0)||(ampereCal[i]<0)||(KWHCal[i]>40000)||(voltCal[i]>1000)||(ampereCal[i]>1000)){
			KWHCal[0] = 22500;
			KWHCal[1] = 22500;
			KWHCal[2] = 22500;
			voltCal[0] = 540;
			voltCal[1] = 540;
			voltCal[2] = 540;
			ampereCal[0] = 555;//Vref = 125mVAC
			ampereCal[1] = 555;
			ampereCal[2] = 555;	
			setCalibrationParam(KWHCal,voltCal,voltCal);
			break;
		}
	}
	/*
	readEEPROM(buffer,72,75);
	TICoef1 = (unsigned char)buffer[1];
	TICoef1 <<= 8;
	TICoef1 |= (unsigned char)buffer[0];  

	TICoef2 = (unsigned char)buffer[3];
	TICoef2 <<= 8;
	TICoef2 |= (unsigned char)buffer[2];
	*/
	TICoef1 = eeprom_read_word(TICOEF_PAR1);
	TICoef2 = eeprom_read_word(TICOEF_PAR2);	

	KWHCal[0] = KWHCal[0]*40/(TICoef1 / TICoef2); 
	KWHCal[1] = KWHCal[1]*40/(TICoef1 / TICoef2); 
	KWHCal[2] = KWHCal[2]*40/(TICoef1 / TICoef2); 

	ampereCal[0] = ampereCal[0]*40/(TICoef1 / TICoef2); 
	ampereCal[1] = ampereCal[1]*40/(TICoef1 / TICoef2); 
	ampereCal[2] = ampereCal[2]*40/(TICoef1 / TICoef2); 

	initADE();
    for(i=0;i<3;i++){
        writeOffsetVolComp(offsetVol[i],i+1); 
        _delay_ms(1);  
        writeOffsetAmpComp(offsetAmp[i],i+1); 
        _delay_ms(1);
		writeWattGain(wattGain[i],i+1);
		//writeApparentGain(appaGain[i],i+1);
		_delay_ms(1);
    } 
	writeGAIN(3);//Vref = 125mVAC
    writeCMODE_REGISTER();    //Noload Threshold selected   
    writeOMODE_REGISTER();    	//Frequency output enable  
	readEEPROM(buffer,119,121);//read 3 bytes PHACALIB
	writePhaseCorrection(buffer[0],1);
	writePhaseCorrection(buffer[1],2);
	writePhaseCorrection(buffer[2],3);
	//CF_Scall = 612*(KWHCal[0]+KWHCal[1]+KWHCal[2])/306000;
	//CF_Scall = 263;
	//CF_Scall = 175;
	CF_Scall = 7000/TICoef1;//Vref = 125mVAC
	CF_Scall *= TICoef2;	// CF_Scall = 175*(200/5)/(TICoef1/TICoef2)
	writeActivePowerScalling((unsigned int)CF_Scall);
	writeReactivePowerScalling((unsigned int)CF_Scall);

}	

//--------------------------------------------
/*
unsigned char readTemperature(){
	unsigned char temp1, temp2;
	CS_ADE(1);    // ChipSelect
	_delay_ms(5);
	spi(OMODE_READ_CMD);
	temp1 = spi(0x00);
	temp1 = spi(0x00);
	temp2 = spi(0x00);				//read mode register
	temp2 |= 0x20;						//enable temperature conversion
	_delay_ms(2);
	spi(OMODE_WRITE_CMD);
	spi(temp1);
	spi(temp2);
	_delay_ms(1000);	
	spi(TEMP_READ_CMD);
	temp1 = spi(0x00);
	temp1 = spi(0x00);
	CS_ADE(0);		//deactive SPI channel	
	return temp1;	
}	*/
//--------------------------------------------
long readVRMS(unsigned char pmNo){ //Bang thuc nghiem: VRMS(max) = 4194304
	unsigned char temp,temp1;
	long vrms = 0; 
	CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		Delay(10);		
		temp1 = AVRMS_READ_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVRMS_READ_CMD;
	}
	else temp1 = CVRMS_READ_CMD;
	//_delay_ms(2); 	
	spi(temp1);
	temp = spi(0x00);
	//vrms = ((long)temp) * 0x10000;
	vrms = ((long)temp<<16);
	temp = spi(0x00);
	//vrms += ((long)temp) * 0x100;
	vrms |= ((long)temp<<8);
	temp = spi(0x00);
	//vrms += (long)temp;
	vrms |= (long)temp;	
	CS_ADE(0);		//deactive SPI channel		 	
	return vrms;
}	

//--------------------------------------------
long readIRMS(unsigned char pmNo){
	unsigned char temp=0,temp1=0;
	long irms = 0;
	CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		Delay(10);	
		temp1 = AIRMS_READ_CMD;
	}
	else if (pmNo == 2){
		temp1 = BIRMS_READ_CMD;
	}
	else    temp1 = CIRMS_READ_CMD;	        	
	spi(temp1);   
	temp = spi(0x00);       //MSBs first
	//irms = ((long)temp) * 0x10000;
	irms = ((long)temp<<16);
	temp = spi(0x00);
	//irms += ((long)temp) * 0x100;
	irms |= ((long)temp<<8);
	temp = spi(0x00);
	//irms += (long)temp;
	irms |= (long)temp; 
	CS_ADE(0);		//deactive SPI channel
	return irms;
}	

//--------------------------------------------
int readACTIVE_ENERGY(unsigned char pmNo){

	unsigned char temp,temp1;
    int aEnergy = 0;   
	CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AWATTHR_READ_CMD;
	}
	else if (pmNo == 2){
		temp1 = BWATTHR_READ_CMD;
	}
	else    temp1 = CWATTHR_READ_CMD; 		
	spi(temp1);
	temp = spi(0x00);       //MSBs fisrt
	//aEnergy = (int)temp* 0x100;
	aEnergy = ((int)temp<<8);
	temp = spi(0x00);
	//aEnergy += (int)temp;
	aEnergy |= (int)temp;	
	CS_ADE(0);		//deactive SPI channel		
	return aEnergy;
	
} 
void writeWattGain(int value,unsigned char pmNo){
    unsigned char temp,temp1;
    CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AWG_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BWG_WRITE_CMD;
	}
	else temp1 = CWG_WRITE_CMD;
	spi(temp1);
	temp = ((value>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (value&0xFF);
	spi(temp); 
	CS_ADE(0);	
} 
/*
int readREACTIVE_ENERGY(unsigned char pmNo){

	unsigned char temp,temp1;
    int aEnergy = 0;   
	CS_ADE(1);
	if (pmNo == 1){
		temp1 = AVARHR_READ_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVARHR_READ_CMD;
	}
	else    temp1 = CVARHR_READ_CMD; 		
	spi(temp1);
	temp = spi(0x00);       //MSBs fisrt
	aEnergy = (int)temp* 0x100;
	temp = spi(0x00);
	aEnergy += (int)temp;
	CS_ADE(0);		//deactive SPI channel	
	//if(aEnergy > 0x7FFF) aEnergy = 0;		
	return aEnergy;
	
}
void writeReactiveGain(int value,unsigned char pmNo){
    unsigned char temp,temp1;
    CS_ADE(1);
	if (pmNo == 1){
		temp1 = AVARG_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVARG_WRITE_CMD;
	}
	else temp1 = CVARG_WRITE_CMD;
	spi(temp1);
	temp = ((value>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (value&0xFF);
	spi(temp); 
	CS_ADE(0);	
}*/

unsigned int readAPPARENT_ENERGY(unsigned char pmNo){

	unsigned char temp,temp1;
    unsigned int aEnergy = 0;   
	CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AVAHR_READ_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVAHR_READ_CMD;
	}
	else    temp1 = CVAHR_READ_CMD; 		
	spi(temp1);
	temp = spi(0x00);       //MSBs fisrt
	//aEnergy = ((unsigned int)temp) * 0x100;
	aEnergy = (unsigned int)temp<<8;
	temp = spi(0x00);
	//aEnergy += (unsigned int)temp;
	aEnergy |= (unsigned int)temp;
	CS_ADE(0);		//deactive SPI channel			
	return aEnergy;
	
}
void writeApparentGain(int value,unsigned char pmNo){
    unsigned char temp,temp1;
    CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AVAG_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVAG_WRITE_CMD;
	}
	else temp1 = CVAG_WRITE_CMD;
	spi(temp1);
	temp = ((value>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (value&0xFF);
	spi(temp); 
	CS_ADE(0);	
}

int readOffsetVol(unsigned char pmNo){
	//unsigned char temp,temp1;
	long temp2;
	int offset;
	_delay_ms(2);
	temp2 = readVRMS(pmNo);  
	offset = (int)(((long)STATIC_VOLTAGE - temp2)/64); 
	return offset; 
	 			 		
}   

int readOffsetAmp(unsigned char pmNo){
	//unsigned char temp,temp1;
	long temp2;
	int offset;
	_delay_ms(2);
	temp2 = readIRMS(pmNo);  
	offset = (int)((STATIC_CURRENT*STATIC_CURRENT - temp2*temp2)/16384); 
	return offset;
			 		
}   
void writeOffsetVolComp(int offset,unsigned char pmNo){ 
    unsigned char temp,temp1;
    CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AVRMSOS_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BVRMSOS_WRITE_CMD;
	}
	else temp1 = CVRMSOS_WRITE_CMD;
	_delay_ms(2); 	
	spi(temp1);
	temp = ((offset>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (offset&0xFF);
	spi(temp); 
	CS_ADE(0);	
}
void writeOffsetAmpComp(int offset,unsigned char pmNo){
    unsigned char temp,temp1;
    CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = AIRMSOS_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BIRMSOS_WRITE_CMD;
	}
	else temp1 = CIRMSOS_WRITE_CMD;
	_delay_ms(2); 	
	spi(temp1);
	temp = ((offset>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (offset&0xFF);
	spi(temp); 
	CS_ADE(0);	
} 
void writeCMODE_REGISTER(void){ //Noload threshold
    unsigned char temp;
    CS_ADE(1);
    spi(CMODE_READ_CMD);
	temp = spi(0x00);     // Read defaul Value
	CS_ADE(0);
	temp |= 0x80;// + 0x20;  // Bit7 = 1:Actived No Load Threshold, BIT5 = 1 -> APCF sum of A,B,CWATTHR
	CS_ADE(1);
	spi(CMODE_WRITE_CMD); // Write new value
	spi(temp);
	CS_ADE(0);	
}
void writeOMODE_REGISTER(void){ //Frequency output enable
    unsigned char temp;
    CS_ADE(1);
    spi(OMODE_READ_CMD);
	temp = spi(0x00);     //Read defaul Value
	CS_ADE(0);
	temp &=(~0x04);       //Bit2 = 0:Frequency output enable
	CS_ADE(1);
	spi(OMODE_WRITE_CMD); // Write new value
	spi(temp);
	CS_ADE(0);	
}
void writePhaseCalibration(char value,unsigned char pmNo){ //Calibration phase for channelx       
    unsigned char temp1;
    CS_ADE(1);
	Delay(5);
	if (pmNo == 1){
		temp1 = APHCAL_WRITE_CMD;
	}
	else if (pmNo == 2){
		temp1 = BPHCAL_WRITE_CMD;
	}
	else temp1 = CPHCAL_WRITE_CMD;
	spi(temp1);
	spi(value); // Write value 
	CS_ADE(0);
}

//--------------------------------------------
void writeGAIN(unsigned char value){// current gain value = 1 or 4
	unsigned char gain;
	//unsigned char i;
	/*
	if((value == 1)&&(lastGain == 4)){
		gain = 0x00;
		for(i=0;i<3;i++){
			accEnergy[i] /= 4;
			ampereCal[i+1] /= 4;
			KWHCal[i+1] /= 4;  
			writeOffsetAmpComp(offsetAmp[i+1],i+1); 
			lastGain = 1;
		}
	}
	if((value == 4)&&(lastGain == 1)){
		gain = 0x02;
		for(i=0;i<3;i++){
			accEnergy[i] *= 4;
			ampereCal[i+1] *= 4;
			KWHCal[i+1] *= 4;
			writeOffsetAmpComp(4*offsetAmp[i+1],i+1); 
			lastGain = 4;
		}
	}*/
	if(value == 1){ //Vref = 500mVAC
		gain = 0x00;
	}
	if(value == 2){ //Vref = 250mVAC
		gain = 0x08;
	}
	if(value == 3){ //Vref = 125mVAC
		gain = 0x10;
	}		
	CS_ADE(1);
	spi(GAIN_WRITE_CMD);
	spi(gain);	
	CS_ADE(0);		//deactive SPI channel				
}	

//====================================================
//high level functions
EnergySruct readTotalEnergy(void){
	EnergySruct Energy,Falure;
	Falure.energy[0] = 0;
	Falure.energy[1] = 0;
	Falure.energy[2] = 0;
	long temp2;	//,totalKWH;
	int temp;      //temp of Active Energy
	unsigned int temp1,i;   //temp of Apparent Energy
	char buffer[12];
	long cosPhi;

	for (i = 0; i< 3; i++){

		if (KWHCal[i] == 0) return Falure;	

		temp = readACTIVE_ENERGY(i + 1);

		/*
		if(i==0)
			DisplayNumber(1,117, (long)temp);
		if(i==1)
			DisplayNumber(21,117,(long)temp);
		if(i==2)
			DisplayNumber(41,117,(long)temp);
		//DisplayNumber(71+25*i,117,(long)accEnergy[i]);
		*/
    	temp1 = readAPPARENT_ENERGY(i + 1); 
		if(temp < 80){
			temp = 0;
		}
		accEnergy[i] += temp;
			
		if(accEnergy[i] >= KWHCal[i]){
			accKWH[i] += (accEnergy[i] / KWHCal[i]);
			accEnergy[i] %= KWHCal[i];//Lay phan du de loai tru sai so tich luy
		}
		Energy.energy[i] = accKWH[i];
		if (temp1 < 50) phase[i] = 100;
	    else{
			cosPhi = (long)100*(long)temp;
			cosPhi /= (long)temp1;
		    if (cosPhi <= 0) cosPhi = 0;     //Bao lap nguoc day TI
		    if (cosPhi > 100) cosPhi = 100.0;
		    if (lastCurrent[i] < I_THRESHOLD) cosPhi = 100.0;
		    phase[i] = (int)cosPhi; 
	    } 
	}	
	//write current Energy to NVRAM
	temp2 = accKWH[0];
	buffer[0] = temp2 >> 24;
	temp2 &= 0xffffff;
	buffer[1] = temp2 >> 16;
	temp2 &= 0xffff;
	buffer[2] = temp2 >> 8;
	temp2 &= 0xff;
	buffer[3] = temp2;
	
	temp2 = accKWH[1];
	buffer[4] = temp2 >> 24;
	temp2 &= 0xffffff;
	buffer[5] = temp2 >> 16;
	temp2 &= 0xffff;
	buffer[6] = temp2 >> 8;
	temp2 &= 0xff;
	buffer[7] = temp2;
	
	temp2 = accKWH[2];
	buffer[8] = temp2 >> 24;
	temp2 &= 0xffffff;
	buffer[9] = temp2 >> 16;
	temp2 &= 0xffff;
	buffer[10] = temp2 >> 8;
	temp2 &= 0xff;
	buffer[11] = temp2;
	
	writeNVRAM(buffer,25,36);//Write NVRAM

	return Energy;
}	

//-------------------------------
void resetTotalEnergy(void){	
	int i;	
	char buffer[12];
	for (i = 0; i< 3; i++){
		accKWH[i] = 0;
	}	
	//readNVRAM(buffer);	
	for (i = 0; i< 12; i++){
		buffer[i] = 0;
	}	
	writeNVRAM(buffer,25,36);
}	

//-------------------------------
int readVoltage(unsigned char pmNo){
	int vrms;
	if (voltCal[pmNo-1] == 0) return 0;
	vrms = (int) (readVRMS(pmNo)*10 / voltCal[pmNo-1]);
	//vrms = (int) (readVRMS(pmNo) / 56);
	if (vrms < 200) return 0;
	//vrms = (lastVoltage[pmNo - 1]  + vrms) >> 1 ;
	lastVoltage[pmNo - 1] = vrms;
	if (vrms < 1000) return 0;//10V
	else return vrms;
	//return vrms;
}	

//-------------------------------
int readCurrent(unsigned char pmNo){
	int irms;
//	int max = 0;
//	unsigned char i;
	if (ampereCal[pmNo-1] == 0) return 0;
	irms = (int) (readIRMS(pmNo)*10 / ampereCal[pmNo-1]);
	/*
	for(i=0;i<3;i++){
		if(lastCurrent[i] >= max) max = lastCurrent[i];
	}
	if((max < 300)&&(lastGain == 1)){ // 3 ampere
		writeGAIN(4);
	}
	if((max >= 400)&&(lastGain == 4)){ // 4 ampere
		writeGAIN(1);
	}*/
	if (irms < 10) return 0;
	//irms = (lastCurrent[pmNo - 1] + irms) >> 1;
	lastCurrent[pmNo - 1] = irms;
	if (irms < I_THRESHOLD) return 0;//2A
	else return irms;
	//return irms;
}	

//--------------------------------------------
unsigned int readPhase(unsigned char pmNo){
	return phase[pmNo - 1];
}

void testCurrentCalib(int * iParam){
	unsigned int TICoef1,TICoef2;
	char buffer[5];

	readEEPROM(buffer,72,75);

	TICoef1 = (unsigned char)buffer[1];
	TICoef1 <<= 8;
	TICoef1 |= (unsigned char)buffer[0];  

	TICoef2 = (unsigned char)buffer[3];
	TICoef2 <<= 8;
	TICoef2 |= (unsigned char)buffer[2]; 

	ampereCal[0] = iParam[0];
	ampereCal[1] = iParam[1];
	ampereCal[2] = iParam[2];
	ampereCal[0] = ampereCal[0]*40/(TICoef1 / TICoef2); 
	ampereCal[1] = ampereCal[1]*40/(TICoef1 / TICoef2); 
	ampereCal[2] = ampereCal[2]*40/(TICoef1 / TICoef2); 
}
//-------------------------------
void setCalibrationParam(long * eParam,int * vParam,int * iParam){
	unsigned int temp,TICoef1,TICoef2;
	unsigned long eTemp;
	char buffer[24];
	readEEPROM(buffer,72,75);
	TICoef1 = (unsigned char)buffer[1];
	TICoef1 <<= 8;
	TICoef1 |= (unsigned char)buffer[0];  

	TICoef2 = (unsigned char)buffer[3];
	TICoef2 <<= 8;
	TICoef2 |= (unsigned char)buffer[2]; 	
	
	KWHCal[0] = eParam[0];
	KWHCal[1] = eParam[1];
	KWHCal[2] = eParam[2];
	KWHCal[0] = KWHCal[0]*40/(TICoef1 / TICoef2); 
	KWHCal[1] = KWHCal[1]*40/(TICoef1 / TICoef2); 
	KWHCal[2] = KWHCal[2]*40/(TICoef1 / TICoef2); 

	voltCal[0] = vParam[0];
	voltCal[1] = vParam[1];
	voltCal[2] = vParam[2];

	ampereCal[0] = iParam[0];
	ampereCal[1] = iParam[1];
	ampereCal[2] = iParam[2];
	ampereCal[0] = ampereCal[0]*40/(TICoef1 / TICoef2); 
	ampereCal[1] = ampereCal[1]*40/(TICoef1 / TICoef2); 
	ampereCal[2] = ampereCal[2]*40/(TICoef1 / TICoef2); 
	
	eTemp = eParam[0];
	buffer[0] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[1] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[2] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[3] = eTemp & 0xff;

	eTemp = eParam[1];
	buffer[4] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[5] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[6] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[7] = eTemp & 0xff;

	eTemp = eParam[2];
	buffer[8] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[9] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[10] = eTemp & 0xff;
	eTemp >>= 8;
	buffer[11] = eTemp & 0xff;

	temp = vParam[0];
	buffer[12] = temp & 0xFF;
	buffer[13] = (temp >> 8) & 0xFF;
	
	temp = vParam[1];
	buffer[14] = temp & 0xFF;
	buffer[15] = (temp >> 8) & 0xFF;
	
	temp = vParam[2];
	buffer[16] = temp & 0xFF;
	buffer[17] = (temp >> 8) & 0xFF;
	
	temp = iParam[0];
	buffer[18] = temp & 0xFF;
	buffer[19] = (temp >> 8) & 0xFF;
	
	temp = iParam[1];
	buffer[20] = temp & 0xFF;
	buffer[21] = (temp >> 8) & 0xFF;
	
	temp = iParam[2];
	buffer[22] = temp & 0xFF;
	buffer[23] = (temp >> 8) & 0xFF;
	
	writeEEPROM(buffer,48,71);
}

void writeActivePowerScalling(unsigned int value){
	unsigned char temp;
	if(value < 1)
		value = 1;
    CS_ADE(1);
	spi(APCFDEN_WRITE_CMD);
	temp = ((value>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (value&0xFF);
	spi(temp); 
	CS_ADE(0);
}
void writeReactivePowerScalling(unsigned int value){
	unsigned char temp;
	if(value < 1)
		value = 1;
    CS_ADE(1);
	spi(VARCFDEN_WRITE_CMD);
	temp = ((value>>8)&0xFF);             //MSB first
	spi(temp);
	temp = (value&0xFF);
	spi(temp); 
	CS_ADE(0);
}
void writePhaseCorrection(char value,unsigned char channel){
	unsigned char temp;
	if(channel == 1)
		temp = APHCAL_WRITE_CMD;
	if(channel == 2)
		temp = BPHCAL_WRITE_CMD;
	if(channel == 3)
		temp = CPHCAL_WRITE_CMD;
    CS_ADE(1);
    spi(temp);
	temp = value;
	spi(temp);     //Read defaul Value
	CS_ADE(0);	
}

