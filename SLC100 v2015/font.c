#include "font.h"
#include "t6963c.h"
#include <string.h>

unsigned char pow2(unsigned char b){
	switch(b){
		case 0: return 1;break;
		case 1: return 2;break;
		case 2: return 4;break;
		case 3: return 8;break;
		case 4: return 16;break;
		case 5: return 32;break;
		case 6: return 64;break;
		case 7: return 128;break;
	}
	return 0;
}    

void lcd_print3(unsigned int x,unsigned int y, char*in,FONT_DEF * strcut1,unsigned char par,unsigned char par1){
     //FONT_DEF * strcut1;
	 register unsigned int offset,width;
     register unsigned char map,height;
	 register int i,j,k;
     unsigned char f_offset,data = 0,m;
	 int widthptr,mapptr,glyphptr, offsetptr;
     unsigned int adr;
	 //strcut1 = &f3;
     //char sss[40];
	 x=XMAX-1-x;
     f_offset = x%8;
     height = pgm_read_byte(&(strcut1->glyph_height)); //
	 y=YMAX-height-y;
	 widthptr = pgm_read_word(&(strcut1->width_table));
	 mapptr = pgm_read_word(&(strcut1->mapping_table));
	 offsetptr = pgm_read_word(&(strcut1->offset_table));
	 glyphptr = pgm_read_word(&(strcut1->glyph_table));
     for(j = par ; j < height - par1 ; j++ )
	 { // for height
          m = f_offset;      
          adr = G_BASE + ((y+ height - par1 - 1 - j)*BYTES_PER_ROW) + (x/8);
          //lcd_set_address(adr);
          //cput(0xB0);
          for(k=0;in[k]!=0;k++)
		  {  // for charater in string in[k]!=0
          	map = pgm_read_byte(mapptr + in[k]);
          	if(pgm_read_byte(&(strcut1->glyph_width)) ==0)
				width  = pgm_read_byte(widthptr + map);
          	else 
				width  = pgm_read_byte(&(strcut1->glyph_width));
          	offset = pgm_read_word(offsetptr + map*2) + j*(1+(width-1)/8);  // offset + (do rong byte cua chu)*so dong dang ve
			//offset = font1_offset_table[map] + j*(1+(width-1)/8);
         	for(i=0;i<width;i++)
			{   //  width
 				if( pgm_read_byte(glyphptr + offset + (i/8)) & pow2(7-(i%8))) data |= 1<<(7-m);
	     		if(m==0)
				{
					lcd_set_address(adr);
					adr--;
					dput(data);
					cput(0xC4);
					data = 0;
                    m = 8;
	     		}
	     		m--;
			}//End i 
	     }// End K
	     if (m<7) 
		 {
		 	lcd_set_address(adr);
		 	adr--;
			dput(data);
			cput(0xC4);
			data = 0;
		 }
	     //cput(0xC4);
     }// End j
}

void lcd_print4(unsigned int x,unsigned int y, char*in,FONT_DEF * strcut1,unsigned char par,unsigned char par1){
     //FONT_DEF * strcut1;
	 register unsigned int offset,width;
     register unsigned char i,j,map,height,k;
     unsigned char f_offset,data = 0xFF,m;
	 int widthptr,mapptr,glyphptr, offsetptr;
     unsigned int adr;
	 //strcut1 = &f3;
     //char sss[40]; 
	 x=XMAX-1-x;
     f_offset = x%8;
     height = pgm_read_byte(&(strcut1->glyph_height)); //
	 y=YMAX-height-y;
	 widthptr = pgm_read_word(&(strcut1->width_table));
	 mapptr = pgm_read_word(&(strcut1->mapping_table));
	 offsetptr = pgm_read_word(&(strcut1->offset_table));
	 glyphptr = pgm_read_word(&(strcut1->glyph_table));
     for(j = par ; j< (height - par1); j++ ){ // for height
          m = f_offset;      
          adr = G_BASE + ((y+ height - par1 - 1 - j)*BYTES_PER_ROW)  + (x/8);
          //lcd_set_address(adr);
          //cput(0xB0);
          for(k=0 ; in[k]!=0 ; k++){  // for charater in string
          	map = pgm_read_byte(mapptr + in[k]);
          	if(pgm_read_byte(&(strcut1->glyph_width)) ==0)	
				width  = pgm_read_byte(widthptr + map);
          	else
				width  = pgm_read_byte(&(strcut1->glyph_width));
          	offset = pgm_read_word(offsetptr + map*2) + j*(1+(width-1)/8);  // offset + (do rong byte cua chu)*so dong dang ve
			//offset = font1_offset_table[map] + j*(1+(width-1)/8);
         		for(i=0; i<width;i++){   //  width
     			if( pgm_read_byte(glyphptr + offset + (i/8)) & pow2( 7 - ( i % 8 ) )) data &= ~(1<<(7-m));
		     	if(m==0){
						lcd_set_address(adr);
						adr--;
	                    dput(data);
						cput(0xC4);
                         data = 0xFF;
                         m = 8;
		     	}
		     	m--;
			}//End i 
	     }// End K
	     if (m<7) 
		 {
		 	lcd_set_address(adr);
		 	adr--;
			dput(data);
			cput(0xC4); 
			data = 0xFF;
		}
	     //cput(0xB2);
     }// End j
}
char bitPosReverse(char data)
{
	char temp = 0,i;
	for (i=0; i<7; i++)
	{
		temp |= (data&0x01);
		temp <<= 1;
		data >>= 1;
	}
	temp |= (data&0x01);
	return temp;
}
//---------------------------------------------------------------------------------
void draw(const char * s,int start_column,int start_line){
unsigned int addr;
int i,j;
start_column=XMAX-1-start_column-2;
start_line=YMAX-1-start_line-29;
addr =  G_BASE + start_line*BYTES_PER_ROW+start_column;
//lcd_set_address(addr);   // set LCD addr. pointer
for(j=23;j>=0;j--){   // theo 
     lcd_set_address(addr);
     cput(0xB0);
     for(i=2;i>=0;i--)
          dput(bitPosReverse(pgm_read_byte(s+i+j*3)));
     cput(0xB2);
     addr += BYTES_PER_ROW;
     }
}

/*
void test(FONT_DEF * strcut1){
	char str3[30];
	register unsigned char map,height;
	height = pgm_read_byte(&(strcut1->glyph_height));
	map = pgm_read_byte(strcut1->mapping_table);
	sprintf(str3,"%4u",pgm_read_word(pgm_read_word(&(strcut1->offset_table)) + 12));
	lcd_print_ram(0,0,str3);
} */
