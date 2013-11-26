/*
transducerADE.h
Library for interfacing ADE7753 power meter
Date created: 01/15/2007

*/

#ifndef ADE7753_H
#define ADE7753_H

//low level funtions
void resetADE(void);
void chip_select(unsigned char state);
void initTransducer(void);
unsigned char readTemperature(void);
long readVRMS(void);
void writeVOffset(unsigned int value);
void writeCH2Offset(unsigned char value);
long readIRMS(void);
void writeIOffset(unsigned int value);
void writeCH1Offset(unsigned char value);
long readAENERGY(void);
void writeAENERGY(long newEnergy);
long readRAENERGY(void);
long readRVAENERGY(void);
void writeVAGAIN(unsigned int gain);
void writeGAIN(unsigned char gainValue);

//high level funtions
long readEnergy(void);
//void resetEnergy(void);
int readVoltage(void);
int readCurrent(void);
int readPhase(void);
void setCalibrationParam(unsigned long eParam, unsigned int vParam, unsigned int iParam);

#endif 
