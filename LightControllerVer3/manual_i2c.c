/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include <avr/io.h>
#include "manual_i2c.h"

#define SCL_LOW()	I2CDDR |=	SCLLO		/* SCL = LOW */
#define SCL_HIGH()	I2CDDR &=	SCLHI		/* SCL = High-Z */
#define	SCL_VAL		((I2CPIN & SCLLO) ? 1 : 0)	/* SCL input level */
#define SDA_LOW()	I2CDDR |=	SDALO		/* SDA = LOW */
#define SDA_HIGH()	I2CDDR &=	SDAHI		/* SDA = High-Z */
#define	SDA_VAL		((I2CPIN & SDALO) ? 1 : 0)	/* SDA input level */

void i2cDelay (void){
	unsigned int n;
	for(n = 70;n;n--){
		asm("NOP");
	}
}
/* Generate start condition on the IIC bus */
void i2cStart (void){
	SDA_HIGH();
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	SDA_LOW();
	i2cDelay();
	SCL_LOW();
	i2cDelay();
}
/* Generate stop condition on the IIC bus */

void i2cStop (void){
	SDA_LOW();
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	SDA_HIGH();
	i2cDelay();
}
/* Send a byte to the IIC bus */

unsigned char i2cWrite (unsigned char dat){
	unsigned char b = 0x80;
	unsigned char ack;
	do {
		if (dat & b)	 {	/* SDA = Z/L */
			SDA_HIGH();
		} else {
			SDA_LOW();
		}
		i2cDelay();
		SCL_HIGH();
		i2cDelay();
		SCL_LOW();
		i2cDelay();
	} while (b >>= 1);
	SDA_HIGH();
	i2cDelay();
	SCL_HIGH();
	ack = SDA_VAL ? 0 : 1;	/* Sample ACK */
	i2cDelay(); 
	SCL_LOW();
	i2cDelay();
	return ack;
}
/* Receive a byte from the IIC bus */
unsigned char i2cRead (unsigned char ack){
	unsigned int d = 1;
	do {
		d <<= 1;
		SCL_HIGH();
		if (SDA_VAL) d++;
		i2cDelay();
		SCL_LOW();
		i2cDelay();
	} while (d < 0x100);
	if (ack) {		/* SDA = ACK */
		SDA_LOW();
	} else {
		SDA_HIGH();
	}
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	SCL_LOW();
	SDA_HIGH();
	i2cDelay();
	return (unsigned char)d;
}

    
