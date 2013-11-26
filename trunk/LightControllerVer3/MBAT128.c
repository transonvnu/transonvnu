 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "MBAT128.h"
#include "timming.h"
#include "t6963c.h"
#include "string1.h"

#define BAUND_RATE 96       //*100 
#define XTAL_FREQ  147456   //*100 Hz
//#define MAX_QUEUE  200
#define MAX_QUEUE1  400
#define BIT(x) (1<<x)

#define UARTSELECT 4        //1 console; 0 modbus

volatile char Uart_Select = 0;
//static volatile unsigned char queueUART1[MAX_QUEUE1];
static volatile char queueUART1[MAX_QUEUE1] __attribute__ ((section (".init1")));
static volatile unsigned int topQueue1, bottomQueue1;
//extern unsigned char currentUart0;


unsigned char pInBuff[MAX_IN_BUF] __attribute__ ((section (".init1")));
unsigned char pOutBuff[MAX_OUT_BUF] __attribute__ ((section (".init1")));
unsigned int nInBuffCount = 0;
unsigned int nOutBuffCount = 0;
unsigned int nByteSent = 0;

//unsigned int outRegs[400] __attribute__ ((section (".init1")));
unsigned char outBytes[100] __attribute__ ((section (".init1")));
unsigned char RegBytes[10000] __attribute__ ((section (".init1")));

unsigned char nMBState = STATE_MB_INIT;
unsigned int crc = 0;

//unsigned char* pTempChar = 0;
unsigned char temp;
unsigned int uIndex ; // will index into CRC lookup table 

//int HoldingReg[125] = {0};

void InitMBSer()
{
	unsigned int i;
	for(i=0;i<10000;i++){
		//outRegs[i] = 24930;
		//outBytes[i] = 0xFF;
		RegBytes[i] = 0x00;
	}
	RegBytes[1] = 69;
	//DDRD |= 0x10;//PD4
	//INIT_RXLED;
	//INIT_TXLED;
	//DISABLE_TX;
	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	
	DDRE |= BIT(2); //PE2 -> RE,DE RS485
	PORTB &= (~BIT(4));
	DDRB |= BIT(4);
	UCSR0C=0x06;//8 Bit data 
	//UBRR0L = (unsigned char)(((long)XTAL_FREQ/((long)16*BAUND_RATE))-1);
	//UBRR0H = (unsigned char)((((long)XTAL_FREQ/((long)16*BAUND_RATE))-1)>>8)&0x0F;
	UBRR0L = 0x5F;
	UBRR0H = 0x00;
	UCSR0B = UCSR0B |(1<<TXCIE)|(1<<RXCIE)|(1<<TXEN)|(1<<RXEN); //TxCIE_UDRIE_RxEN_TxEN_CH9_RXB8_TxB8
	DISABLE_TX;
	nMBState = STATE_MB_INIT;
	START_MB_TIME35();
}
void modBusSelect(void){
	PORTB &= (~BIT(4));//Modbus
	DISABLE_TX;
	nMBState = STATE_MB_INIT;
	START_MB_TIME35();
	Uart_Select = 0;
	_delay_ms(20);
}
void modBusDeselect(void){
	PORTB |= BIT(4);//Console selected
	ENABLE_TX;
	Uart_Select = 1;
}
char parseMBFrm()
{
	if((nMBState == STATE_MB_FRM_NOK)||(nInBuffCount <= 2))
	{
		nMBState = STATE_MB_IDLE;
		return MB_FRAME_NOK;
	}	
	mb_crc(pInBuff, nInBuffCount-2);
	if(crc == ((pInBuff[nInBuffCount-2] <<8)| (pInBuff[nInBuffCount-1])) )
	{
		nMBState = STATE_MB_IDLE;
		return MB_FRAME_OK;
	}
	nMBState = STATE_MB_IDLE;
	return MB_FRAME_NOK;
}

char ReadHoldingRegister(unsigned char nSlaveAddr,unsigned int nRegAddr,char nToRead)//Function 0x03
{	
	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x03;			//ham 0x03
	pOutBuff[2] = (unsigned char)(nRegAddr>>8);
	pOutBuff[3] = (unsigned char)nRegAddr;
	pOutBuff[4] = (unsigned char)(nToRead>>8);
	pOutBuff[5] = (unsigned char)nToRead;
	mb_crc (pOutBuff,6 );
	pOutBuff[6] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[7] = (unsigned char)crc;
	nOutBuffCount = 8;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}
