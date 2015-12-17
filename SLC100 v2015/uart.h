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

void initUART(void);

//UART1
/*
unsigned char readUART1();				//wait until data available
int readWordUART1();
void writeUART1(unsigned char data);			//write UART data and wait until transmit complete
void writeUART1Raw(unsigned char data);			//write UART data and return immediately
void writeWordUART1(int data);
void writeWordUART1Raw(int data);
void writeStringUART1(prog_char * str);			//write ROM string
void writeMStringUART1(unsigned char * str);			//write RAM string
unsigned int queueLengthUART1();			//check buffer size
void flushUART1RX(void);				//empty RX buffer
*/
//UART2
char readUART2();				//wait until data available
void writeUART2Header(void);
void writeUART2Footer(void);
int readWordUART2();
void writeUART2(char data);			//write UART data and wait until transmit complete
void writeUART2Raw(char data);			//write UART data and return immediately
void writeWordUART2(int data);
void writeWordUART2Raw(int data);
void writeStringUART2(prog_char * str);			//write ROM string
void writeMStringUART2(char * str);			//write RAM string
unsigned int queueLengthUART2();			//check buffer size
void flushUART2RX(void);				//empty RX buffer 

#endif 
