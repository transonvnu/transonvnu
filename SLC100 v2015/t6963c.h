//                           1001   Graphic   Text    Curser_enable   Blink_enable

/* ----------------------------------------------------------
 * Program to control a T6963C-based 240x64 pixel LCD display
 * using the PC's Parallel Port (LPT1:) in bidirectional mode
 * written in Microsoft Quick C
 *
 * Written by John P. Beale May 3-4, 1997  beale@best.com
 *
 *  Based on information from Steve Lawther,
 *  "Writing Software for T6963C based Graphic LCDs", 1997 which is at
 *  http://ourworld.compuserve.com/homepages/steve_lawther/t6963c.pdf
 *
 *  and the Toshiba T6963C data sheet, also on Steve's WWW page
 *
 *  and info at: http://www.citilink.com/~jsampson/lcdindex.htm
 *               http://www.cs.colostate.edu/~hirsch/LCD.html
 *               http://www.hantronix.com/
 * ----------------------------------------------------------
 */
#ifndef _T6962C_H_
#define _T6962C_H_ 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>     // rand()
#include <string.h>     // strlen()
#include <math.h>       // cos(),sin()
#include "ds1307.h"
#include "ade7758.h"

// 18/03/2010  Ver3.0 
// Data        --> PORTF
// RESET       --> PD1
// WRITE       --> PD4
// READ        --> PD5
// CE          --> PD6
// C/D         --> PD7

#define RESET		0x02  //PD1
#define RESETHI   	PORTD |= RESET
#define RESETLO   	PORTD &= ~RESET
#define RESET_DDRHI DDRD  |= RESET
#define RESET_DDRLO DDRD  &= ~RESET

#define WRITE		0x10  //PD4
#define WRHI   		PORTD  |= WRITE 
#define WRLO   		PORTD  &= ~WRITE
#define WR_DDRHI 	DDRD   |= WRITE
#define WR_DDRLO 	DDRD   &= ~WRITE

#define READ		0x20  //PD5
#define RDHI   		PORTD  |= READ
#define RDLO   		PORTD  &= ~READ
#define RD_DDRHI	DDRD   |= READ
#define RD_DDRLO	DDRD   &= ~READ

#define CE			0x40  //PD6
#define CEHI   		PORTD  |= CE
#define CELO   		PORTD  &= ~CE
#define CE_DDRHI	DDRD   |= CE
#define CE_DDRLO	DDRD   &= ~CE

#define CDATA		0x80  //PD7
#define CDHI   		PORTD  |= CDATA
#define CDLO   		PORTD  &= ~ CDATA
#define CD_DDRHI 	DDRD   |= CDATA
#define CD_DDRLO 	DDRD   &= ~ CDATA

#define DATAIN  DDRF=0
#define DATAOUT DDRF=0XFF

#define DATAPIN  PINF
#define DATAPORT PORTF

/* ----- Definitions concerning LCD internal memory  ------ */

#define G_BASE 0x0200            // base address of graphics memory
#define T_BASE 0x0000            // base address of text memory
#define BYTES_PER_ROW 30         // how many bytes per row on screen
#define BYTES_PER_ROWG 30
/* ----------------------------------------------------------- */

void dput(int byte); 			 // write data byte to LCD module
int  dget(void);    			 // get data byte from LCD module
int  sget(void);  			 // check LCD display status pbrt
void cput(int byte);			 // write command byte to LCD module
void lcd_print(int,int,const char *string);  	 // send string of characters to LCD
void lcd_print_ram(int,int,char *string);  	 // send string of characters to LCD
void lcd_set_address(unsigned int addr);

void lcd_clear_graph();    		 // clear graphics memory of LCD
void lcd_clear_text();  		 // clear text memory of LCD

void lcd_xy(int x, int y); 		 // set memory pointer to (x,y) position (text)
void lcd_clrpixel(int column, int row);  // set single pixel in 240x64 array
void lcd_setpixel(int column, int row);  // set single pixel in 240x64 array
void lcd_pixel(int column, int row,char show);

//void show(const char * data PROGMEM,int,int);
void lcd_line(int x1, int y1, int x2, int y2, unsigned char show);
void lcd_line_ver(unsigned char row,unsigned char s_col,unsigned char f_col,char show );
//void lcd_circle(int x, int y, int radius, unsigned char show);
//void lcd_circle_half(int x, int y, int radius, unsigned char show);
void lcd_box(int x1, int y1, int x2, int y2, unsigned char show);
void lcd_box_fill(int x1, int y1, int x2, int y2, unsigned char show);
//void lcd_degree_line(int x, int y, int degree, int inner_radius, int outer_radius, unsigned char show);
//void lcd_degree_line_bold(int x, int y, int degree, int inner_radius, int outer_radius, unsigned char show);
//void lcd_fill(int x1, int y1, int x2, int y2, unsigned char persent,char first);

void lcd_setup();			 // make sure control lines are at correct levels
void lcd_init();     			 // initialize LCD memory and display modes

#define XMAX 240        // limits of (x,y) LCD graphics drawing
#define XMIN 0
#define YMAX 128
#define YMIN 0


#endif /* _T6962C_H_ */

