#ifndef _I2C_INCLUDED_
#define _I2C_INCLUDED_

#define I2CDDR	DDRG		// I2C DDR define
#define	I2CPIN	PING		// I2C Port in
#define	SCLHI	0xEF		// SCL pin 4
#define	SCLLO	0x10		
#define	SDAHI	0xF7		// SDA pin 3
#define	SDALO	0x08

void i2cDelay (void);
void i2cStart (void);
void i2cStop (void);
unsigned char i2cWrite (unsigned char dat);
unsigned char i2cRead (unsigned char ack);

#endif
