/*
uart.c
Implement uart communication
Date created: 24/07/2008
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "sysComm.h"
    
#define BAUND_RATE 96      //*100 
#define XTAL_FREQ  80000   //*100 Hz

#define MAX_QUEUE  300

#define BIT(x) (1<<x)

static volatile unsigned char queueUART[MAX_QUEUE];
static volatile unsigned int topQueue,bottomQueue;

//vars
/*
union packInt {
	int word;
	unsigned char byte[2];
} pack;
*/
//init
void initUART(void){

	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud rate: 19200
	// UCSRC = 0x86; 
	DDRD |= 0x02; //UART TxD
	/*UBRRL = (unsigned char)(((long)XTAL_FREQ/((long)16*BAUND_RATE)));
	UBRRH = (unsigned char)((((long)XTAL_FREQ/((long)16*BAUND_RATE))-1)>>8); 
	UCSRB = UCSRB |(1<<RXCIE)|(1<<TXEN)|(1<<RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	UCSRA = UCSRA |(1<<U2X);*/
	UCSRA=0x02;
	UCSRB=UCSRB |(1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
	UCSRC=0x86;
	UBRRH=0x01;
	UBRRL=0xA0;
	topQueue = 0;				 //init receiver queue
	bottomQueue = 0;

}

//UART section
char readUART(){										//wait until data available
	char temp;
	while (topQueue == bottomQueue);
	temp = queueUART[topQueue];
	topQueue++;
	if (topQueue == MAX_QUEUE) topQueue = 0;
	return temp;
}
/*
void writeUARTHeader(void){
	writeStringUART("AT+MYCMD=\"");
}
void writeUARTFooter(void){
	writeStringUART("\"\r\n");	
}*/
//-------------------------------------
void writeUART(char data){	//write UART data and wait until transmit complete
	while (!(UCSRA & (1<<UDRE))){}
	UDR = data;
}
/*
//-------------------------------------
void writeUARTRaw(char data){			//write UART data and return immediately
	UDR = data;
}

//-------------------------------------
void writeWordUART(int data){
	pack.word = data;
	writeUART(pack.byte[1]);
	writeUART(pack.byte[0]);
}

//-------------------------------------
void writeWordUARTRaw(int data){
	pack.word = data;
	writeUARTRaw(pack.byte[1]);
	writeUARTRaw(pack.byte[0]);
}*/

//-------------------------------------
void writeStringUART(prog_char * str){			//write ROM string
	unsigned char i = 0;
	while (str[i] != 0){
		//_delay_ms(1);
		writeUART(str[i]);
		i++;
	}
}

//-------------------------------------
void writeMStringUART(char * str){			//write RAM string
	unsigned char i = 0;
	while (str[i] != 0){
		//_delay_ms(1);
		writeUART(str[i]);
		i++;
	}
}
/*
//-------------------------------------
int readWordUART(){
	pack.byte[1] = readUART();
	pack.byte[0] = readUART();
	return pack.word;
}*/

//-------------------------------------
unsigned int queueLengthUART(){//check buffer size
	return (bottomQueue>=topQueue) ? (bottomQueue - topQueue) : ((MAX_QUEUE - topQueue) + bottomQueue);
}

//-------------------------------------
/*
void flushUARTRX(void){		//empty RX buffer
	topQueue = bottomQueue;
}*/

ISR(USART_RXC_vect){ 
    queueUART[bottomQueue] = (char) UDR;
	bottomQueue++;
	if (bottomQueue == MAX_QUEUE) bottomQueue = 0;
}
     
