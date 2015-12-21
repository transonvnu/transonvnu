/*
 * ks0108.c
 *
 * Created: 31/08/2013 11:11:49 SA
 *  Author: SCEC
 */ 
/**************************************************************/
#include "ks0108.h"
#include <util/delay.h>
#include "font5x7.h"

#define DefaultRow		0xc0
#define DefaultColumn	0x40		// 0b01000000
#define DefaultPage		0xb8		// 0b10111000                                                            

#define uchar unsigned char
#define uint unsigned int


union _ROWBUF
{
	uchar AllRow[128];
	struct
	{
		uchar LRow[64];
		uchar RRow[64];
	};
};

union _ROWBUF Display_Buf [8];
uchar * Dis_Buf = (uchar *)Display_Buf;
uchar bufferChanged = 0;

uchar AREA=1;
uchar Block0;
uchar Block1;
uchar End=0;
uchar error_flag=0;


/*************************************************/
/*      West Text Database   8*16                          */
/*************************************************/
/*
const uchar CTAB[0x60][0x10] PROGMEM = \
{\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,
0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,
0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,
0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,
0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,
0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,
0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,
0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,
0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,
0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,
0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,
0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,
0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,
0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,
0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,
0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,
0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,
0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,
0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,
0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,
0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,
0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,
0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,
0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,
0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,
0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,
0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,
0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,
0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,
0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,
0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,
0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,
0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,
0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,
0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,
0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,
0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,
0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,
0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,
0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,
0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,
0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,
0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,
0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,
0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,
0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,
0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,
0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,
0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,
0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,
0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,
0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,
0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,
0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,
0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,
0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,
0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,
0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,
0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,
0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,
0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,
0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,
0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,
0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,
0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,
0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,
0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,
0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,
0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,
0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,
0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,
0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,
0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,
0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,
0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};*/



//////////////////Underlyingfunction////////////////////////////////////////////////////////////////


/*************************************************/
/*      LCDInitialization function                            */
/*************************************************/
void InitialLcd(void)
{
	MCUCR |= (1<<SRE)|(1<<SRW10);             //Enable bus
	XMCRA |= (1<<SRW00)|(1<<SRW01)|(1<<SRW11);//Added to the waiting
	XMCRB |= (1<<XMM0);
	

	SET_RST_OUTPUT();			//Let RST is output
	SET_CSA_OUTPUT();			//?CSA???		
	SET_CSB_OUTPUT();			//?CSB???
	SET_CSA();					//CSA CSB set to an invalid state
	SET_CSB();
	CLR_RST();					//Pull to RST Reset
	LCDDelay(255);				//Delay
	SET_RST();					//Pulled
	LCDDelay(255);				//??
	DisStartLine(0);
    ShowLcd(0x00);				//Off Display
	ClearDisBuf(0x00);				//Clear the screen
	UpdateDis();
	LocateLcdPageColumn(0x00,0x00);//Positioning display point		
	ShowLcd(0x01 );				//On Display
}


/*************************************************/
/*      Sub-busy function error brought out by the global variable error_flag    */
/*************************************************/
void BusyOrNot()
{
	uchar _data,i=0;
	while(1)
	{
		_data = CMD_READ;      //Status read command
		_data=0x90 & _data;
		if(_data==0x00) break;//Compare
		if(i++==255){error_flag=1;return;}//Timeout error
	}
	_delay_us(10);
}

/*************************************************/
/*      Write command to the right area                            */
/*************************************************/
void WriteCommandToRightArea(uchar Command)
{
	CLR_CSB();			//Select the right area
	//BusyOrNot();		//Sub-busy
			_delay_us(10);
	CMD_WRITE = Command;//Write command
	SET_CSB();			//Release the right area
}

/*************************************************/
/*      Write data to the right area                             */
/*************************************************/
void WriteDataToRightArea(uchar _Data)
{
	CLR_CSB();			//Select the right area
	//BusyOrNot();		//Sub-busy
			_delay_us(10);
	DAT_WRITE = _Data;	//Write Data
	SET_CSB();			//Release the right area
}

/*************************************************/
/*      Left area write command                            */
/*************************************************/
void WriteCommandToLeftArea(uchar Command)
{
	CLR_CSA();			//Select the right area
	//BusyOrNot();		//??
			_delay_us(10);
	CMD_WRITE = Command;//???
	SET_CSA();			//Release the right area
}

/*************************************************/
/*      Write data to the left area                            */
/*************************************************/
void WriteDataToLeftArea(uchar _Data)
{
	CLR_CSA();			//Select the right area
	//BusyOrNot();		//??
			_delay_us(10);
	DAT_WRITE = _Data;	//???
	SET_CSA();			//????
}


unsigned char checkBufferChanged(void){
	if(bufferChanged){
		bufferChanged = 0;
		return 1;
	}else
	{
	  return 0;
	}	
}

/*************************************************/
/*      Clear LCD screen                                  */
/*************************************************/
void ClearDisBuf(uchar val)
{
	uint i;
	for(i=0;i<1024;i++)Dis_Buf[i]=val;
	bufferChanged = 1;
}