/*
char ReplyReadHoldingRegister(unsigned char nSlaveAddr,unsigned char nReg,unsigned int startAddr,unsigned int* outReg){
	
	unsigned char i;

	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x03;			//Ham 0x03: read Holding Register
	pOutBuff[2] = (unsigned char)(2*nReg);//Byte count
	for(i=0;i<nReg;i++){
		pOutBuff[3+i*2] = (unsigned char)(outReg[startAddr+i]>>8);
		pOutBuff[4+i*2] = (unsigned char)(outReg[startAddr+i]);
	}
	//pOutBuff[5+2*nReg] = (unsigned char)valToWrite;
	mb_crc (pOutBuff,3+2*nReg);
	pOutBuff[3+2*nReg] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[4+2*nReg] = (unsigned char)crc;
	nOutBuffCount = 5+2*nReg;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}
*/
char ReplyReadHoldingRegister(unsigned char nSlaveAddr,unsigned char nReg,unsigned int startAddr,unsigned char* outBytes){
	
	unsigned char i;

	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x03;			//Ham 0x03: read Holding Register
	pOutBuff[2] = (unsigned char)(2*nReg);//Byte count
	for(i=0;i<2*nReg;i++){
		pOutBuff[3+i] = (unsigned char)(outBytes[startAddr+i]);
	}
	mb_crc (pOutBuff,3+2*nReg);
	pOutBuff[3+2*nReg] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[4+2*nReg] = (unsigned char)crc;
	nOutBuffCount = 5+2*nReg;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}
char WriteSingleRegister(unsigned char nSlaveAddr,unsigned int nRegAddr, int valToWrite)//Function 0x06
{
	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x06;			//Ham 0x06: Write Single Register
	pOutBuff[2] = (unsigned char)(nRegAddr>>8);
	pOutBuff[3] = (unsigned char)nRegAddr;
	pOutBuff[4] = (unsigned char)(valToWrite>>8);
	pOutBuff[5] = (unsigned char)valToWrite;
	mb_crc (pOutBuff,6 );
	pOutBuff[6] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[7] = (unsigned char)crc;
	nOutBuffCount = 8;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}
char WriteMultipleRegister(unsigned char nSlaveAddr,unsigned int nRegAddr,unsigned int nReg,unsigned int * arrToWrite){
	unsigned char i;
	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x10;			//Ham 0x10: Write Multiple Register
	pOutBuff[2] = (unsigned char)(nRegAddr>>8);
	pOutBuff[3] = (unsigned char)nRegAddr;
	pOutBuff[4] = (unsigned char)(nReg>>8);
	pOutBuff[5] = (unsigned char)nReg;
	pOutBuff[6] = (unsigned char)(2*nReg);
	for(i=0;i<nReg;i++){
		pOutBuff[7+i*2] = (unsigned char)(arrToWrite[7+i]>>8);
		pOutBuff[8+i*2] = (unsigned char)(arrToWrite[7+i]);
	}
	mb_crc (pOutBuff,(7+2*nReg));
	pOutBuff[7+2*nReg] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[8+2*nReg] = (unsigned char)crc;
	nOutBuffCount = 9+2*nReg;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;

}
char ReplyWriteMultipleRegister(unsigned char nSlaveAddr,unsigned int nReg,unsigned int startAddr){
	
//	unsigned char i;

	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = nSlaveAddr;
	pOutBuff[1] = 0x10;			//Ham 0x10: Write Multiple Register
	pOutBuff[2] = (unsigned char)(startAddr>>8);
	pOutBuff[3] = (unsigned char)startAddr;
	pOutBuff[4] = (unsigned char)(nReg>>8);
	pOutBuff[5] = (unsigned char)nReg;
	mb_crc (pOutBuff,6);
	pOutBuff[6] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[7] = (unsigned char)crc;
	nOutBuffCount = 8;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}
