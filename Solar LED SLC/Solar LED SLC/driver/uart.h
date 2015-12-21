/*
uart.h
Implement uart communication
Date created: 24/07/2008
*/

#ifndef _UART_INCLUDED_
#define _UART_INCLUDED_     
#include <avr/pgmspace.h>
/*
//USART control and status register A 
#define RXC   0x80
#define TXC   0x40
#define UDRE  0x20
#define FE    0x10
#define DOR   0x08
#define UPE   0x04
#define U2X   0x02
#define MPCM  0x01 
//USART control and status register B
#define RXCIE 0x80
#define TXCIE 0x40
#define UDRIE 0x20
#define RXEN  0x10
#define TXEN  0x08
#define UCSZ  0x04
#define RXB8  0x02
#define TXB8  0x01 
*/

#define USE_UART0
#define USE_UART1

void initUART(void);

#ifdef USE_UART0
//UART0
unsigned char getchar0();				//wait until data available
int readWordUART0();
void putchar0(unsigned char data);			//write UART data and wait until transmit complete
void writeUART0Raw(unsigned char data);			//write UART data and return immediately
void writeWordUART0(int data);
void writeWordUART0Raw(int data);
void putsf0(prog_char * str);			//write ROM string
void puts0(unsigned char * str);			//write RAM string
unsigned int queueLengthUART0();			//check buffer size
void flushUART0RX(void);				//empty RX buffer
#endif

#ifdef	USE_UART1
//UART1
char getchar1();				//wait until data available
void writeUART1Header(void);
void writeUART1Footer(void);
int readWordUART1();
void putchar1(char data);			//write UART data and wait until transmit complete
void writeUART1Raw(char data);			//write UART data and return immediately
void writeWordUART1(int data);
void writeWordUART1Raw(int data);
void putsf1(prog_char * str);			//write ROM string
void puts1(char * str);			//write RAM string
unsigned int queueLengthUART1();			//check buffer size
void flushUART1RX(void);				//empty RX buffer 
unsigned char * getAddrUart1Buff(void);
#endif

#endif 
