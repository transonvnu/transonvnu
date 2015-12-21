/*
uart.c
Implement uart communication
Date created: 24/07/2008
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "sysComm.h"
    
#define BAUND_RATE1 192     //*100 
#define BAUND_RATE2 192     //*100 
#define XTAL_FREQ  147456   //*100 Hz
//#define MAX_QUEUE1  1
#define MAX_QUEUE2  500

#define BIT(x) (1<<x)

//static volatile unsigned char queueUART1[MAX_QUEUE1];	//receiver queue
//static volatile unsigned char * queueUART2 = (unsigned char*)0x1400;
//static volatile unsigned char queueUART2[MAX_QUEUE2];//,queueUART1[MAX_QUEUE1];// __attribute__ ((section (".noinit")));
static volatile unsigned char queueUART2[MAX_QUEUE2];// __attribute__ ((section (".noinit")));
//static volatile unsigned int topQueue1, bottomQueue1, topQueue2, bottomQueue2;
static volatile unsigned int topQueue2, bottomQueue2;

//vars
union packInt {
	int word;
	unsigned char byte[2];
} pack;

//init
void initUART(void){
	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: On
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud rate: 19200
	/*
	UCSR0C=0x06;//8 Bit data 
	UBRR0L = (unsigned char)(((long)XTAL_FREQ/((long)16*BAUND_RATE1))-1);
	UBRR0H = (unsigned char)((((long)XTAL_FREQ/((long)16*BAUND_RATE1))-1)>>8)&0xFF;
	UCSR0B = UCSR0B |(1<<RXCIE)|(1<<TXEN)|(1<<RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	topQueue1 = 0;				 //init receiver queue
	bottomQueue1 = 0;
	*/
	// USART1 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	// USART1 Baud rate: 19200
	UCSR1C=0x06; 
	UBRR1L = (unsigned char)(((long)XTAL_FREQ/((long)16*BAUND_RATE2))-1);
	UBRR1H = (unsigned char)((((long)XTAL_FREQ/((long)16*BAUND_RATE2))-1)>>8)&0xFF; 
	UCSR1B = UCSR1B |(1<<RXCIE)|(1<<TXEN)|(1<<RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	topQueue2 = 0;				 //init receiver queue
	bottomQueue2 = 0;

}

//===============================================================
//UART1 section
/*
unsigned char readUART1(){	//wait until data available
	unsigned char temp;
	while (topQueue1 == bottomQueue1);
	temp = queueUART1[topQueue1];
	topQueue1++;
	if (topQueue1 == MAX_QUEUE1) topQueue1 = 0;
	return temp;
}
//-------------------------------------
void writeUART1(unsigned char data){	//write UART data and wait until transmit complete
	while (!(UCSR0A & (1<<UDRE))){}
	UDR0 = data;
}

//-------------------------------------
void writeUART1Raw(unsigned char data){	//write UART data and return immediately
	UDR0 = data;
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

//-------------------------------------
void writeStringUART1(prog_char * str){			//write ROM string
	unsigned char i = 0;
	while (str[i] != 0){
		writeUART1(str[i]);
		i++;
	}
}

//-------------------------------------
void writeMStringUART1(unsigned char * str){			//write RAM string
	unsigned char i = 0;
	while (str[i] != 0){
		writeUART1(str[i]);
		i++;
	}
}

//-------------------------------------
int readWordUART1(){
	pack.byte[1] = readUART1();
	pack.byte[0] = readUART1();
	return pack.word;
}
//-------------------------------------
unsigned int queueLengthUART1(){//check buffer size
	return (bottomQueue1>=topQueue1) ? (bottomQueue1 - topQueue1) : ((MAX_QUEUE1 - topQueue1) + bottomQueue1);
}
//-------------------------------------
void flushUART1RX(void){							//empty RX buffer
	topQueue1 = bottomQueue1;
}*/
//===============================================================
//UART2 section
char readUART2(){										//wait until data available
	char temp;
	while (topQueue2 == bottomQueue2);
	temp = queueUART2[topQueue2];
	topQueue2++;
	if (topQueue2 == MAX_QUEUE2) topQueue2 = 0;
	return temp;
}
void writeUART2Header(void){
	writeStringUART2("AT+MYCMD=\"");
}
void writeUART2Footer(void){
	writeStringUART2("\"\r\n");	
}
//-------------------------------------
void writeUART2(char data){	//write UART data and wait until transmit complete
	while (!(UCSR1A & (1<<UDRE))){}
	UDR1 = data;
}

//-------------------------------------
void writeUART2Raw(char data){			//write UART data and return immediately
	UDR1 = data;
}

//-------------------------------------
void writeWordUART2(int data){
	pack.word = data;
	writeUART2(pack.byte[1]);
	writeUART2(pack.byte[0]);
}

//-------------------------------------
void writeWordUART2Raw(int data){
	pack.word = data;
	writeUART2Raw(pack.byte[1]);
	writeUART2Raw(pack.byte[0]);
}

//-------------------------------------
void writeStringUART2(prog_char * str){			//write ROM string
	unsigned char i = 0;
	while (str[i] != 0){
		_delay_ms(1);
		writeUART2(str[i]);
		i++;
	}
}

//-------------------------------------
void writeMStringUART2(char * str){			//write RAM string to UART2
	unsigned char i = 0;
	while (str[i] != 0){
		_delay_ms(1);
		writeUART2(str[i]);
		i++;
	}
}

//-------------------------------------
int readWordUART2(){
	pack.byte[1] = readUART2();
	pack.byte[0] = readUART2();
	return pack.word;
}

//-------------------------------------
unsigned int queueLengthUART2(){//check buffer size
	return (bottomQueue2>=topQueue2) ? (bottomQueue2 - topQueue2) : ((MAX_QUEUE2 - topQueue2) + bottomQueue2);
}

//-------------------------------------
void flushUART2RX(void){		//empty RX buffer
	topQueue2 = bottomQueue2;
}

//interrupt section 
/*
ISR(SIG_UART0_RECV){
    queueUART1[bottomQueue1] = (unsigned char) UDR0;
	bottomQueue1++;
	if (bottomQueue1 == MAX_QUEUE1) bottomQueue1 = 0;
	//PORTA ^= BIT(6);
}*/

ISR(SIG_UART1_RECV){ 
    queueUART2[bottomQueue2] = (char) UDR1;
	//if(queueUART2[bottomQueue2] > 127) resetModem();
	bottomQueue2++;
	if (bottomQueue2 == MAX_QUEUE2) bottomQueue2 = 0;
}
     
