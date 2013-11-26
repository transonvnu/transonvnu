#ifndef _SDCARD_INCLUDE_
#define _SDCARD_INCLUDE_
#include "ds1307.h"

void pre_SD(void);
void writeLog(TimeStruct * Time,char * str);
void writeSystemLog(TimeStruct * Time,char SystemReset,char ModemReset,char OnOff,char ChangeSchedule,char ChangeTimeDate,char Setting,char ResetEnergy);
//void writeLog(void);
//void readSD(unsigned char date,unsigned char month,unsigned char year);
void readSD(unsigned char date,unsigned char month,unsigned char year,unsigned char hour1,unsigned char minute1,unsigned char hour2,unsigned char minute2);
void readSystemLog(unsigned char month,unsigned char year);
void copyFile(char * sourName,char * destName);
void initSpiInOut(void);
void initSdcard(void);
void initFirstTimeSDCard(void);
void deselectSdcard(void);
void deleteFile(char * Name);
unsigned long checkCapacitor(unsigned long* free,unsigned long* user);
#endif
