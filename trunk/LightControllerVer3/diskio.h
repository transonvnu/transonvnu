/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.05   (C)ChaN, 2007
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#define _READONLY	0	/* 1: Read-only mode */
#define _USE_IOCTL	1

#include "integer.h"
#include <avr/io.h>

BYTE temp;
/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

//static

void power_on (void);

void power_off (void);

DSTATUS disk_initialize (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);
#if	_READONLY == 0
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);
#endif
DRESULT disk_ioctl (BYTE, BYTE, void*);

//	PORTB.0	SS
//	PORTB.1	CLK
//	PORTB.2	DI
//	PORTB.3	DO
//	PORTB.4	CS_ADE
//	PORTB.5	Power sw
//	PORTE.7	INS
//	PORTE.6	WP


/* Port Controls  (Platform dependent) */
#define CS_MASK     0x01                    //0000 0001
#define SELECT()	PORTB &= ~CS_MASK		/* MMC CS = L */
#define	DESELECT()	PORTB |= CS_MASK		/* MMC CS = H */

//#define SOCKPORT	PINE			/* Socket contact port */
//#define SOCKET		PORTE	
//#define SOCKDDR		DDRE
//#define SOCKWP		0x40			/* Write protect switch (PE6) */   //0100 0000
//#define SOCKINS		0x80			// PE7	1000 0000
//#define SOCKINIT SOCKDDR &= ~(SOCKWP | SOCKINS)

#define SPI_PORT		PORTB
#define SPI_DDR			DDRB
														 //        0       1         2          3     
#define PORT_EN_DEVICE    (SPI_PORT & 0xF5) | 0x05         //PORT CS = 1; SCK = 0; MOSI = 1; MISO = 0; 
#define DDR_EN_DEVICE     (SPI_DDR  & 0xF7) | 0x07         //DDR  CS = 1; SCK = 1; MOSI = 1; MISO = 0;

#define PORT_DIS_DEVICE   SPI_PORT & 0xF1;               //0000 0xxx PORT
#define DDR_DIS_DEVICE    SPI_DDR  & 0xF1;               //0000 0xxx DDR  

/*
#define PW_MASK			0b00100000			// PB5
#define PW_ON  			PORTB &= ~PW_MASK
#define PW_OFF 			PORTB |= PW_MASK
#define PW_ON_INIT		DDRB	|=	PW_MASK */

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22

void	disk_timerproc (void);

#define _DISKIO
#endif
