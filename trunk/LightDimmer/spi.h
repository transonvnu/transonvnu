/*
  CodeVisionAVR C Compiler
  (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.

  Prototype for SPI access function */
  
#ifndef _SPI_INCLUDED_
#define _SPI_INCLUDED_

void initSPI(void);
unsigned char spi(unsigned char data);

#endif

