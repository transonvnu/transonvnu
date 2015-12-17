
#include "t6963c.h"
#include "timming.h"

/* ==============================================================
 * Low-level I/O routines to interface to LCD display
 * based on four routines:
 *
 *          dput(): write data byte
 *          cput(): write control byte
 *          dget(): read data byte         (UNTESTED)
 *          sget(): read status
 * ==============================================================
 */

// -------------------------------------------------------------------------------

int sget(void)  // get LCD display status byte
{
int lcd_status;

  DATAIN;       // make 8-bit parallel port an input
  CDHI;         // bring LCD C/D line high (read status byte)
  RDLO;         // bring LCD /RD line low (read active)
  CELO;         // bring LCD /CE line low (chip-enable active)
  asm ("nop");
  asm ("nop");
  lcd_status = DATAPIN;      // read LCD status byte
  CEHI;         // bring LCD /CE line high, disabling it
  RDHI;         // deactivate LCD read mode
  DATAOUT; // make 8-bit parallel port an output port

  return(lcd_status);
} // sget()
//----------------------------------------------------------------------------------------
/*
void cput(int byte) // write command byte to LCD module
		    // assumes port is in data OUTPUT mode
{
  do {} while ((0x03 & sget()) != 0x03); // wait until display ready

  DATAPORT=byte;  // present data to LCD on PC's port pins

  CDHI;         // control/status mode
  RDHI;         // make sure LCD read mode is off
  WRLO;         // activate LCD write mode
  CELO;         // pulse ChipEnable LOW, > 80 ns, enables LCD I/O
  CEHI;         // disable LCD I/O
  WRHI;         // deactivate write mode

} // cput()
// -------------------------------------------------------------------------------

int dget(void)      // get data byte from LCD module
{
int lcd_byte;

  do {} while ((0x03 & sget()) != 0x03); // wait until display ready
  DATAIN; // make PC's port an input port
  WRHI;   // make sure WRITE mode is inactive
  CDLO;   // data mode
  RDLO;   // activate READ mode
  CELO;   // enable chip, which outputs data
  lcd_byte = DATAPIN;  // read data from LCD
  CEHI;   // disable chip
  RDHI;   // turn off READ mode
  DATAOUT; // make 8-bit parallel port an output port

  return(lcd_byte);
} // dget()

//----------------------------------------------------------------------------------------

void dput(int byte) // write data byte to LCD module over par. port
		    // assume PC port in data OUTPUT mode
{
  do {} while ((0x03 & sget()) != 0x03); // wait until display ready
  CDLO;
  WRLO;         // activate LCD's write mode
  DATAPORT=byte;          // write value to data port
  CELO;                       // pulse enable LOW > 80 ns (hah!)
  //#asm("nop");
  //#asm("nop");
  CEHI;                       // return enable HIGH
  WRHI;                       // restore Write mode to inactive

 // using my P5/75 MHz PC with ISA bus, CE stays low for 2 microseconds

} // end dput()*/

void cput(int byte) // write command byte to LCD module
		    // assumes port is in data OUTPUT mode
{
  unsigned char i = 0;
  while ((0x03 & sget()) != 0x03){
  	if(i > 5) return;
	i++;
  } // wait until display ready

  DATAPORT=byte;  // present data to LCD on PC's port pins

  CDHI;         // control/status mode
  RDHI;         // make sure LCD read mode is off
  WRLO;         // activate LCD write mode
  CELO;         // pulse ChipEnable LOW, > 80 ns, enables LCD I/O
  CEHI;         // disable LCD I/O
  WRHI;         // deactivate write mode

} // cput()
// -------------------------------------------------------------------------------

int dget(void)      // get data byte from LCD module
{
int lcd_byte = 0;

  while ((0x03 & sget()) != 0x03){
  	if(lcd_byte > 5) return 0;
	lcd_byte++;
  } // wait until display ready
  DATAIN; // make PC's port an input port
  WRHI;   // make sure WRITE mode is inactive
  CDLO;   // data mode
  RDLO;   // activate READ mode
  CELO;   // enable chip, which outputs data
  lcd_byte = DATAPIN;  // read data from LCD
  CEHI;   // disable chip
  RDHI;   // turn off READ mode
  DATAOUT; // make 8-bit parallel port an output port

  return(lcd_byte);
} // dget()

//----------------------------------------------------------------------------------------

