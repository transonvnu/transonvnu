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
char isHexCharacter(char character){
	if ((character >= '0') && (character <= '9')) return 1;
	if ((character >= 'a') && (character <= 'f')) return 1;
	if ((character >= 'A') && (character <= 'F')) return 1;
	return 0;			
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
}
//convert two character Hexa to Decimal Value	
unsigned char parseHexString(char s1, char s2){
	unsigned char n1=0,n2=0;
	if(s1<='9') n1 = s1 - '0';
	else{
		switch (toUpper(s1)){
			case 'A':
				n1 = 10;
				break;
			case 'B':
				n1 = 11;
				break;
			case 'C':
				n1 = 12;
				break;
			case 'D':
				n1 = 13;
				break;
			case 'E':
				n1 = 14;
				break;
			case 'F':
				n1 = 15;
				break;
			default : n1 = 0;
		}
	}
	if(s2<='9') n2 = s2 - '0';
	else{
		switch (toUpper(s2)){
			case 'A':
				n2 = 10;
				break;
			case 'B':
				n2 = 11;
				break;
			case 'C':
				n2 = 12;
				break;
			case 'D':
				n2 = 13;
				break;
			case 'E':
				n2 = 14;
				break;
			case 'F':
				n2 = 15;
				break;
			default : n2 = 0;
		}
	}	
	return (n1*16 + n2);
}
//------------------------------------------------
//a number one byte to Decimal string
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
}	
void timeToString(char data, char* str){
	str[1] = (data % 10) + '0';
	str[0] = ((data/10)%10) + '0';
	str[2] = 0;
}
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
//a number one byte to Hexa string
void byteToHexString(char  data, char * str){
	char hi,lo;
	hi = (data >> 4)&0x0F;
	lo = data&0x0F;
	if(hi<=9) hi = hi + '0';
	else{
		switch (hi){
			case 10:
				hi = 'A';
				break;
			case 11:
				hi = 'B';
				break;
			case 12:
				hi = 'C';
				break;
			case 13:
				hi = 'D';
				break;
			case 14:
				hi = 'E';
				break;
			case 15:
				hi = 'F';
				break;
		}
	}
	if(lo<=9) lo = lo + '0';
	else{
		switch (lo){
			case 10:
				lo = 'A';
				break;
			case 11:
				lo = 'B';
				break;
			case 12:
				lo = 'C';
				break;
			case 13:
				lo = 'D';
				break;
			case 14:
				lo = 'E';
				break;
			case 15:
				lo = 'F';
				break;
		}
	}
	str[0] = hi;
	str[1] = lo;
	str[2] = 0;
}
//a number two bytes to a Hexa string
void wordToHexString(unsigned int  data, char * str){
	char hi,lo,tempStr[3];
	hi = (data >> 8) & 0x00FF;
	lo =  data & 0x00FF;
	byteToHexString(hi,tempStr);
	str[0] = tempStr[0];
	str[1] = tempStr[1];
	byteToHexString(lo,tempStr);
	str[2] = tempStr[0];
	str[3] = tempStr[1];
	str[4] = 0;
}
//a number three bytes to a Hexa string
void threeByteToHexString(unsigned long data, char * str){
	char hi,no,lo,tempStr[3];
	hi = (data >> 16) & 0x00FF;
	no = (data >> 8) & 0x00FF;
	lo =  data & 0x00FF;
	byteToHexString(hi,tempStr);
	str[0] = tempStr[0];
	str[1] = tempStr[1];
	byteToHexString(no,tempStr);
	str[2] = tempStr[0];
	str[3] = tempStr[1];
	byteToHexString(lo,tempStr);
	str[4] = tempStr[0];
	str[5] = tempStr[1];
	str[6] = 0;
}
//number long to a Hexa string
void dwordToHexString(unsigned long  data, char * str){
	unsigned int hi,lo;
	char tempStr[4];
	hi = (data >> 16) & 0x0000FFFF;
	lo =  data & 0x0000FFFF;
	wordToHexString(hi,tempStr);
	str[0] = tempStr[0];
	str[1] = tempStr[1];	
	str[2] = tempStr[2];
	str[3] = tempStr[3];
	wordToHexString(lo,tempStr);
	str[4] = tempStr[0];
	str[5] = tempStr[1];	
	str[6] = tempStr[2];
	str[7] = tempStr[3];
	str[8] = 0;
}
//Function convert Data for transmitted file
void convertData(char * sourStr, char * desStr){
	char oneByte;
	unsigned int oneWord;
	unsigned long threeByte;
//	unsigned long dWord;
	oneByte = (sourStr[0] - '0')*10 + sourStr[1] - '0';
	byteToHexString(oneByte,desStr);   //Gio
	oneByte = (sourStr[3] - '0')*10 + sourStr[4] - '0';
	byteToHexString(oneByte,desStr+2); //Phut
	oneByte = (sourStr[6] - '0')*10 + sourStr[7] - '0';
	byteToHexString(oneByte,desStr+4); //Giay
	oneByte = sourStr[9] - '0';						   
	byteToHexString(oneByte,desStr+6); //On/Off
	oneWord = (unsigned int)(sourStr[11] - '0')*10000 + (unsigned int)(sourStr[12] - '0')*1000 + (unsigned int)(sourStr[13] - '0')*100 + (unsigned int)(sourStr[14] - '0')*10 + (unsigned int)(sourStr[15] - '0');
	wordToHexString(oneWord,desStr+8); //U1
	oneWord = (unsigned int)(sourStr[17] - '0')*10000 + (unsigned int)(sourStr[18] - '0')*1000 + (unsigned int)(sourStr[19] - '0')*100 + (unsigned int)(sourStr[20] - '0')*10 + (unsigned int)(sourStr[21] - '0');
	wordToHexString(oneWord,desStr+12);//U2
	oneWord = (unsigned int)(sourStr[23] - '0')*10000 + (unsigned int)(sourStr[24] - '0')*1000 + (unsigned int)(sourStr[25] - '0')*100 + (unsigned int)(sourStr[26] - '0')*10 + (unsigned int)(sourStr[27] - '0');
	wordToHexString(oneWord,desStr+16);//U3
	oneWord = (unsigned int)(sourStr[29] - '0')*10000 + (unsigned int)(sourStr[30] - '0')*1000 + (unsigned int)(sourStr[31] - '0')*100 + (unsigned int)(sourStr[32] - '0')*10 + (unsigned int)(sourStr[33] - '0');
	wordToHexString(oneWord,desStr+20);//I1
	oneWord = (unsigned int)(sourStr[35] - '0')*10000 + (unsigned int)(sourStr[36] - '0')*1000 + (unsigned int)(sourStr[37] - '0')*100 + (unsigned int)(sourStr[38] - '0')*10 + (unsigned int)(sourStr[39] - '0');
	wordToHexString(oneWord,desStr+24);//I2
	oneWord = (unsigned int)(sourStr[41] - '0')*10000 + (unsigned int)(sourStr[42] - '0')*1000 + (unsigned int)(sourStr[43] - '0')*100 + (unsigned int)(sourStr[44] - '0')*10 + (unsigned int)(sourStr[45] - '0');
	wordToHexString(oneWord,desStr+28);//I3
	oneByte = (sourStr[47] - '0')*100 + (sourStr[48] - '0')*10 + (sourStr[49] - '0');
	byteToHexString(oneByte,desStr+32);//Pha1
	oneByte = (sourStr[51] - '0')*100 + (sourStr[52] - '0')*10 + (sourStr[53] - '0');
	byteToHexString(oneByte,desStr+34);//Pha2
	oneByte = (sourStr[55] - '0')*100 + (sourStr[56] - '0')*10 + (sourStr[57] - '0');
	byteToHexString(oneByte,desStr+36);//Pha3
	threeByte = (long)(sourStr[59] - '0')*10000000 + (long)(sourStr[60] - '0')*1000000 + (long)(sourStr[61] - '0')*100000 + (long)(sourStr[62] - '0')*10000 
			  + (long)(sourStr[63] - '0')*1000 + (long)(sourStr[64] - '0')*100 + (long)(sourStr[65] - '0')*10 + (long)(sourStr[66] - '0');
	threeByteToHexString(threeByte,desStr+38);//E1
	threeByte = (long)(sourStr[68] - '0')*10000000 + (long)(sourStr[69] - '0')*1000000 + (long)(sourStr[70] - '0')*100000 + (long)(sourStr[71] - '0')*10000
			  + (long)(sourStr[72] - '0')*1000 + (long)(sourStr[73] - '0')*100 + (long)(sourStr[74] - '0')*10 + (long)(sourStr[75] - '0');
	threeByteToHexString(threeByte,desStr+44);//E2
	threeByte = (long)(sourStr[77] - '0')*10000000 + (long)(sourStr[78] - '0')*1000000 + (long)(sourStr[79] - '0')*100000 + (long)(sourStr[80] - '0')*10000
			  + (long)(sourStr[81] - '0')*1000 + (long)(sourStr[82] - '0')*100 + (long)(sourStr[83] - '0')*10 + (long)(sourStr[84] - '0');
	threeByteToHexString(threeByte,desStr+50);//E3
	desStr[56] = 0;
}
void convertData2(char * sourStr, char * desStr){
	char oneByte;
	oneByte = (sourStr[0] - '0')*10 + sourStr[1] - '0';
	byteToHexString(oneByte,desStr);   //Gio
	oneByte = (sourStr[3] - '0')*10 + sourStr[4] - '0';
	byteToHexString(oneByte,desStr+2); //Phut
	oneByte = (sourStr[6] - '0')*10 + sourStr[7] - '0';
	byteToHexString(oneByte,desStr+4); //Giay
	oneByte = (sourStr[9] - '0')*10 + sourStr[10] - '0';					   
	byteToHexString(oneByte,desStr+6); //Ngay
	oneByte = (sourStr[12] - '0');
	byteToHexString(oneByte,desStr+8);//SystemReset
	oneByte = (sourStr[14] - '0');
	byteToHexString(oneByte,desStr+10);//ModemReset
	oneByte = (sourStr[16] - '0');
	byteToHexString(oneByte,desStr+12);//On/Off
	oneByte = (sourStr[18] - '0');
	byteToHexString(oneByte,desStr+14);//ChangeSchedule
	oneByte = (sourStr[20] - '0');
	byteToHexString(oneByte,desStr+16);//ChangeTime
	oneByte = (sourStr[22] - '0');
	byteToHexString(oneByte,desStr+18);//Setting
	oneByte = (sourStr[24] - '0');
	byteToHexString(oneByte,desStr+20);//ResetEnergy
	desStr[22] = 0;
}
//number long to character array
void l_to_a(unsigned long number,char * str){
	char i;
	for(i=0;i<8;i++){
		str[7-i] = (number%10) + '0';
		number /= 10;
	}
	str[8] = 0;
}


