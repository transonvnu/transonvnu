/*
 * ks0108.h
 *
 * Created: 31/08/2013 11:13:11 SA
 *  Author: SCEC
 */ 


#ifndef KS0108_H_
#define KS0108_H_


#include <avr/io.h>
#include <avr/pgmspace.h>

#define uchar unsigned char
#define uint  unsigned int

#define MAXCOLUMN	128
//Address of the LCD
#define CMD_READ  (*(volatile unsigned char *)0x2008)		// 0b0000 1000
#define CMD_WRITE (*(volatile unsigned char *)0x2000)		// 0b0000 0000
#define DAT_READ  (*(volatile unsigned char *)0x2018)		// 0b0001 1000
#define DAT_WRITE (*(volatile unsigned char *)0x2010)		// 0b0001 0000

#define SET_RST_OUTPUT() DDRD |= _BV(PD4)
#define SET_RST()		PORTD |= _BV(PD4)
#define CLR_RST()		PORTD &=~_BV(PD4)

#define SET_CSA_OUTPUT() DDRD |= _BV(PD6)
#define SET_CSA()		PORTD |= _BV(PD6)
#define CLR_CSA()		PORTD &=~_BV(PD6)

#define SET_CSB_OUTPUT() DDRD |= _BV(PD5)
#define SET_CSB()		PORTD |= _BV(PD5)
#define CLR_CSB()		PORTD &=~_BV(PD5)


/**************** Underlying operating proceduredeclaration **************************************/
void BusyOrNot(void);                          		//Sub-busy             /
void WriteCommandToRightArea(uchar Command);		//Write command to the right area     /
void WriteDataToRightArea(uchar _Data);	//Write data to the right area    /
void WriteCommandToLeftArea(uchar Command);			//Left area write command     /
void WriteDataToLeftArea(uchar _Data);  //Write data to the left area     /
/**********************************************************************/

unsigned char checkBufferChanged(void);
																										 
/**************************************************************/
void ClearDisBuf(uchar val);              //Clear memory
void ShowLcd(uchar on);               //On Display
void DisStartLine(unsigned char LineN);
void InitialLcd(void);			      //Clear LCD RAM, locate the starting line 0 0, open display
void LocateLcdColumn(uchar Column);   //Column positioning
void LocateLcdPage(uchar Page);       //Page Positioning
void LocateLcdPageColumn(uchar Page,uchar Column);//Positioning page and columns
void UpdateDis(void);

void OnePixel(uchar x,uchar y,uchar val);
uchar ReadPixel(uchar x,uchar y);
void clearROW(unsigned char _idxrow);
void invertROW(unsigned char _idxrow);

void f5x7_putsf(unsigned char x, unsigned char y, prog_char * str);
void f5x7_puts(unsigned char x, unsigned char y, char *str);
void f5x7_putchar(unsigned char x,unsigned char y,unsigned char chr_dat);

void LCDDelay(uint n);
//Delay

#endif /* KS0108_H_ */