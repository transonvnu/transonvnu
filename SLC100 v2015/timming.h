/*
timming.h
Library for timming and delay
This library use Timer4 and its interrupt
Date created: 04/19/2007
*/

#ifndef _TIMMING_INCLUDED_
#define _TIMMING_INCLUDED_
void initTimer0(void);
void initTimer2(void);
void timer2_init(void);
void delayMs(unsigned int ms);
unsigned int getTime();
void resetTime(void);
unsigned int getRealtimeFailure(void);
void resetRealtimeFailure(void);
void delay(unsigned long time);
void Delay(unsigned int time);

void CLEAR_MB_TIME(void);
void START_MB_TIME15(void);
void START_MB_TIME2(void);
void START_MB_TIME35(void);

#endif 
