/*
 * font5x7.h
 *
 * Created: 05/09/2013 5:35:53 CH
 *  Author: SCEC
 */ 


#ifndef FONT5X7_H_
#define FONT5X7_H_

#include <avr/pgmspace.h>

const char font1_char_table[] PROGMEM = {
        0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char  
        0x00, 0x60, 0xFA, 0x60, 0x00,            // Code for char !
        0xE0, 0xC0, 0x00, 0xE0, 0xC0,            // Code for char "
        0x24, 0x7E, 0x24, 0x7E, 0x24,            // Code for char #
        0x24, 0xD4, 0x56, 0x48, 0x00,            // Code for char $
        0xC6, 0xC8, 0x10, 0x26, 0xC6,            // Code for char %
        0x6C, 0x92, 0x6A, 0x04, 0x0A,            // Code for char &
        0x00, 0xE0, 0xC0, 0x00, 0x00,            // Code for char '
        0x00, 0x7C, 0x82, 0x00, 0x00,            // Code for char (
        0x00, 0x82, 0x7C, 0x00, 0x00,            // Code for char )
        0x10, 0x7C, 0x38, 0x7C, 0x10,            // Code for char *
        0x10, 0x10, 0x7C, 0x10, 0x10,            // Code for char +
        0x00, 0x07, 0x06, 0x00, 0x00,            // Code for char ,
        0x10, 0x10, 0x10, 0x10, 0x10,            // Code for char -
        0x00, 0x06, 0x06, 0x00, 0x00,            // Code for char .
        0x04, 0x08, 0x10, 0x20, 0x40,            // Code for char /
        0x7C, 0x8A, 0x92, 0xA2, 0x7C,            // Code for char 0
        0x00, 0x42, 0xFE, 0x02, 0x00,            // Code for char 1
        0x46, 0x8A, 0x92, 0x92, 0x62,            // Code for char 2
        0x44, 0x92, 0x92, 0x92, 0x6C,            // Code for char 3
        0x18, 0x28, 0x48, 0xFE, 0x08,            // Code for char 4
        0xF4, 0x92, 0x92, 0x92, 0x8C,            // Code for char 5
        0x3C, 0x52, 0x92, 0x92, 0x0C,            // Code for char 6
        0x80, 0x8E, 0x90, 0xA0, 0xC0,            // Code for char 7
        0x6C, 0x92, 0x92, 0x92, 0x6C,            // Code for char 8
        0x60, 0x92, 0x92, 0x94, 0x78,            // Code for char 9
        0x00, 0x36, 0x36, 0x00, 0x00,            // Code for char :
        0x00, 0x37, 0x36, 0x00, 0x00,            // Code for char ;
        0x10, 0x28, 0x44, 0x82, 0x00,            // Code for char <
        0x24, 0x24, 0x24, 0x24, 0x24,            // Code for char =
        0x00, 0x82, 0x44, 0x28, 0x10,            // Code for char >
        0x40, 0x80, 0x9A, 0x90, 0x60,            // Code for char ?
        0x7C, 0x82, 0xBA, 0xAA, 0x78,            // Code for char @
        0x7E, 0x88, 0x88, 0x88, 0x7E,            // Code for char A
        0xFE, 0x92, 0x92, 0x92, 0x6C,            // Code for char B
        0x7C, 0x82, 0x82, 0x82, 0x44,            // Code for char C
        0xFE, 0x82, 0x82, 0x82, 0x7C,            // Code for char D
        0xFE, 0x92, 0x92, 0x92, 0x82,            // Code for char E
        0xFE, 0x90, 0x90, 0x90, 0x80,            // Code for char F
        0x7C, 0x82, 0x92, 0x92, 0x5E,            // Code for char G
        0xFE, 0x10, 0x10, 0x10, 0xFE,            // Code for char H
        0x00, 0x82, 0xFE, 0x82, 0x00,            // Code for char I
        0x0C, 0x02, 0x02, 0x02, 0xFC,            // Code for char J
        0xFE, 0x10, 0x28, 0x44, 0x82,            // Code for char K
        0xFE, 0x02, 0x02, 0x02, 0x02,            // Code for char L
        0xFE, 0x40, 0x20, 0x40, 0xFE,            // Code for char M
        0xFE, 0x40, 0x20, 0x10, 0xFE,            // Code for char N
        0x7C, 0x82, 0x82, 0x82, 0x7C,            // Code for char O
        0xFE, 0x90, 0x90, 0x90, 0x60,            // Code for char P
        0x7C, 0x82, 0x8A, 0x84, 0x7A,            // Code for char Q
        0xFE, 0x90, 0x90, 0x98, 0x66,            // Code for char R
        0x64, 0x92, 0x92, 0x92, 0x4C,            // Code for char S
        0x80, 0x80, 0xFE, 0x80, 0x80,            // Code for char T
        0xFC, 0x02, 0x02, 0x02, 0xFC,            // Code for char U
        0xF8, 0x04, 0x02, 0x04, 0xF8,            // Code for char V
        0xFC, 0x02, 0x3C, 0x02, 0xFC,            // Code for char W
        0xC6, 0x28, 0x10, 0x28, 0xC6,            // Code for char X
        0xE0, 0x10, 0x0E, 0x10, 0xE0,            // Code for char Y
        0x8E, 0x92, 0xA2, 0xC2, 0x00,            // Code for char Z
        0x00, 0xFE, 0x82, 0x82, 0x00,            // Code for char [
        0x40, 0x20, 0x10, 0x08, 0x04,            // Code for char BackSlash
        0x00, 0x82, 0x82, 0xFE, 0x00,            // Code for char ]
        0x20, 0x40, 0x80, 0x40, 0x20,            // Code for char ^
        0x01, 0x01, 0x01, 0x01, 0x01,            // Code for char _
        0x00, 0xC0, 0xE0, 0x00, 0x00,            // Code for char `
        0x04, 0x2A, 0x2A, 0x2A, 0x1E,            // Code for char a
        0xFE, 0x22, 0x22, 0x22, 0x1C,            // Code for char b
        0x1C, 0x22, 0x22, 0x22, 0x14,            // Code for char c
        0x1C, 0x22, 0x22, 0x22, 0xFE,            // Code for char d
        0x1C, 0x2A, 0x2A, 0x2A, 0x10,            // Code for char e
        0x10, 0x7E, 0x90, 0x90, 0x00,            // Code for char f
        0x18, 0x25, 0x25, 0x25, 0x3E,            // Code for char g
        0xFE, 0x20, 0x20, 0x1E, 0x00,            // Code for char h
        0x00, 0x00, 0xBE, 0x02, 0x00,            // Code for char i
        0x02, 0x01, 0x21, 0xBE, 0x00,            // Code for char j
        0xFE, 0x08, 0x14, 0x22, 0x00,            // Code for char k
        0x00, 0x00, 0xFE, 0x02, 0x00,            // Code for char l
        0x3E, 0x20, 0x18, 0x20, 0x1E,            // Code for char m
        0x3E, 0x20, 0x20, 0x1E, 0x00,            // Code for char n
        0x1C, 0x22, 0x22, 0x22, 0x1C,            // Code for char o
        0x3F, 0x22, 0x22, 0x22, 0x1C,            // Code for char p
        0x1C, 0x22, 0x22, 0x22, 0x3F,            // Code for char q
        0x22, 0x1E, 0x22, 0x20, 0x10,            // Code for char r
        0x10, 0x2A, 0x2A, 0x2A, 0x04,            // Code for char s
        0x20, 0x7C, 0x22, 0x24, 0x00,            // Code for char t
        0x3C, 0x02, 0x04, 0x3E, 0x00,            // Code for char u
        0x38, 0x04, 0x02, 0x04, 0x38,            // Code for char v
        0x3C, 0x06, 0x0C, 0x06, 0x3C,            // Code for char w
        0x36, 0x08, 0x08, 0x36, 0x00,            // Code for char x
        0x39, 0x05, 0x06, 0x3C, 0x00,            // Code for char y
        0x26, 0x2A, 0x2A, 0x32, 0x00,            // Code for char z
        0x10, 0x7C, 0x82, 0x82, 0x00,            // Code for char {
        0x00, 0x00, 0xFF, 0x00, 0x00,            // Code for char |
        0x00, 0x82, 0x82, 0x7C, 0x10,            // Code for char }
        0x40, 0x80, 0x40, 0x80, 0x00             // Code for char ~
};

