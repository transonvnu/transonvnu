/*
 * sysComm.h
 *
 * Created: 22/09/2013 3:05:24 CH
 *  Author: SCEC
 */ 


#ifndef SYSCOMM_H_
#define SYSCOMM_H_

#define COMM_TIMEOUT		8		//~ 800 milisec, total time on once read command
#define CMD_STRING_LENGTH	4

unsigned char readCommand(char* args);


#endif /* SYSCOMM_H_ */