/*
 * main.h
 *
 * Created: 13/01/2014 18:47:10 PM
 *  Author: Administrator
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define	ADC_VCELL	0		// NimH battery voltage
#define	ADC_ILAMP	1		// current output to lamp
#define	ADC_VBAT1	2		// Vbat1
#define	ADC_MAIN	3		// DC supply voltage
#define	ADC_VBATT	4		// Vbat1 + Vbat2
#define	ADC_ICHR2	5		// Charger current to battery 2
#define	ADC_ICHR1	6		// Charger current to battery 1
#define	ADC_VACIN	7		// AC voltage
#define CMD_CHANGEID	9
#define CMD_READID		10

#define USE_BAT		1
#define USE_MAIN	0

#define IVP_MINIMUM 200
#define SUPPLY_SOURCE_ADDR	12	

extern unsigned char mcucsr __attribute__((section(".noinit")));
extern char dateCompiler[];
extern char NRID_RAM[];
extern unsigned int	NiMH_voltage;
extern unsigned char fail_RTC;
extern unsigned char node_ID;

extern unsigned char	supply_source,OldSupply_source;
extern unsigned int	Umain;
extern unsigned int	Uvp,Ubi,Umi,Iacin,Ivp,Ub1,Ub2;
extern unsigned int	Ich1, Ich2;

union longintpak{
	long lval;
	unsigned char bytes[4];
};


void writelVal_NVRAM(long _val, unsigned char _addr);
long readlVal_NVRAM(unsigned char _addr);
unsigned char getChangedSupplySrc();
unsigned char getSupplySrc();
void enbClockBar();
void disClockBar();
unsigned char getRefreshFlag(void);
void setRefreshable();
void displayEbat(char * _str);

#endif /* MAIN_H_ */