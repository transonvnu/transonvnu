/*
 * base.c
 *
 * Created: 22/09/2013 9:19:07 CH
 *  Author: SCEC
 */ 
char toUpper(char character){
	if ((character >= 'a') && (character <= 'z')) return (character - 'a' + 'A');
	else return character;
}

void int2char(unsigned int *src, unsigned char *des, unsigned char len){
	unsigned char i, _tmp;
	for(i=0;i<len;i++){
		_tmp = (unsigned char)src[i];
		des[i] = _tmp;
	}
}