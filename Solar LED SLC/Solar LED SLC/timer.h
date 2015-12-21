/*
 * timer.h
 *
 * Created: 06/09/2013 4:36:04 CH
 *  Author: SCEC
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define BATCHAGER	4
#define BATCHG_DDR	DDRB
#define BATCHG_PORT	PORTB

void Timer0Init(void);
void Timer1Init(void);

void setTimerT0( unsigned int Ptime);
unsigned char checkTimerT0 (void);

unsigned char checkTick1s(void);
void delayScanKey(unsigned char	_tick);
void changePWMT0(unsigned char PWMvl);

#endif /* TIMER_H_ */