
#ifndef _FONT_H_
#define _FONT_H_ 

#include <avr/pgmspace.h>

typedef const struct{
   const unsigned char glyph_width;                 /* glyph width in pixels              */
                                             		/* 0 for variable width fonts     	  */
   const unsigned char glyph_height;                /* glyph height for storage           */
   const unsigned char par;
   const char *glyph_table;         /* font table start address in memory */
   const char *mapping_table;       /* used to find the index of a glyph  */
   const char *width_table;         /* width table start adress           */
                                    /*        NULL for fixed width fonts  */
   const int *offset_table;         /* ks the offsets of the first byte   */
                                    /*                      of each glyph */
}FONT_DEF;

typedef const struct IMG_DEF {
   const unsigned char width_in_pixels;     /* Image width */
   const unsigned char height_in_pixels;    /* Image height*/
   const unsigned char *char_table;         /* Image table start address in memory  */
}IMG_DEF;

//unsigned char pow2(unsigned char b);
//void lcd_print2(unsigned int x,unsigned int y, char*in, FONT_DEF * strcut1,unsigned char par);
void lcd_print3(unsigned int x,unsigned int y, char*in,FONT_DEF * strcut1,unsigned char par,unsigned char par1);
void lcd_print4(unsigned int x,unsigned int y, char*in,FONT_DEF * strcut1,unsigned char par,unsigned char par1);
void test(FONT_DEF * strcut1);
void draw(const char * s,int start_column,int start_line);
#endif
