/*
 * ds1307.c
 *
 * Created: 09/09/2013 12:21:27 CH
 *  Author: SCEC
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <stdio.h>
#include "ds1307.h"
#include "relay.h"
#include "../base.h"

#define MAX_TRIES 50
#define DS1307_ID    0xD0        // I2C DS1307 Device Identifier
#define DS1307_ADDR  0x00        // I2C DS1307 Device Address
#define I2C_START 0
#define I2C_DATA 1
#define I2C_DATA_ACK 2
#define I2C_STOP 3
#define ACK 1
#define NACK 0

#define HOUR_24 0
#define HOUR_12 1

#define SQWOUT_PORT		PORTE
#define SQWOUT_DDR		DDRE
#define SQWOUT_PIN		PINE
#define SQWE	4
//-------Rate select------------
//	RS1		RS2		F_out
//	0		0		1Hz
//	0		1		4.096kHz
//	1		0		8.192kHz
//	1		1		32.768kHz
#define	RATESL	0x00

//	bit:	0	1	2	3	4	5	6
//	data:	ss	mm	hh	dd	DD	MM	YY
//
char hour_mode, ampm_mode;
//char *weekday[]={"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
//char *month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	
volatile static	unsigned char RTCtick1s;

/* START I2C Routine */
unsigned char i2c_transmit(unsigned char type) {
  switch(type) {
     case I2C_START:    // Send Start Condition
       TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
       break;
     case I2C_DATA:     // Send Data with No-Acknowledge
       TWCR = (1 << TWINT) | (1 << TWEN);
	   break;
     case I2C_DATA_ACK: // Send Data with Acknowledge
       TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
	   break;
     case I2C_STOP:     // Send Stop Condition
	   TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	   return 0;
  }
  // Wait for TWINT flag set on Register TWCR
  while (!(TWCR & (1 << TWINT)));
  // Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
  return (TWSR & 0xF8);
}

char i2c_start(unsigned int dev_id, unsigned int dev_addr, unsigned char rw_type)
{
  unsigned char n = 0;
  unsigned char twi_status;
  char r_val = -1;
i2c_retry:
  if (n++ >= MAX_TRIES) return r_val;
  // Transmit Start Condition
  twi_status=i2c_transmit(I2C_START);

  // Check the TWI Status
  if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
  if ((twi_status != TW_START) && (twi_status != TW_REP_START)) goto i2c_quit;
  // Send slave address (SLA_W)
  TWDR = (dev_id & 0xF0) | (dev_addr & 0x07) | rw_type;
  // Transmit I2C Data
  twi_status=i2c_transmit(I2C_DATA);
  // Check the TWSR status
  if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) goto i2c_retry;
  if (twi_status != TW_MT_SLA_ACK) goto i2c_quit;
  r_val=0;
i2c_quit:
  return r_val;
/*  
	for(n = 0; n< MAX_TRIES; n++){
		// Transmit Start Condition
		twi_status=i2c_transmit(I2C_START);
		// Check the TWI Status
		if (twi_status == TW_MT_ARB_LOST) continue;
		if ((twi_status != TW_START) && (twi_status != TW_REP_START)) break;
		// Send slave address (SLA_W)
		TWDR = (dev_id & 0xF0) | (dev_addr & 0x07) | rw_type;
		// Transmit I2C Data
		twi_status=i2c_transmit(I2C_DATA);
		// Check the TWSR status
		if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) continue;
		if (twi_status != TW_MT_SLA_ACK) break;
	}
	return -1;
*/
}

void i2c_stop(void)
{
  unsigned char twi_status;
  // Transmit I2C Data
  twi_status=i2c_transmit(I2C_STOP);
}

char i2c_write(char data)
{
  unsigned char twi_status;
  char r_val = -1;
  // Send the Data to I2C Bus
  TWDR = data;
  // Transmit I2C Data
  twi_status=i2c_transmit(I2C_DATA);
  // Check the TWSR status
  if (twi_status != TW_MT_DATA_ACK) goto i2c_quit;
  r_val=0;
i2c_quit:
  return r_val;
}
char i2c_read(char *data,char ack_type)
{
  unsigned char twi_status;
  char r_val = -1;               

  if (ack_type) {
    // Read I2C Data and Send Acknowledge
    twi_status=i2c_transmit(I2C_DATA_ACK);
    if (twi_status != TW_MR_DATA_ACK) goto i2c_quit;
  } else {
    // Read I2C Data and Send No Acknowledge
    twi_status=i2c_transmit(I2C_DATA);
    if (twi_status != TW_MR_DATA_NACK) goto i2c_quit;
  }
  // Get the Data
  *data=TWDR;
  r_val=0;
i2c_quit:
  return r_val;
}

// Convert Decimal to Binary Coded Decimal (BCD)
char dec2bcd(char num)
{
  return ((num/10 * 16) + (num % 10));
}
// Convert Binary Coded Decimal (BCD) to Decimal
char bcd2dec(char num)
{
  return ((num/16 * 10) + (num % 16));
}