char ResponseHoldingReg(unsigned char data)
{	
	if(nMBState != STATE_MB_IDLE)
		return 	nMBState;
	pOutBuff[0] = 88;
	pOutBuff[1] = 0x03;			//ham 0x03
	pOutBuff[2] = 0x02;
	pOutBuff[3] = 0x00;
	pOutBuff[4] = data;
	mb_crc (pOutBuff,5 );
	pOutBuff[5] = (unsigned char)(crc>>8);		//truyen byte high cuoi cung
	pOutBuff[6] = (unsigned char)crc;
	nOutBuffCount = 7;
	//TXLED_ON;	
	ENABLE_TX;
	nByteSent = 0;
	writeUART1(pOutBuff[0]);
	nMBState = STATE_MB_EMISSION;
	return 	nMBState;
}

void mb_crc (unsigned char* buf,unsigned  char cnt)
{
	unsigned char i,j;
	crc=0xFFFF;
	for (i=0; i<cnt; i++) 
	{
		crc=crc ^ buf[i];
		for (j=1; j<=8; j++) 
		{
			uIndex = crc & 0x0001;
			crc=crc >> 1;
			if (uIndex)
				crc = crc ^ 0xA001;
		}
	}
	crc = (unsigned int )((crc >> 8) & 0x00ff)| (unsigned int )((crc << 8) & 0xff00);
}

//console communication

