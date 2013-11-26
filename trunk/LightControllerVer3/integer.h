/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/
//#include <stdlib.h>
#ifndef _INTEGER
//#define EXBYTE unsigned char __attribute__ ((section (".extmem")))
/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

//typedef unsigned char __attribute__ ((section (".extmem"))) EXUCHAR;
//typedef int __attribute__ ((section (".extmem"))) EXINT;
//typedef unsigned int __attribute__ ((section (".extmem"))) EXUINT;
//typedef long __attribute__ ((section (".extmem"))) EXLONG;
//typedef unsigned long __attribute__ ((section (".extmem"))) EXULONG;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOL;

#define _INTEGER
#endif
