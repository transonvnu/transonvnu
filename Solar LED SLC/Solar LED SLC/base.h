/*
 * base.h
 *
 * Created: 25/11/2013 14:12:19 PM
 *  Author: Administrator
 */ 


#ifndef BASE_H_
#define BASE_H_

#define cbi(byt, bit)   (byt &= ~_BV(bit))
#define sbi(byt, bit)   (byt |= _BV(bit))
#define chb(byt, bit)   (byt & _BV(bit))
#define inb(byt, bit)   (byt ^= _BV(bit))

#define BYTETOBINARY(byte)  \
(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0)

#define hi8(wrd) (wrd >> 8)
#define lo8(wrd) (wrd & 0xff)

#define bsd2as_hi(bsd)	( ((bsd & 0xf0) >> 4) + 0x30)
#define bsd2as_lo(bsd)	( (bsd & 0x0f) + 0x30)
char toUpper(char character);
void int2char(unsigned int *src, unsigned char *des, unsigned char len);



#endif /* BASE_H_ */