char readUART1(){	//wait until data available
	char temp;
	while (topQueue1 == bottomQueue1);
	temp = queueUART1[topQueue1];
	topQueue1++;
	if (topQueue1 == MAX_QUEUE1) topQueue1 = 0;
	return temp;
}
void writeUART1(char data){	//write UART data and wait until transmit complete
	while (!(UCSR0A & (1<<UDRE))){}
	UDR0 = data;
	
}
//-------------------------------------
void writeUART1Raw(char data){	//write UART data and return immediately
	UDR0 = data;
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
void writeMStringUART1(char * str){			//write RAM string
	unsigned char i = 0;
	while (str[i] != 0){
		writeUART1(str[i]);
		i++;
	}
}
//-------------------------------------
unsigned int queueLengthUART1(){//check buffer size
	return (bottomQueue1>=topQueue1) ? (bottomQueue1 - topQueue1) : ((MAX_QUEUE1 - topQueue1) + bottomQueue1);
}
//-------------------------------------
void flushUART1RX(void){							//empty RX buffer
	topQueue1 = bottomQueue1;
}
//-------------------------------------
void setIDnode(unsigned char nrID1,unsigned char nrID2,unsigned char nrID3,unsigned char nrID4,unsigned char nrID5,unsigned char nrID6,char newID){
	//char str[3];
	/*
	outRegs[0] = 0;
	outRegs[0] <<= 8;
	outRegs[0] |= 0;
	outRegs[1] = newID;
	outRegs[1] <<= 8;
	outRegs[1] |= nrID[0];
	outRegs[2] = nrID[1];
	outRegs[2] <<= 8;
	outRegs[2] |= nrID[2];
	outRegs[3] = nrID[3];
	outRegs[3] <<= 8;
	outRegs[3] |= nrID[4];
	outRegs[4] = nrID[5];
	outRegs[4] <<= 8;
	outRegs[4] |= 0;
	*/
	outBytes[0] = 0;
	outBytes[1] = 0;
	outBytes[2] = newID;
	outBytes[3] = nrID1;
	outBytes[4] = nrID2;
	outBytes[5] = nrID3;
	outBytes[6] = nrID4;
	outBytes[7] = nrID5;
	outBytes[8] = nrID6;

}
void dimAllNode(unsigned char dimValue){
	//outRegs[200] = 2;
	//outRegs[201] = 0xFF00 + dimValue;
}
void dimOneNode(unsigned char NodeID,unsigned char dimValue){
	/*
	outRegs[0] = 1;
	outRegs[1] = NodeID;
	outRegs[1] <<= 8;
	outRegs[1] |= dimValue;
	*/
	outBytes[0] = 0;
	outBytes[1] = 1;
	outBytes[2] = NodeID;
	outBytes[3] = dimValue;

}
 void SetTimeNode(unsigned char NodeID,TimeStruct* time){
 	/*
	outRegs[0] = 2;
	outRegs[1] = NodeID;
	outRegs[1] <<= 8;
	outRegs[1] |= 0;//giay
	outRegs[2] = (*time).minute;
	outRegs[2] <<= 8;
	outRegs[2] |= (*time).hour;
	outRegs[3] = (*time).day;
	outRegs[3] <<= 8;
	outRegs[3] |= (*time).date;
	outRegs[4] = (*time).month;
	outRegs[4] <<= 8;
	outRegs[4] |= (*time).year;
	*/
	outBytes[0] = 0;
	outBytes[1] = 2;
	outBytes[2] = NodeID;
	outBytes[3] = 0;//giay
	outBytes[4] = (*time).minute;
	outBytes[5] = (*time).hour;
	outBytes[6] = (*time).day;
	outBytes[7] = (*time).date;
	outBytes[8] = (*time).month;
	outBytes[9] = (*time).year;

}
//End console communication

void sendEchelonCmd(char* Args){
	unsigned char i;
	for(i = 0;i<Args[32]/2;i++){
		outBytes[i] = Args[i];
	}
}

unsigned char sendLedCmd(TimeStruct* time,char * cmd,char * receiData){
	char str[3],ttemp = 0;
	//unsigned char i = 0;

	flushUART1RX();
	
	if (compStr(cmd, "PLAY")){	// Lenh so 5
		cmd[0]='P'; cmd[1]='L'; cmd[2]='A'; cmd[3]='Y';
		cmd[4]='A'; cmd[5]='L'; cmd[6]='L'; cmd[7]= 0;
	}else
	if (((cmd[0]=='T')&&(cmd[1]=='I')&&(cmd[2]=='M')&&(cmd[3]=='E'))){	// lenh so 14:	TIME_ON.hh:mm		-> TIMEON_hh_mm
		if(cmd[6]=='N'){
			cmd++;
			cmd[0] = 'T'; cmd[1] = 'I'; cmd[2] = 'M'; cmd[3] = 'E';
			cmd[6] = '_'; cmd[9] = '_';
		}else{
			cmd[4] = 'O'; cmd[5] = 'F'; cmd[6] = 'F';
			cmd[7] = '_'; cmd[10] = '_';
		}			
	}else
	if (compStr(cmd, "S_COLOR")){ // lenhh 6 S_COLOR.rrr.ggg.bbb  -> SCOLOR_rrr_ggg_bbb
		cmd++;
		cmd[0]='S'; cmd[6]='_';  cmd[10]='_';  cmd[14]='_';
	}else
	if (compStr(cmd, "OF_LAMP")){	//lenh 7 OF_LAMP	->	OFFLAMP
		cmd[2] = 'F';
	}else
	if (compStr(cmd, "UPDAT_TIME\0")){
		writeStringUART1("$000000SETRTC_");		//$000000SETRTC_hh_mm_ss_dd_MM_yy#
		timeToString((*time).hour,str);			//hh
		writeMStringUART1(str);
		writeStringUART1("_");
		timeToString((*time).minute,str);		//mm
		writeMStringUART1(str);
		writeStringUART1("_");
		timeToString((*time).second,str);		//ss
		writeMStringUART1(str);
		writeStringUART1("_");
		timeToString((*time).date,str);			//dd
		writeMStringUART1(str);
		writeStringUART1("_");
		timeToString((*time).month,str);		//MM
		writeMStringUART1(str);
		writeStringUART1("_");
		timeToString((*time).year,str);			//yy
		writeMStringUART1(str);
		writeStringUART1("#");
		return 1;
	}
	writeStringUART1("$000000");
	writeMStringUART1(cmd);
	writeStringUART1("#");
	return 1;
}
/*
unsigned char sendLedCmd2(TimeStruct* time,char * cmd,char * receiData){
	char str[3],ttemp = 0,Tel1[16],Tel2[16],content[25];//Tel1 = +84xxxx;Tel2 = 0xxxx
	unsigned char i,j;

	resetTime();
	i = 0;	
	j = 0;					
	while(cmd[i] != '*'){//Loc lay noi dung tin nhan = abcdefgh...
		if (getTime() > 2000){
			return 0;					
		}
		content[i] = cmd[i];
		i++;
	}
	content[i] = 0;//End str
	i += 1;
	while(cmd[i] != '#'){//Lay so dien thoai da gui SMS +84915454775
		if (getTime() > 2000){
			return 0;					
		}
		Tel1[j] = cmd[i];   // Tel1 = +84915454775
		i++;
		j++;
	}
	Tel1[j] = 0;
	Tel2[0] = '0';
	for(i=3;i<j;i++){
		Tel2[i-2] = Tel1[i];
	}
	Tel2[i-2] = '\r';
	Tel2[i-1] = '\n';// Tel2 = 0915454775\r\n
	Tel2[i]   = 0;
	i = 0;

	flushUART1RX();
	writeStringUART1("A+CMTI: 1\r\n\r");
	_delay_ms(50);
	do{
		writeStringUART1("A+CMTI: 1\r\n\r");
		_delay_ms(50);
		i++;
		if(i>3)
			return 0;
	}while(waitStr("AT+CMGR=1")==0);
	
	writeStringUART1("+CMGR: \"XXX\",\"");//+84912345678\",,\"");
	writeMStringUART1(Tel1);
	writeStringUART1("\",,\"");
	timeToString((*time).date,str);
	writeMStringUART1(str);
	writeStringUART1("/");
	timeToString((*time).month,str);
	writeMStringUART1(str);
	writeStringUART1("/");
	timeToString((*time).year,str);
	writeMStringUART1(str);
	writeStringUART1(",");
	timeToString((*time).hour,str);
	writeMStringUART1(str);
	writeStringUART1(":");
	timeToString((*time).minute,str);
	writeMStringUART1(str);
	writeStringUART1(":");
	timeToString((*time).second,str);
	writeMStringUART1(str);
	writeStringUART1("+28\"\r*");
	writeMStringUART1(content);
	writeStringUART1("#\r\n\r");
	if(waitStr("AT+CMGS=")){//0912345678\r\n")){
		if(waitMStr(Tel2)){
			resetTime();
			i = 0;
			while (ttemp != 0x1A){
				while (queueLengthUART1() == 0){
					if (getTime() > 8000){
						writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
						//writeStringUART1("+CMGS: 1\r\n");//+CMGS:<space>1<\r\n>
						return 0;					
					}
				}
				ttemp = readUART1();//1-10
				receiData[i+1] = ttemp;		
				i++;
			}
			receiData[0] = i-1;
			receiData[i] = 0;
			writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
			return 1;
		}
	}
	writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
	//writeStringUART1("+CMGS: 1\r\n");//+CMGS:<space>1<\r\n>
	return 0;
}
unsigned char sendLedCmd3(TimeStruct* time,char * cmd,char * receiData){
	char str[3],ttemp = 0;
	unsigned char i = 0;

	flushUART1RX();
	writeStringUART1("A+CMTI: 1\r\n\r");
	_delay_ms(50);
	do{
		writeStringUART1("A+CMTI: 1\r\n\r");
		_delay_ms(50);
		i++;
		if(i>3)
			return 0;
	}while(waitStr("AT+CMGR=1")==0);

	writeStringUART1("+CMGR: \"XXX\",\"+84912345678\",,\"");
	timeToString((*time).date,str);
	writeMStringUART1(str);
	writeStringUART1("/");
	timeToString((*time).month,str);
	writeMStringUART1(str);
	writeStringUART1("/");
	timeToString((*time).year,str);
	writeMStringUART1(str);
	writeStringUART1(",");
	timeToString((*time).hour,str);
	writeMStringUART1(str);
	writeStringUART1(":");
	timeToString((*time).minute,str);
	writeMStringUART1(str);
	writeStringUART1(":");
	timeToString((*time).second,str);
	writeMStringUART1(str);
	writeStringUART1("+28\"\r*");
	i = 0;
	resetTime();
	while(cmd[i] != '*'){//Send command
		if (getTime() > 2000){
			return 0;					
		}
		writeUART1(cmd[i]);
		if(i<99){
			i++;
		}
	}
	writeStringUART1("#\r\n\r");
	if(waitStr("AT+CMGS=0912345678\r\n")){
	//if(waitStr("bbb")){
		resetTime();
		i = 0;
		while (ttemp != 0x1A){
			while (queueLengthUART1() == 0){
				if (getTime() > 8000){
					writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
					//writeStringUART1("+CMGS: 1\r\n");//+CMGS:<space>1<\r\n>
					return 0;					
				}
			}
			ttemp = readUART1();//1-10
			receiData[i+1] = ttemp;		
			i++;
		}
		receiData[0] = i-1;
		receiData[i] = 0;
		writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
		return 1;
	}
	writeStringUART1("OK\r\n\r");//+CMGS:<space>1<\r\n>
	//writeStringUART1("+CMGS: 1\r\n");//+CMGS:<space>1<\r\n>
	return 0;
}
*/

unsigned char compStr(char * comp_str, prog_char * str){
	unsigned char i=0;
	
	while (i < strlen(comp_str)){
		if( str[i] == comp_str[i]) i++;
		else return 0;
	}
	return 1;
}

/*unsigned char waitStr(prog_char * str){
	unsigned char i;
	char ttemp;
	resetTime();
	i = 0;
	while (i < strlen(str)){
		while (queueLengthUART1() == 0){
			if (getTime() > 5000) return 0;
		}
		ttemp = readUART1();			
		if (ttemp == str[i]) i++;
		else i = 0;
	}
	return 1;
}
unsigned char waitMStr(char * str){
	unsigned char i;
	char ttemp;
	resetTime();
	i = 0;
	while (i < strlen(str)){
		while (queueLengthUART1() == 0){
			if (getTime() > 5000) return 0;
		}
		ttemp = readUART1();			
		if (ttemp == str[i]) i++;
		else i = 0;
	}
	return 1;
}*/


ISR(SIG_UART0_RECV)
{ 
	if(Uart_Select == 0){//ModBus
		
		CLEAR_MB_TIME();
		temp = (char)UDR0;
		if(nMBState == STATE_MB_INIT )
		{
			START_MB_TIME35();
			return;
		}
		if(nMBState == STATE_MB_IDLE )
		{
			//RXLED_ON;
			pInBuff[0] = temp;
			nInBuffCount = 1;			
			START_MB_TIME15();		//start t15 
			nMBState = STATE_MB_RECEPTION ;
			return;
		}

		if(nMBState == STATE_MB_RECEPTION )
		{		
			if(nInBuffCount>=MAX_IN_BUF)
			{
				nInBuffCount = MAX_IN_BUF-1;
				nMBState = STATE_MB_FRM_ERR;
				START_MB_TIME35();		//start t15 
				//RXLED_ON;
				return;
			}

			pInBuff[nInBuffCount] = temp;
			nInBuffCount++ ;
			START_MB_TIME15();		//start t15 
			return;
		}

		if(nMBState == STATE_MB_ISERR)
		{
			nMBState = STATE_MB_FRM_ERR;
			START_MB_TIME35();		//start t35 
			return;
		}

		if(nMBState == STATE_MB_FRM_ERR)
		{
			START_MB_TIME35();		//start t35 
			return;
		}
		START_MB_TIME35();
	}else{//UART binh thuong
		queueUART1[bottomQueue1] = (char) UDR0;
		//currentUart0 = queueUART1[bottomQueue1];
		bottomQueue1++;
		if (bottomQueue1 == MAX_QUEUE1) bottomQueue1 = 0;
	}
}
void clearFlag(unsigned char flag){ //flag: 1 --> 97 
	outBytes[flag+31] = 0xFF;
}

ISR(USART0_TX_vect)
{	
	if(Uart_Select == 0){
		if(nMBState == STATE_MB_EMISSION)
		{
			nByteSent++;
			if(nByteSent < nOutBuffCount)
			{
				UDR0 = pOutBuff[nByteSent];
				CLEAR_MB_TIME();
			}
			else
			{
				START_MB_TIME35();
				//DISABLE_TX;
			}	
			return;
		}
		DISABLE_TX;
		nMBState = STATE_MB_IDLE ;
	}
	else{

	}
	//Disable Transmiter
	
}
ISR(TIMER2_OVF_vect)
{
	CLEAR_MB_TIME();			//Stop counter
	if(Uart_Select == 0){
		if(nMBState == STATE_MB_INIT )
		{
			temp = UDR0;
			nMBState = STATE_MB_IDLE ;
			return ;
		}

		if(nMBState == STATE_MB_RECEPTION )
		{
			nMBState = STATE_MB_ISERR;
			START_MB_TIME2();
			//RXLED_OFF;
			return ;
		}
	
		if(nMBState == STATE_MB_ISERR)
		{				
			nMBState = STATE_MB_FRM_CHECK;
			//RXLED_OFF;
			return ;
		}	
		if(nMBState == STATE_MB_FRM_ERR)
		{	
			nMBState = STATE_MB_FRM_NOK;
			//RXLED_OFF;
			return ;
		}	
		if(nMBState == STATE_MB_EMISSION)
		{
			temp = (char) UDR0;
			nMBState = STATE_MB_IDLE ;
			DISABLE_TX;
			//TXLED_OFF;
			return ;
		}
	}
	else{

	}
	
	
}
