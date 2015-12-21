/*
 * ctrkey.h
 *
 * Created: 06/09/2013 11:21:30 SA
 *  Author: SCEC
 */ 


#ifndef CTRKEY_H_
#define CTRKEY_H_

#define KEY_DDR		DDRB
#define KEY_PORT	PORTB
#define KEY_PIN		PINB

#define _KEY_CLK	6
#define _KEY_RST	5
#define _KEY_INP	7

#define LEFT_PRESSED	1
#define RIGHT_PRESSED	2
#define UP_PRESSED		3
#define DOWN_PRESSED	4
#define OK_PRESSED		5
#define ESC_PRESSED		6

struct _BUTTON_MD {
	unsigned char Pressed;
	unsigned char ButtonNum;
};

void initKeyboard(void);
void scanKey(void);
unsigned char checkBtPressed(void);
unsigned char getButton(void);

#endif /* CTRKEY_H_ */