void dput(int byte) // write data byte to LCD module over par. port
		    // assume PC port in data OUTPUT mode
{
  unsigned char i = 0;
  while ((0x03 & sget()) != 0x03){
  	if(i > 5) return;
	i++;
  } // wait until display ready
  CDLO;
  WRLO;         // activate LCD's write mode
  DATAPORT=byte;          // write value to data port
  CELO;                       // pulse enable LOW > 80 ns (hah!)
  //#asm("nop");
  //#asm("nop");
  CEHI;                       // return enable HIGH
  WRHI;                       // restore Write mode to inactive

 // using my P5/75 MHz PC with ISA bus, CE stays low for 2 microseconds

} // end dput()

//------------------------------------------------------------------------------------------

//======================================================================================
//
//	trafic layer:
//
//======================================================================================
void lcd_set_address(unsigned int addr){
	dput(addr);
	dput(addr>>8);
	cput(0x24);
}
//---------------------------------------------------------------------------------
void lcd_xy(int x, int y)  // set memory pointer to (x,y) position (text)
{
int addr;

  addr = T_BASE + (y * BYTES_PER_ROW) + x;
  lcd_set_address(addr);   // set LCD addr. pointer

} // lcd_xy()
//---------------------------------------------------------------------------------

void lcd_setpixel(int column, int row)  // set single pixel in 240x64 array
{

int addr;       // memory address of byte containing pixel to write
  if( (column>=XMAX) || (row>=YMAX) )return;
  addr =  G_BASE + (row*BYTES_PER_ROW)  + (column/8);
  lcd_set_address(addr);  // set LCD addr. pointer
  cput(0xf8 | ((7-column%8)) );  // set bit-within-byte command
}
//--------------------------------------------------------------------------------

void lcd_clrpixel(int column, int row)
{
int addr;       // memory address of byte containing pixel to write
  if( (column>=XMAX) || (row>=YMAX) )return;
  addr =  G_BASE + (row*BYTES_PER_ROW)  + (column/8);
  lcd_set_address(addr);  // set LCD addr. pointer
  cput(0xf0 | ((7-column%8)) );  // set bit-within-byte command


}

//---------------------------------------------------------------------------------

void lcd_pixel(int column, int row,char show)
{
int addr;       // memory address of byte containing pixel to write
if( (column>=XMAX) || (row>=YMAX) )return;
addr =  G_BASE + (row*BYTES_PER_ROW)  + (column/8);
lcd_set_address(addr);   // set LCD addr. pointer
if(show)
  cput(0xf8 | ((7-column%8)) );  // set bit-within-byte command
else
  cput(0xf0 | ((7-column%8)) );  // set bit-within-byte command
} 
//---------------------------------------------------------------------------------


//======================================================================================
//	HIGH LEVER FUNCTION
//	
//======================================================================================

//--------------------------------------------------------------------------------

void lcd_print_ram(int x,int y,char *string)  // send string of characters to LCD
{
	int i;
	int c;
	lcd_xy(x,y);
  	for (i=0;string[i]!=0;i++) {
 		c = string[i] - 0x20;     // convert ASCII to LCD char address
      	if (c<0) c=0;
      	dput(c);
      	cput(0xc0);               // write character, increment memory ptr.
  	} // end for

} // end lcd_string

//--------------------------------------------------------------------------------

void lcd_print(int x,int y,const char *string)  // send string of characters to LCD
{
int i;
int c;
lcd_xy(x,y);
  for (i=0;string[i]!=0;i++) {
      c = string[i] - 0x20;     // convert ASCII to LCD char address
      if (c<0) c=0;
      dput(c);
      cput(0xc0);               // write character, increment memory ptr.
  } // end for

} // end lcd_string

//--------------------------------------------------------------------------------

void lcd_clear_graph()    // clear graphics memory of LCD
{
	int i;
	lcd_set_address(G_BASE);     // addrptr at address G_BASE
	for (i=0;i<3840;i++) {   // must be  3840 ?????????????????     5120
		dput(0); cput(0xc0);               // write data, inc ptr.
	}	 // end for(i)
} // end lcd_clear_graph()
//----------------------------------------------------------------

void lcd_clear_text()
{
 int i;   
 lcd_set_address(T_BASE);     // addrptr at address T_BASE

 for (i=0;i<480;i++) {  //  must be 480   ???????????     640  
      dput(0); cput(0xc0);               // write data, inc ptr.
 } // end for(i)

} // lcd_clear_text()

//--------------------------------------------------------------------------------

