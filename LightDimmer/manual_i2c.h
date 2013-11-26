#ifndef _I2C_INCLUDED_
#define _I2C_INCLUDED_

#define SCLpin	5
#define SDApin	6

#define I2CDDR	DDRD		// I2C DDR define
#define	I2CPIN	PIND		// I2C Port in

#define	SCLHI	~(1<<SCLpin)	// SCL pin
#define	SCLLO	(1<<SCLpin)		
#define	SDAHI	~(1<<SDApin)	// SDA pin
#define	SDALO	(1<<SDApin)

void i2cDelay (void);
void i2cStart (void);
void i2cStop (void);
unsigned char i2cWrite (unsigned char dat);
unsigned char i2cRead (unsigned char ack);

#endif
