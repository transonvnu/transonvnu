/*
string.c
Implementation for string and character manipulation
Date created: 24/07/2008
*/

#include "string1.h"

char toUpper(char character){
	if ((character >= 'a') && (character <= 'z')) return (character - 'a' + 'A');
	else return character;
}	
/*
//------------------------------------------------
char isDigit(char character){
	if ((character >= '0') && (character <= '9')) return 1;
	else return 0;
}	

//------------------------------------------------
char isSpace(char character){
	if ((character == ' ') || (character == '\t') || (character == '\n') || (character == '\r')) return 1;
	else return 0;
}	
//------------------------------------------------
char parseByte(char* str){
	unsigned char i = 0;
	char byteValue = 0;
	while (str[i] != 0){
		if (!isDigit(str[i])) return 0;
		byteValue = byteValue * 10 + (str[i] - '0');
		i++;
	}
	return byteValue;
}	
//------------------------------------------------
unsigned int parseWord(char* str){
	unsigned char i = 0;
	unsigned int wordValue = 0;
	while (str[i] != 0){
		if (!isDigit(str[i])) return 0;
		wordValue = wordValue * 10 + (str[i] - '0');
		i++;
	}
	return wordValue;
}	
//------------------------------------------------
//convert string to Decimal Value
unsigned long parseDword(char* str){
	unsigned char i = 0;
	unsigned long dwordValue = 0;
	while (str[i] != 0){
		if (!isDigit(str[i])) return 0;
		dwordValue = dwordValue * 10 + (str[i] - '0');
		i++;
	}
	return dwordValue;
}*/
//------------------------------------------------
//a number one byte to Decimal string
/*
void byteToString(unsigned char data, char* str){
	char tempBuf[3];
	unsigned char i, j;
	i = 0;
	do {
		tempBuf[i] = (data % 10) + '0';
		data /= 10;
		i++;
	}
	while (data != 0);
	
	j = 0;
	while (i>0){
		str[j] = tempBuf[i - 1];
		i--;
		j++;
	}
	str[j] = 0;
}*/
/*	
void numberToString(long data, char str[10]){
	char tempBuf[8];
	unsigned char i, j;
	j = 0;
	if(data < 0){
		data = -1*data;
		str[0] = '-';
		j++;
	}
	i = 0;
	do {
		tempBuf[i] = (data % 10) + '0';
		data /= 10;
		i++;
	}
	while (data != 0);
	while (i>0){
		str[j] = tempBuf[i - 1];
		i--;
		j++;
	}
	str[j] = 0;
}	
void floatToString(long data, char * str){
	char tempBuf[9];
	unsigned char i, j;
	long N; //Phan nguyen
	unsigned char T; //Phan thap phan
	j = 0;
	if(data < 0){
		data = -1*data;
		str[0] = '-';
		j++;
	}
	N = data/100;
	T = (data%100);
	i = 0;
	do {
		tempBuf[i] = (N % 10) + '0';
		N /= 10;
		i++;
	}
	while (N != 0);
	while (i>0){
		str[j] = tempBuf[i - 1];
		i--;
		j++;
	}
	str[j] = '.';
	str[j+1] = (T/10) + '0';
	str[j+2] = (T%10) + '0';
	str[j+3] = 0;
}	
//------------------------------------------------
//a number two byte to Decimal string
void wordToString(unsigned int data, char* str){
	char tempBuf[5];
	unsigned char i = 0, j = 0;
	do {
		tempBuf[i] = (data % 10) + '0';
		data /= 10;
		i++;
	}
	while (data != 0);
	
	while (i>0){
		str[j] = tempBuf[i - 1];
		i--;
		j++;
	}
	str[j] = 0;
}	
//------------------------------------------------
//a number four byte to Decimal string
void dwordToString(unsigned long data, char* str){
	char tempBuf[11];
	unsigned char i = 0, j = 0;
	do {
		tempBuf[i] = (data % 10) + '0';
		data /= 10;
		i++;
	}
	while (data != 0);
	
	while (i>0){
		str[j] = tempBuf[i - 1];
		i--;
		j++;
	}
	str[j] = 0;
}	
//-------------------------------------------------
//Function convert Data for transmitted file

*/


