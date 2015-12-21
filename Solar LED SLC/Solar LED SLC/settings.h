/*
 * settings.h
 *
 * Created: 14/11/2013 11:13:20 SA
 *  Author: SCEC
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_


#define COUNT_WR_EEMEM_ADDR 10

void writeEEPROM(char* data,unsigned int startPos,unsigned int endPos);
void readEEPROM(char* data,unsigned int startPos,unsigned int endPos);
void countWriteEEMEMcycle(void);

#endif /* SETTINGS_H_ */