/***********************************************************************
Draws a line from x1,y1 go x2,y2. Line can be drawn in any direction.
Set show to 1 to draw pixel, set to 0 to hide pixel.
***********************************************************************/
void lcd_line(int x1, int y1, int x2, int y2, unsigned char show) 
{
  int dy ;
  int dx ;
  int stepx, stepy, fraction;
  x1=XMAX-1-x1;
  x2=XMAX-1-x2;
  y1=YMAX-1-y1;
  y2=YMAX-1-y2;
  dy = y2 - y1;
  dx = x2 - x1;
  if (dy < 0) 
  {
    dy = -dy;
    stepy = -1;
  }
  else 
  {
    stepy = 1;
  }
  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else
  {
    stepx = 1;
  }
  dy <<= 1;
  dx <<= 1;
  lcd_pixel(x1,y1,show);
  if (dx > dy)
  {
    fraction = dy - (dx >> 1); 
    while (x1 != x2)
    {
      if (fraction >= 0)
      {
        y1 += stepy;
        fraction -= dx;
      }
      x1 += stepx;
      fraction += dy;  
      lcd_pixel(x1,y1,show);
    }
  }
  else
  {
    fraction = dx - (dy >> 1);
    while (y1 != y2)
    {
      if (fraction >= 0)
      {
        x1 += stepx;
        fraction -= dy;
      }
      y1 += stepy;
      fraction += dx;
      lcd_pixel(x1,y1,show);
    }
  }
}

void lcd_line_ver(
     unsigned char row,
     unsigned char s_col,
     unsigned char f_col,
     char show
){
     unsigned int adr;
     char i,data;
	 //s_col=XMAX-1-s_col;
	 row=YMAX-1-row;
     adr = G_BASE + (int)row*BYTES_PER_ROW + s_col;
     lcd_set_address(adr);
     if(show) data = 0xFF; else data = 0x00;
     cput(0xB0);
     for(i=s_col;(i<BYTES_PER_ROW)&&(i<=f_col);i++){
          dput(data);
     }
     cput(0xB2);
}

void lcd_box(int x1, int y1, int x2, int y2, unsigned char show)
{
lcd_line(x1,y1,x2,y1,show);  // up
lcd_line(x1,y2,x2,y2,show);  // down
lcd_line(x2,y1,x2,y2,show);  // right
lcd_line(x1,y1,x1,y2,show);  // left
}

void lcd_box_fill(int x1, int y1, int x2, int y2, unsigned char show){
     int addr,i,j;
     addr =  G_BASE +y1*BYTES_PER_ROW+x1;    
     for(j=y1;j<y2;j++){
          lcd_set_address(addr);   // set LCD addr. pointer
          cput(0xB0);
          for(i=x1;i<x2;i++){
	          if(show)dput(0xFF); 
	          else dput(0x00);
     	}
          cput(0xB2);
     	addr+=30;
     }     
}

/*====================================================================
		LCD SETUP AND INIT
	lcd_setup();

=====================================================================*/

void lcd_setup()  // make sure control lines are at correct levels
{
	//BG_DDRHI;
 	CE_DDRHI;
 	RD_DDRHI;
 	WR_DDRHI;
 	CD_DDRHI;
 	RESET_DDRHI;
 	CEHI;  // disable chip
 	RDHI;  // disable reading from LCD
 	WRHI;  // disable writing to LCD
 	CDHI;  // command/status mode
 	DATAOUT; // make 8-bit parallel port an output port
 	RESETHI;
 	_delay_ms(5);
 	RESETLO;
 	_delay_ms(5);
 	RESETHI;
} // end lcd_setup()

//---------------------------------------------------------------------------

void lcd_init()  // initialize LCD memory and display modes
{
 	//dput(G_BASE%256);
 	dput(G_BASE&0x00FF);
	dput(G_BASE>>8);
 	cput(0x42);       // set graphics memory to address G_BASE

 	//dput(BYTES_PER_ROWG%256);
	dput(BYTES_PER_ROWG&0x00FF);
 	dput(BYTES_PER_ROWG>>8);
 	cput(0x43);  // n bytes per graphics line

 	//dput(T_BASE%256);
	dput(T_BASE&0x00FF);
 	dput(T_BASE>>8);
 	cput(0x40);       // text memory at address T_BASE

 	//dput(BYTES_PER_ROW%256);
	dput(BYTES_PER_ROW&0x00FF);
 	dput(BYTES_PER_ROW>>8);
 	cput(0x41);  // n bytes per text line


 	cput(0x81);  // mode set: Graphics OR Text, ROM CGen

 	cput(0xa7);  // cursor is 8 lines high
 	dput(0x00);
 	dput(0x00);
 	cput(0x21);  // put cursor at (x,y) location

 	cput(0x9C);  
	      // (For cursor to be visible, need to set up position

} // end lcd_init()