const char font2_char_table[] PROGMEM =
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char  
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x3F, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char !
        0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,  // Code for char "
        0x40, 0x02, 0xF8, 0x02, 0x40, 0x1F, 0x40, 0x02, 0x40, 0x02, 0xF8, 0x02, 0x40, 0x1F, 0x40, 0x02, 0x00, 0x00,  // Code for char #
        0x00, 0x00, 0x08, 0x0E, 0x08, 0x11, 0xFE, 0x1F, 0x88, 0x7C, 0x98, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char $
        0x08, 0x1C, 0x18, 0x22, 0x60, 0x22, 0xC0, 0x1D, 0x70, 0x03, 0x88, 0x0C, 0x88, 0x18, 0x70, 0x20, 0x00, 0x00,  // Code for char %
        0x70, 0x00, 0x98, 0x1E, 0x88, 0x23, 0x88, 0x21, 0xE8, 0x22, 0x30, 0x1C, 0xF8, 0x00, 0x08, 0x00, 0x00, 0x00,  // Code for char &
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char '
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x07, 0x1C, 0x1C, 0x06, 0x30, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00,  // Code for char (
        0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x06, 0x30, 0x1C, 0x1C, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char )
        0x00, 0x00, 0x00, 0x11, 0x00, 0x0A, 0x00, 0x0A, 0x80, 0x3F, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x11, 0x00, 0x00,  // Code for char *
        0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xF0, 0x07, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,  // Code for char +
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1B, 0x00, 0x1E, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char -
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char .
        0x00, 0x00, 0x02, 0x00, 0x1C, 0x00, 0xF0, 0x00, 0x80, 0x03, 0x00, 0x1E, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,  // Code for char /
        0x00, 0x00, 0xE0, 0x07, 0x50, 0x08, 0x88, 0x10, 0x88, 0x11, 0x08, 0x11, 0x10, 0x0A, 0xE0, 0x07, 0x00, 0x00,  // Code for char 0
        0x00, 0x00, 0x08, 0x04, 0x08, 0x08, 0x08, 0x10, 0xF8, 0x1F, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,  // Code for char 1
        0x00, 0x00, 0x08, 0x08, 0x18, 0x10, 0x28, 0x10, 0x48, 0x10, 0x88, 0x18, 0x08, 0x0F, 0x08, 0x00, 0x00, 0x00,  // Code for char 2
        0x00, 0x00, 0x08, 0x10, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x90, 0x13, 0xE0, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char 3
        0x60, 0x00, 0xE0, 0x00, 0x20, 0x03, 0x20, 0x0E, 0x20, 0x18, 0xF8, 0x1F, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00,  // Code for char 4
        0x00, 0x00, 0x08, 0x1F, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x90, 0x11, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 5
        0x00, 0x00, 0xE0, 0x03, 0x90, 0x0C, 0x08, 0x19, 0x08, 0x11, 0x08, 0x11, 0x90, 0x11, 0xF0, 0x00, 0x00, 0x00,  // Code for char 6
        0x00, 0x00, 0x00, 0x10, 0x08, 0x10, 0x38, 0x10, 0xE0, 0x10, 0x80, 0x13, 0x00, 0x1E, 0x00, 0x18, 0x00, 0x00,  // Code for char 7
        0x00, 0x00, 0x70, 0x0E, 0x98, 0x12, 0x08, 0x11, 0x08, 0x11, 0x88, 0x12, 0x70, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char 8
        0x00, 0x00, 0x00, 0x0F, 0x88, 0x09, 0x88, 0x10, 0x88, 0x10, 0x90, 0x10, 0x30, 0x09, 0xC0, 0x07, 0x00, 0x00,  // Code for char 9
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x03, 0x18, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char :
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1B, 0x03, 0x1E, 0x03, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ;
        0x00, 0x00, 0x80, 0x00, 0xC0, 0x01, 0x60, 0x03, 0x30, 0x06, 0x10, 0x04, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char <
        0x00, 0x00, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x00, 0x00,  // Code for char =
        0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x10, 0x04, 0x30, 0x06, 0x60, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, 0x00,  // Code for char >
        0x00, 0x00, 0x00, 0x00, 0xD8, 0x21, 0x18, 0x21, 0x00, 0x11, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ?
        0xFC, 0x01, 0x06, 0x0E, 0xE1, 0x18, 0x11, 0x33, 0x11, 0x22, 0x31, 0x22, 0xF0, 0x23, 0x10, 0x10, 0xE0, 0x0F,  // Code for char @
        0x00, 0x00, 0x38, 0x00, 0xE0, 0x01, 0x20, 0x1E, 0x20, 0x10, 0x20, 0x1E, 0xE0, 0x03, 0x38, 0x00, 0x00, 0x00,  // Code for char A
        0x00, 0x00, 0xF8, 0x1F, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x98, 0x13, 0xF0, 0x0E, 0x00, 0x00, 0x00, 0x00,  // Code for char B
        0x00, 0x00, 0xE0, 0x03, 0x30, 0x0C, 0x08, 0x18, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x10, 0x08, 0x00, 0x00,  // Code for char C
        0x00, 0x00, 0xF8, 0x1F, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x18, 0x10, 0x30, 0x08, 0xE0, 0x07, 0x00, 0x00,  // Code for char D
        0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x08, 0x11, 0x00, 0x00,  // Code for char E
        0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x00, 0x00,  // Code for char F
        0x00, 0x00, 0xE0, 0x03, 0x30, 0x0C, 0x18, 0x18, 0x08, 0x10, 0x88, 0x10, 0x88, 0x10, 0xF8, 0x08, 0x00, 0x00,  // Code for char G
		0x00, 0x00, 0xF8, 0x1F, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0xF8, 0x1F, 0x00, 0x00,  // Code for char H
        0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x08, 0x10, 0xF8, 0x1F, 0x08, 0x10, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00,  // Code for char I
        0x00, 0x00, 0x10, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0xF0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char J
        0x00, 0x00, 0xF8, 0x1F, 0x80, 0x01, 0xC0, 0x03, 0x60, 0x06, 0x30, 0x0C, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00,  // Code for char K
        0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,  // Code for char L
        0xF8, 0x1F, 0xF8, 0x1F, 0x00, 0x0E, 0x80, 0x03, 0x40, 0x00, 0x80, 0x03, 0x00, 0x0C, 0xF8, 0x1F, 0xF8, 0x1F,  // Code for char M
        0x00, 0x00, 0xF8, 0x1F, 0x00, 0x18, 0x00, 0x06, 0x80, 0x01, 0x60, 0x00, 0x18, 0x00, 0xF8, 0x1F, 0x00, 0x00,  // Code for char N
        0xE0, 0x07, 0x10, 0x0C, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x30, 0x08, 0xE0, 0x07, 0x00, 0x00,  // Code for char O
        0x00, 0x00, 0xF8, 0x1F, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x00, 0x19, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,  // Code for char P
        0xE0, 0x07, 0x10, 0x08, 0x08, 0x10, 0x0E, 0x10, 0x09, 0x10, 0x11, 0x08, 0xE3, 0x07, 0x00, 0x00, 0x00, 0x00,  // Code for char Q
		0x00, 0x00, 0xF8, 0x1F, 0x00, 0x11, 0x00, 0x11, 0x80, 0x11, 0x60, 0x12, 0x18, 0x0E, 0x08, 0x00, 0x00, 0x00,  // Code for char R
        0x00, 0x00, 0x08, 0x0E, 0x08, 0x11, 0x08, 0x11, 0x88, 0x11, 0x88, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char S
        0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0xF8, 0x1F, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00,  // Code for char T
        0x00, 0x00, 0xF0, 0x1F, 0x18, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x10, 0x00, 0xE0, 0x1F, 0x00, 0x00,  // Code for char U
        0x00, 0x00, 0x00, 0x1C, 0xC0, 0x07, 0xF8, 0x00, 0x08, 0x00, 0xF8, 0x00, 0x80, 0x07, 0x00, 0x1C, 0x00, 0x00,  // Code for char V
        0xF8, 0x1F, 0x18, 0x00, 0x70, 0x00, 0xC0, 0x01, 0x00, 0x03, 0xE0, 0x00, 0x18, 0x00, 0xF8, 0x1F, 0x00, 0x00,  // Code for char W
        0x08, 0x10, 0x18, 0x18, 0x70, 0x0C, 0xC0, 0x07, 0x80, 0x03, 0xE0, 0x0E, 0x30, 0x18, 0x18, 0x10, 0x08, 0x00,  // Code for char X
		0x00, 0x10, 0x00, 0x08, 0x00, 0x06, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x08, 0x00, 0x10,  // Code for char Y
        0x00, 0x00, 0x08, 0x10, 0x38, 0x10, 0x68, 0x10, 0x88, 0x11, 0x08, 0x16, 0x08, 0x1C, 0x08, 0x10, 0x00, 0x00,  // Code for char Z
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00,  // Code for char [
        0x00, 0x00, 0x00, 0x20, 0x00, 0x1C, 0x80, 0x07, 0xE0, 0x00, 0x3C, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char BackSlash
        0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x18, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00,  // Code for char ^
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,  // Code for char _
        0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char `
        0x00, 0x00, 0x30, 0x00, 0x48, 0x01, 0x48, 0x02, 0x48, 0x02, 0x50, 0x02, 0xF8, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char a
        0x00, 0x00, 0xF8, 0x3F, 0x08, 0x01, 0x08, 0x02, 0x08, 0x02, 0x10, 0x03, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char b
        0x00, 0x00, 0xF0, 0x00, 0x10, 0x01, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char c
        0x00, 0x00, 0xF0, 0x00, 0x18, 0x01, 0x08, 0x02, 0x08, 0x02, 0x10, 0x02, 0xF8, 0x3F, 0x00, 0x00, 0x00, 0x00,  // Code for char d
        0x00, 0x00, 0xF0, 0x00, 0x50, 0x01, 0x48, 0x02, 0x48, 0x02, 0x48, 0x02, 0x48, 0x03, 0xC8, 0x01, 0x00, 0x00,  // Code for char e
        0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0xF8, 0x1F, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x20, 0x00, 0x00,  // Code for char f
        0x00, 0x00, 0xFE, 0x01, 0x29, 0x02, 0x29, 0x02, 0x29, 0x02, 0x29, 0x02, 0xC9, 0x03, 0x06, 0x02, 0x00, 0x00,  // Code for char g
        0x00, 0x00, 0xF8, 0x3F, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0xF8, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char h
        0x00, 0x00, 0x08, 0x02, 0x08, 0x32, 0xF8, 0x33, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char i
        0x00, 0x00, 0x02, 0x02, 0x01, 0x02, 0x01, 0x02, 0x03, 0x32, 0xFE, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char j
        0x00, 0x00, 0xF8, 0x3F, 0x80, 0x00, 0xC0, 0x00, 0x20, 0x01, 0x10, 0x02, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char k
        0x00, 0x00, 0x08, 0x20, 0x08, 0x20, 0xF8, 0x3F, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char l
        0x00, 0x00, 0xF8, 0x03, 0x00, 0x03, 0x00, 0x02, 0xF8, 0x03, 0x00, 0x03, 0x00, 0x02, 0xF8, 0x03, 0x00, 0x00,  // Code for char m
        0x00, 0x00, 0xF8, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0xF8, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char n
        0x00, 0x00, 0xF0, 0x00, 0x18, 0x01, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x10, 0x03, 0xE0, 0x01, 0x00, 0x00,  // Code for char o
        0x00, 0x00, 0xFF, 0x03, 0x08, 0x01, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x10, 0x03, 0xE0, 0x01, 0x00, 0x00,  // Code for char p
        0x00, 0x00, 0xF0, 0x00, 0x18, 0x01, 0x08, 0x02, 0x08, 0x02, 0x10, 0x02, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00,  // Code for char q
        0x00, 0x00, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x80, 0x01, 0x00, 0x00,  // Code for char r
        0x00, 0x00, 0x88, 0x01, 0xC8, 0x02, 0x48, 0x02, 0x48, 0x02, 0x48, 0x02, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char s
        0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0xF0, 0x1F, 0x18, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x00, 0x00,  // Code for char t
        0x00, 0x00, 0xF0, 0x03, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x10, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00,  // Code for char u
        0x00, 0x00, 0x00, 0x03, 0xC0, 0x01, 0x38, 0x00, 0x08, 0x00, 0x30, 0x00, 0xC0, 0x01, 0x00, 0x03, 0x00, 0x00,  // Code for char v
        0x00, 0x00, 0xF8, 0x03, 0x08, 0x00, 0x70, 0x00, 0x80, 0x00, 0x70, 0x00, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00,  // Code for char w
        0x00, 0x00, 0x08, 0x02, 0x18, 0x03, 0xB0, 0x01, 0xC0, 0x00, 0xE0, 0x01, 0x30, 0x03, 0x18, 0x02, 0x08, 0x00,  // Code for char x
        0x01, 0x00, 0x01, 0x03, 0xC1, 0x01, 0x3E, 0x00, 0x0C, 0x00, 0x70, 0x00, 0xC0, 0x01, 0x00, 0x03, 0x00, 0x00,  // Code for char y
        0x00, 0x00, 0x08, 0x02, 0x18, 0x02, 0x68, 0x02, 0xC8, 0x02, 0x08, 0x03, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char z
        0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x7E, 0x1F, 0x03, 0x20, 0x01, 0x20, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00,  // Code for char {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char |
        0x00, 0x00, 0x01, 0x20, 0x01, 0x20, 0x03, 0x30, 0x7E, 0x1F, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char }
        0x00, 0x00, 0xC0, 0x00, 0x00, 0x01, 0x00, 0x01, 0x80, 0x01, 0xC0, 0x00, 0x40, 0x00, 0x40, 0x00, 0x80, 0x01   // Code for char ~
};

#endif /* FONT5X7_H_ */