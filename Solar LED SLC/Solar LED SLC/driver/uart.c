/*
uart.c
Implement uart communication
Date created: 24/07/2008
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"
//#include "sysComm.h"

#ifdef	USE_UART0
	#define BAUND_RATE0 19200     // 
	#define MAX_QUEUE0  64
#endif
#ifdef	USE_UART1
	#define BAUND_RATE1 2400     //
	#define MAX_QUEUE1  64		//	 2^N 
#endif

#define XTAL_FREQ	147456UL
#define BIT(x) (1<<x)
#ifdef	USE_UART0
	static volatile unsigned char queueUART0[MAX_QUEUE0];	//receiver queue
	static volatile unsigned int topQueue0, bottomQueue0;
#endif
#ifdef	USE_UART1
	//static volatile unsigned char * queueUART1 = (unsigned char*)0x1400;
	//static volatile unsigned char queueUART1[MAX_QUEUE1];//,queueUART1[MAX_QUEUE1];// __attribute__ ((section (".init1")));
	static volatile unsigned char queueUART1[MAX_QUEUE1];// __attribute__ ((section (".init1"))); 
	static volatile unsigned int topQueue1, bottomQueue1;
#endif
//vars

union packInt {
	int word;
	unsigned char byte[2];
} pack;

//init
void initUART(void){
	#ifdef USE_UART0
		long baudRate0 = 0;
	#endif
	#ifdef USE_UART1
		long baudRate1 = 0;
	#endif
	#ifdef	USE_UART0
	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: On
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud rate: 19200
	
	UCSR0C= _BV(UCSZ01)|_BV(UCSZ00);	//0x06; 
	baudRate0 = (F_CPU>>7)/(BAUND_RATE0>>7);
	baudRate0 = (baudRate0>>4) - 1;
	UBRR0L = (unsigned char)baudRate0;
	UBRR0H = (unsigned char)(baudRate0>>8); 
	UCSR0B = _BV(RXCIE)|_BV(TXEN)|_BV(RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	topQueue0 = 0;				 //init receiver queue
	bottomQueue0 = 0;
	#endif
	#ifdef	USE_UART1
	// USART1 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	// USART1 Baud rate: 19200
	UCSR1C= (1<<UCSZ11)|(1<<UCSZ10);	//0x06; 
	baudRate1 = F_CPU/BAUND_RATE1;
	baudRate1 = (baudRate1>>4) - 1;
	UBRR1L = (unsigned char)baudRate1;
	UBRR1H = (unsigned char)(baudRate1>>8); 
	UCSR1B = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	topQueue1 = 0;				 //init receiver queue
	bottomQueue1 = 0;
	#endif
}
#ifdef	USE_UART0
//===============================================================
//UART0 section

unsigned char getchar0(){	//wait until data available
	unsigned char temp;
	while (topQueue0 == bottomQueue0);
	temp = queueUART0[topQueue0];
	topQueue0++;
	if (topQueue0 == MAX_QUEUE0) topQueue0 = 0;
	return temp;
}
//-------------------------------------

void putchar0(unsigned char data){	//write UART data and wait until transmit complete
	while (!(UCSR0A & (1<<UDRE))){}
	UDR0 = data;
}
/*
//-------------------------------------
void writeUART0Raw(unsigned char data){	//write UART data and return immediately
	UDR0 = data;
}

//-------------------------------------
void writeWordUART0(int data){
	pack.word = data;
	writeUART0(pack.byte[1]);
	writeUART0(pack.byte[0]);
}
//-------------------------------------
void writeWordUART0Raw(int data){
	pack.word = data;
	writeUART0Raw(pack.byte[1]);
	writeUART0Raw(pack.byte[0]);
}
*/
//-------------------------------------
void putsf0(prog_char * str){			//write ROM string
	//unsigned char i = 0;
	char temp;
	temp = pgm_read_byte(str);
	while (temp != 0){
		_delay_us(50);
		putchar0(temp);
		temp = pgm_read_byte(++str);
	}
}

//-------------------------------------
void puts0(unsigned char * str){			//write RAM string
	unsigned char i = 0;
	while (str[i] != 0){
		putchar0(str[i]);
		i++;
	}
}
/*
//-------------------------------------
int readWordUART0(){
	pack.byte[1] = readUART0();
	pack.byte[0] = readUART0();
	return pack.word;
}
//-------------------------------------
unsigned int queueLengthUART0(){//check buffer size
	return (bottomQueue0>=topQueue0) ? (bottomQueue0 - topQueue0) : ((MAX_QUEUE0 - topQueue0) + bottomQueue0);
}
//-------------------------------------
void flushUART0RX(void){							//empty RX buffer
	topQueue0 = bottomQueue0;
}*/
ISR(SIG_UART0_RECV){
    queueUART0[bottomQueue0] = (unsigned char) UDR0;
	bottomQueue0++;
	if (bottomQueue0 == MAX_QUEUE0) bottomQueue0 = 0;
	//PORTA ^= BIT(6);
}
#endif

#ifdef	USE_UART1
//===============================================================
//UART1 section

char getchar1(){										//wait until data available
	char temp;
	while (topQueue1 == bottomQueue1);
	temp = queueUART1[topQueue1];
	topQueue1++;
	if (topQueue1 == MAX_QUEUE1) topQueue1 = 0;
	return temp;
}
//-------------------------------------
void putchar1(char data){	//write UART data and wait until transmit complete
	while (!(UCSR1A & (1<<UDRE))){}
	UDR1 = data;
}

//-------------------------------------
/*
void writeUART1Raw(char data){			//write UART data and return immediately
	UDR1 = data;
}

//-------------------------------------
void writeWordUART1(int data){
	pack.word = data;
	writeUART1(pack.byte[1]);
	writeUART1(pack.byte[0]);
}

//-------------------------------------
void writeWordUART1Raw(int data){
	pack.word = data;
	writeUART1Raw(pack.byte[1]);
	writeUART1Raw(pack.byte[0]);
}
*/
//-------------------------------------
void putsf1(prog_char * str){			//write ROM string
	//unsigned char i = 0;
	char temp;
	temp = pgm_read_byte(str);
	while (temp != 0){
		_delay_us(50);
		putchar1(temp);
		temp = pgm_read_byte(++str);
	}
}

//-------------------------------------
void puts1(char * str){			//write RAM string to UART2
	unsigned char i = 0;
	while (str[i] != 0){
		_delay_ms(1);
		putchar1(str[i]);
		i++;
	}
}
/*
//-------------------------------------
int readWordUART1(){
	pack.byte[1] = readUART1();
	pack.byte[0] = readUART1();
	return pack.word;
}*/

//-------------------------------------
unsigned int queueLengthUART1(){//check buffer size
	return (bottomQueue1>=topQueue1) ? (bottomQueue1 - topQueue1) : ((MAX_QUEUE1 - topQueue1) + bottomQueue1);
}

//-------------------------------------
void flushUART1RX(void){	//empty RX buffer
	topQueue1 = bottomQueue1 = 0;
}
unsigned char * getAddrUart1Buff(void){
	return 	queueUART1;
}

ISR(SIG_UART1_RECV){
    queueUART1[bottomQueue1] = UDR1;
	bottomQueue1++;
	if (bottomQueue1 == MAX_QUEUE1) bottomQueue1 = 0;
}
#endif


    