/*
string.h
Library for string and character manipulation
Date created: 24/07/2008
*/

#ifndef _STRING1_INCLUDED_
#define _STRING1_INCLUDED_

char toUpper(char character);
char isDigit(char character);
char isSpace(char character);
char isHexCharacter(char character);
char parseByte(char * str);
unsigned int parseWord(char * str);
unsigned long parseDword(char * str);
unsigned char parseHexString(char s1, char s2);
void byteToString(unsigned char data, char* str);
void timeToString(char data, char* str);
void numberToString(long data, char * str);
void floatToString(long data, char * str);
void wordToString(unsigned int data, char * str);
void threeByteToHexString(unsigned long data,char * str);
void dwordToString(unsigned long data, char * str);
void byteToHexString(char  data, char * str);
void wordToHexString(unsigned int  data, char * str);
void dwordToHexString(unsigned long  data, char * str);
void convertData(char * sourStr, char * desStr);
void convertData2(char * sourStr, char * desStr);
void l_to_a(unsigned long number, char * str);

#endif 
