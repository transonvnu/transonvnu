
#include <avr/io.h>   // Header file of device
#include <avr/pgmspace.h>
#include "ds1307.h"

#ifndef	_MBAT128_H_
#define _MBAT128_H_

#define	STATE_MB_IDLE				0x00//trang thai khoi tao
#define	STATE_MB_INIT				0x0F//trang thai khoi tao

#define	STATE_MB_RECEPTION			0x08//0x01
#define	STATE_MB_ISERR				0x09//0x02
#define	STATE_MB_FRM_CHECK			0x05

#define	STATE_MB_FRM_ERR			0x10
//#define	STATE_MB_FRM_OK				0x11
#define	STATE_MB_FRM_NOK			0x12

#define	STATE_MB_EMISSION			0x21
#define	STATE_MB_INIT_EMISSION		0x22

#define	MB_FRAME_OK					0x88//0x01
#define	MB_FRAME_NOK				0xFF


//#define	INIT_RXLED				DDRC |= 0x01
//#define	INIT_TXLED				DDRC |= 0x04
#define	INIT_TXEN				UCSR0B |= (1<<TXEN)
#define	ENABLE_TX				UCSR0B |= (1<<TXEN);PORTE |= 0x04
#define	DISABLE_TX				UCSR0B &= (~(1<<TXEN));PORTE &= 0xFB
//#define	ENABLE_RX				UCSR0B |= (1<<RXEN);DDRE &= 0xFE
//#define	DISABLE_RX				UCSR0B &= (~(1<<RXEN));DDRE |= 0x01
//#define	RXLED_ON				PORTA |= 0x40
//#define	RXLED_OFF				PORTA &= (~0x40)

//#define	TXLED_ON				PORTC |= 0x04
//#define	TXLED_OFF				PORTC &= 0x0B

#define	MAX_IN_BUF					800
#define	MAX_OUT_BUF					400
//#define	TIME1CHAR15					190
//#define	TIME1CHAR2					250
//#define	TIME1CHAR35					440
//#define	TIME19200PR					3694

//#define	CLEAR_MB_TIME			TCCR1B = 0x00;TCNT1H = 0;TCNT1L = 0;		//stop counter 8 nay, can dat lai counter reg 0?
//#define	START_MB_TIME15			ICR1L  = TIME1CHAR15;	TCCR1B = 0x13;		//start t15 ,Fclk = 125 kHz
//#define	START_MB_TIME2			ICR1L  = TIME1CHAR2 ;	TCCR1B = 0x13;		//start t2	,Fclk = 125 kHz
//#define	START_MB_TIME35			ICR1  = TIME1CHAR35;	TCCR1B = 0x13;		//start t35 ,Fclk = 125 kHz


extern unsigned char pInBuff[MAX_IN_BUF] __attribute__ ((section (".init1")));
extern unsigned char pOutBuff[MAX_OUT_BUF] __attribute__ ((section (".init1")));
extern unsigned int nInBuffCount ;
extern unsigned int nOutBuffCount;
extern unsigned char nMBState ;
extern unsigned int crc ;
//extern unsigned char* pTempChar ;
extern unsigned int nByteSent;

//extern unsigned int outRegs[400] __attribute__ ((section (".init1")));
extern unsigned char outBytes[100] __attribute__ ((section (".init1")));
extern unsigned char RegBytes[10000] __attribute__ ((section (".init1")));


void mb_crc (unsigned char* buf,unsigned  char cnt);
void SendBuffWithCRC();
void InitMBSer();

char parseMBFrm();
char ReadHoldingRegister(unsigned char nSlaveAddr,unsigned int nRegAddr,char nToRead);
//char ReplyReadHoldingRegister(unsigned char nSlaveAddr,unsigned char nReg,unsigned int startAddr,unsigned int* outReg);
char ReplyReadHoldingRegister(unsigned char nSlaveAddr,unsigned char nReg,unsigned int startAddr,unsigned char* outBytes);
char WriteSingleRegister(unsigned char nSlaveAddr,unsigned int nRegAddr,int valToWrite);
char WriteMultipleRegister(unsigned char nSlaveAddr,unsigned int nRegAddr,unsigned int nReg,unsigned int * arrToWrite);
char ReplyWriteMultipleRegister(unsigned char nSlaveAddr,unsigned int nReg,unsigned int startAddr);
char ResponseHoldingReg(unsigned char data);

char readUART1();				//wait until data available
int readWordUART1();
void writeUART1(char data);			//write UART data and wait until transmit complete
void writeUART1Raw(char data);			//write UART data and return immediately
//void writeWordUART1(int data);
//void writeWordUART1Raw(int data);
void writeStringUART1(prog_char * str);			//write ROM string
void writeMStringUART1(char * str);			//write RAM string
unsigned int queueLengthUART1();			//check buffer size
void flushUART1RX(void);				//empty RX buffer

void modBusSelect(void);
void modBusDeselect(void);

void setIDnode(unsigned char nrID1,unsigned char nrID2,unsigned char nrID3,unsigned char nrID4,unsigned char nrID5,unsigned char nrID6,char newID);
void dimAllNode(unsigned char dimValue);
void dimOneNode(unsigned char NodeID,unsigned char dimValue);
void SetTimeNode(unsigned char NodeID,TimeStruct* time);
unsigned char sendLedCmd(TimeStruct* time,char * cmd,char * receiData);
unsigned char sendLedCmd2(TimeStruct* time,char * cmd,char * receiData);
unsigned char sendLedCmd3(TimeStruct* time,char * cmd,char * receiData);
unsigned char waitStr(prog_char * str);
unsigned char waitMStr(char * str);
void sendEchelonCmd(char* Args);
unsigned char compStr(char * comp_str, prog_char * str);

#endif	//_MBAT128_H_
