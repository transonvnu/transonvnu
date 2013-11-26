#ifndef _ADE7758_INCLUDED_
#define _ADE7758_INCLUDED_ 

typedef struct {
	 long energy[3];
}EnergySruct; 

//low level funtions 
void CS_ADE(unsigned char state); //Chip Select for ADE7758 ; Active is 1
void initTransducer(void);
//unsigned char readTemperature();
long readVRMS(unsigned char pmNo);//Read Voltage RMS
long readIRMS(unsigned char pmNo);//Read Current RMS 
int readACTIVE_ENERGY(unsigned char pmNo);//Read Active energy  
void writeWattGain(int value,unsigned char pmNo);
int readREACTIVE_ENERGY(unsigned char pmNo);  
void writeReactiveGain(int value,unsigned char pmNo);
unsigned int readAPPARENT_ENERGY(unsigned char pmNo);
void writeApparentGain(int value,unsigned char pmNo);
void writeAENERGY(unsigned char pmNo, long newEnergy);
void writeGAIN(unsigned char value); 
void writeCMODE_REGISTER(void); 
void writeOMODE_REGISTER(void); //Frequency output enable
void writePhaseCalibration(char value,unsigned char pmNo); //Calibration phase for channelx
int readOffsetVol(unsigned char pmNo);
int readOffsetAmp(unsigned char pmNo);
void writeOffsetVolComp(int offset,unsigned char pmNo);
void writeOffsetAmpComp(int offset,unsigned char pmNo);
void offsetVolComp(unsigned char pmNo);
void offsetAmpComp(unsigned char pmNo);
//high level funtions
EnergySruct readTotalEnergy(void);
void resetTotalEnergy(void);
int readVoltage(unsigned char pmNo);
int readCurrent(unsigned char pmNo);
unsigned int readPhase(unsigned char pmNo);
void testCurrentCalib(int * iParam);
void setCalibrationParam(long * eParam,int * vParam,int * iParam);
void writeActivePowerScalling(unsigned int value);
void writeReactivePowerScalling(unsigned int value);
void writePhaseCorrection(char value,unsigned char channel);

#endif