void Read_DS1307(struct _RTCDATA * RTCdata)
{
   char data;
   // First we initial the pointer register to address 0x00
   // Start the I2C Write Transmission
   i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
    // Start from Address 0x00
   i2c_write(0x00);
   // Stop I2C Transmission
   i2c_stop();

   // Start the I2C Read Transmission
   i2c_start(DS1307_ID,DS1307_ADDR,TW_READ);
   // Read the Second Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   (*RTCdata).second=bcd2dec(data & 0x7F);
   // Read the Minute Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   (*RTCdata).minute=bcd2dec(data);
   // Read the Hour Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   /*if ((data & 0x40) == 0x40) {
     hour_mode = HOUR_12;
	 ampm_mode=(data & 0x20) >> 5;   // ampm_mode: 0-AM, 1-PM
	 (*RTCdata).hour=bcd2dec(data & 0x1F);
   } else {
     hour_mode = HOUR_24;
	 ampm_mode=0;
     (*RTCdata).hour=bcd2dec(data & 0x3F);
   }*/
   (*RTCdata).hour=bcd2dec(data & 0x3F);
   // Read the Day of Week Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   (*RTCdata).day=bcd2dec(data);
   // Read the Date of Month Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   (*RTCdata).date=bcd2dec(data);
   // Read the Month Register, Send Master Acknowledge
   i2c_read(&data,ACK);
   (*RTCdata).month=bcd2dec(data);
   // Read the Year Register, Send Master No Acknowledge
   i2c_read(&data,NACK);
   (*RTCdata).year=bcd2dec(data);
   // Stop I2C Transmission
   i2c_stop();
}
void Write_DS1307(struct _RTCDATA * RTCdata)
{
	char ds1307_addr[7];
   unsigned char i, hour_format;
   ds1307_addr[0] = RTCdata->second;
   ds1307_addr[1] = RTCdata->minute;
   ds1307_addr[2] = RTCdata->hour;
   ds1307_addr[3] = RTCdata->day;
   ds1307_addr[4] = RTCdata->date;
   ds1307_addr[5] = RTCdata->month;
   ds1307_addr[6] = RTCdata->year;
   
   
   // Make sure we enable the Oscillator control bit CH=0 on Register 0x00
   ds1307_addr[0]=ds1307_addr[0] & 0x7F;
   // Start the I2C Write Transmission
   i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
   // Start from Address 0x00
   i2c_write(0x00);
   // Write the data to the DS1307 address start at 0x00
   // DS1307 automatically will increase the Address.
   for (i=0; i<7; i++) {
     if (i == 2) {
	   hour_format=dec2bcd(ds1307_addr[i]);
	   /*if (hour_mode) {
	     hour_format |= (1 << 6);
   	     if (ampm_mode)
	       hour_format |= (1 << 5);
         else
	       hour_format &= ~(1 << 5);
	   } else {
	     hour_format &= ~(1 << 6);
       }*/
	   hour_format &= ~(1 << 6);
	   //hour_format &= ~(1 << 5);
	   i2c_write(hour_format);
	 } else {
       i2c_write(dec2bcd(ds1307_addr[i]));
     }
   }
   // Stop I2C Transmission
   i2c_stop();
}

void Write_NVRAM(char * _data, unsigned char _addr, unsigned char _Nbyt){
	unsigned char i;
	// Start the I2C Write Transmission
	i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
	// Start from Address 0x00
	i2c_write(_addr);
	for(i = 0;i<_Nbyt;i++){
		i2c_write(_data[i]);
	}
	// Stop I2C Transmission
	i2c_stop();	
}
void writeByte_NVRAM(char _data,unsigned char _addr){
	i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
	// Start from Address 0x00
	i2c_write(_addr);
	i2c_write(_data);
	// Stop I2C Transmission
	i2c_stop();
}

void Read_NVRAM(char * _data, unsigned char _addr, unsigned char _Nbyt ){
	unsigned char i;
	// First we initial the pointer register to address _addr
	// Start the I2C Write Transmission
	i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
	// Start from Address 0x00
	i2c_write(_addr);
	// Stop I2C Transmission
	i2c_stop();
	// Start the I2C Read Transmission
	i2c_start(DS1307_ID,DS1307_ADDR,TW_READ);
	// Read the Second Register, Send Master Acknowledge
	for(i = 0; (i+1)<_Nbyt;i++)
		i2c_read(_data + i,ACK);
	i2c_read(_data + i,NACK);
	// Stop I2C Transmission
	i2c_stop();
}

char readByte_NVRAM(unsigned char _addr){
	char _data;
	// First we initial the pointer register to address _addr
	// Start the I2C Write Transmission
	i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
	// Start from Address 0x00
	i2c_write(_addr);
	// Stop I2C Transmission
	i2c_stop();
	// Start the I2C Read Transmission
	i2c_start(DS1307_ID,DS1307_ADDR,TW_READ);
	// Read the Second Register, Send Master Acknowledge
	i2c_read(&_data,NACK);
	// Stop I2C Transmission
	i2c_stop();
	return _data;
}

void SQWEnable (void){
	// Start the I2C Write Transmission
   i2c_start(DS1307_ID,DS1307_ADDR,TW_WRITE);
   // Start from Address 0x00
   i2c_write(0x07);
   i2c_write(_BV(SQWE)|RATESL);
   // Stop I2C Transmission
   i2c_stop();
}

void ExtINT4Init(){
	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	// INT2: Off
	// INT3: Off
	// INT4: On
	// INT4 Mode: Falling Edge
	// INT5: Off
	// INT6: Off
	// INT7: Off
	EICRA=0x00;
	EICRB=0x02;
	EIMSK=0x10;
	EIFR=0x10;
	cbi(DDRE,4);
	sbi(PORTE,4);
}

void TWIInit(void){
	TWSR = 0x00;   // Select Prescaler of 1
	// SCL frequency = 14745600 / (16 + 2 * 66 * 1) = 99,632 khz
	TWBR = 66;
	SQWEnable();
}

unsigned char checkSWQ1s(void){
	if(RTCtick1s){RTCtick1s = 0; return 1;}
	return 0;
}

ISR(INT4_vect){
	RTCtick1s = 1;
	//PORT_RL_SEL_SOURCE ^= _BV(RL_SEL_SOURCE);
}
