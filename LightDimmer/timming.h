/*
timming.h
Library for timming and delay
This library use Timer4 and its interrupt
Date created: 04/19/2007
*/

#ifndef _TIMMING_INCLUDED_
#define _TIMMING_INCLUDED_
//void delay(long time);
void initTimer0(void);
//void initTimer2(void);
//void delayMs(unsigned int ms);
//void delayUs(unsigned int us);
unsigned int getTime();
void resetTime();
void forceBTAoff(void);

#endif 