/*************************************************/
/*      Update Display                                 */
/*************************************************/
void UpdateDis(void)
{
	uchar Page,Column;
	uchar *p = Dis_Buf+1023;
	//LocateLcdPageColumn(0,64);//Positioning the right page and column area
	for(Page=0;Page<8;Page++)
	{
		//Column=0;
		WriteCommandToRightArea(DefaultColumn);
		WriteCommandToRightArea(DefaultPage|Page);
		//LocateLcdPageColumn(Page,Column);//Positioning the right page and column area
		for(Column=0;Column<64;Column++){
			WriteDataToRightArea(*(p--));//Write Right Area
			_delay_us(10);
		}
		WriteCommandToLeftArea(DefaultColumn);
		WriteCommandToLeftArea(DefaultPage|Page);		
		//LocateLcdPageColumn(Page,Column);//Positioning the left page and column area
		for(Column=64;Column<128;Column++){
			WriteDataToLeftArea(*p--);//Write the left area
			_delay_us(10);
		}
	}
	LocateLcdPageColumn(0x00,0x00);//Positioning display point		
	ShowLcd(0x01 );				//On Display
}


/*************************************************/
/*      Switch LCD Display                              */
/*************************************************/
void ShowLcd(uchar on)//LCDThe display switch, on = 0x00, off; on = 0x01, open.
{
	uchar temp;
	temp=0x3E;			//00111110B
	temp|=on;
	WriteCommandToRightArea(temp);
	WriteCommandToLeftArea(temp);
}
void DisStartLine(unsigned char LineN){
	uchar temp;
	temp=0xC0;			//11000000B
	temp|=LineN;
	WriteCommandToRightArea(temp);
	WriteCommandToLeftArea(temp);
}

//Positioning column
void LocateLcdColumn(uchar Column)
{
	uchar temp = Column;
	//temp = DefaultColumn;//01000000B
	//temp += Column;					   
	if(Column>63)
	{
		temp -= 64;		        //The data is written to indicate the right area
		WriteCommandToRightArea(temp|DefaultColumn); //Positioning the right column area
	}
	else WriteCommandToLeftArea(temp|DefaultColumn);//Positioning the left column area	   
}

//Positioning page
void LocateLcdPage(uchar Page)
{
	uchar temp;
	temp=DefaultPage;					//10111000B;
	temp|=Page;
	WriteCommandToLeftArea(temp);
	WriteCommandToRightArea(temp);
}

//Positioning page column
void LocateLcdPageColumn(uchar Page,uchar Column)
{
	LocateLcdPage(Page);
	LocateLcdColumn(Column);
}



////////////////Above the underlying function ///////////////////////////////////////////////////////////////

/*************************************************/
/*    Padding a pixel data at x, y coordinate val is 0 or 1      */
/*************************************************/
void OnePixel(uchar x,uchar y,uchar val)
{
	uchar *p = &Dis_Buf[ ((y>>3)<<7) + x ];//Find the corresponding byte
	if(val)*p |= (1<<(y%8));		//Modify the corresponding position
	else *p &= ~(1<<(y%8));
	bufferChanged = 1;
}

unsigned char ReadPixel(uchar x,uchar y)
{
	uchar *p = &Dis_Buf[ ((y>>3)<<7) + x ];//Find the corresponding byte
	if(*p & (1<<(y%8)))return 1;
	else return 0;
}



void LCDDelay(uint n)
{
	while(n--);
}
//	Function putchar into display RAM buffer
// font 5x7	( lcd text ( 8 line - 21 text)
//
void f5x7_putchar(\
unsigned char x,\
unsigned char y,\
unsigned char chr_dat\
){
	unsigned char i=0, font_byte;	// , font_temp;
	prog_char * font_offset;
	font_offset = (prog_char *)font1_char_table + (chr_dat-32)*5;
	x = x*6+1;
	for(i=0;i<5;i++){
		/*
		font_byte = 0;
		font_temp = pgm_read_byte(font_offset+i);
		for(j=0;j<8;j++){
			if((font_temp&(0x80>>j))!=0x00) font_byte |= (1<<j);
		}
		Display_Buf[y].AllRow[x++] = font_byte;
		*/
		font_byte = pgm_read_byte(font_offset+i);
		Display_Buf[y].AllRow[x++] = font_byte;
	}
	Display_Buf[y].AllRow[x++] = 0x00;
	bufferChanged = 1;	
	return;
}

void f5x7_putsf(unsigned char x, unsigned char y, prog_char * str){
	unsigned char i=0;
	while(pgm_read_byte(str+i) != 0x00){
		f5x7_putchar(x+i,y,pgm_read_byte(str+i));
		i++;
	}
	bufferChanged = 1;
}

void f5x7_puts(unsigned char x, unsigned char y, char *str){
	unsigned char i=0;
	while(str[i] != 0x00){
		f5x7_putchar(x+i,y,str[i]);
		i++;
	}
	bufferChanged = 1;
} 

void clearROW(unsigned char _idxrow){
	unsigned char i;
	for(i=0;i<MAXCOLUMN;i++){
		Display_Buf[_idxrow].AllRow[i] = 0;
	}
}
void invertROW(unsigned char _idxrow){
	unsigned char i;
	for(i=0;i<MAXCOLUMN;i++){
		Display_Buf[_idxrow].AllRow[i] = ~Display_Buf[_idxrow].AllRow[i];
	}
}