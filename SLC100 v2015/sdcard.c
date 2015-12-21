#include <util/delay.h>
#include "sdcard.h"
#include "tff.h"
#include "integer.h"
#include "diskio.h"
#include "ds1307.h"
#include "spi.h"
#include "timming.h"
#include "uart.h"
#include "ade7758.h"
#include "sysComm.h"
#include "stdio.h"
#include "string1.h"

//FILINFO finfo;
FATFS __attribute__ ((section (".noinit"))) fatfs;				/* File system object for each logical drive */
//FATFS fatfs;	
//FIL __attribute__ ((section (".noinit"))) file0;//,file1;
FIL file0;//,file1;
char buff[500] __attribute__ ((section (".noinit")));
UINT nread,nwrite;

void initSpiInOut(void){
	//PW_ON_INIT;				// For SD Card
	//SOCKINIT;					// For SD Card
	SPI_PORT = PORT_EN_DEVICE;	// Enable drivers 
	SPI_DDR  = DDR_EN_DEVICE;	// for ADE and SD Card
	//PW_ON;                    // Power on SD Card
	//&fatfs = 0x1500;
}

void initSdcard(void){
	deselectADE();	
	//PW_ON;
	SPI_DDR  = DDR_EN_DEVICE;
	SPCR = 0b01010000;			/* Initialize SPI port (Mode 0) */
	SPSR = 0b00000000;			// SCK = Fosc / 2 ;
	SELECT();
}
void initFirstTimeSDCard(void){
//	unsigned char buffer[2];
	//FATFS  fatfs;
	//unsigned int time;
	//readEEPROM(buffer,39,40);
	//time = ((int) buffer[0]) * 256 + ((int) buffer[1]);
	//setLogTime(time);
	initSdcard();
	_delay_ms(1);
	//_delay_ms(2);
	disk_initialize(0);
	f_mount(0, &fatfs);
	//f_mount(0, FatFs);
}
void deselectSdcard(void){

	DESELECT();

}

void writeLog(TimeStruct * Time,char * str){

	//unsigned char buff[400];
	char file_name[30];
	//initSdcard();
	initFirstTimeSDCard();
	_delay_ms(1);
	sprintf(file_name,"%02u_%02u_%02u.txt",Time->date,Time->month,Time->year%100);	
	//sprintf(file_name,"minheft.txt");
	if(f_open(&file0,file_name, FA_READ | FA_WRITE) == 0){
		do{
			f_read(&file0, buff, sizeof(buff)-1, &nread);
		}while(nread != 0);
	}else{
		f_open(&file0,file_name, FA_READ | FA_WRITE | FA_CREATE_NEW);
		//f_sync(&file0);
	}
	f_puts(str,&file0);
	//f_putc('M',&file0);
	f_putc('\r',&file0);
	f_putc('\n',&file0);
	//f_sync(&file0);
	f_close (&file0); 

}
/*
void testWriteLog(){
	unsigned char Buff[50];
	char file_name[15];
	//initSdcard();
	initFirstTimeSDCard();
	_delay_ms(1);
	sprintf(file_name,"minheft.txt");	
	//sprintf(file_name,"minheft.txt",Time->date,Time->month,Time->year%100);
	if(f_open(&file0,file_name, FA_READ | FA_WRITE) == 0){
		do{
			f_read(&file0, Buff, sizeof(Buff)-1, &nread);
		}while(nread != 0);
	}else{
		f_open(&file0,file_name, FA_READ | FA_WRITE | FA_CREATE_NEW);
		//f_sync(&file0);
	}
	//f_puts(str,&file0);
	f_putc('M',&file0);
	f_putc('\n',&file0);
	//f_sync(&file0);
	f_close (&file0);

}
*/
void writeSystemLog(TimeStruct * Time,char SystemReset,char ModemReset,char OnOff,char ChangeSchedule,char ChangeTimeDate,char Setting,char ResetEnergy){
	//unsigned char buff[50];
	char file_name[30];
	char str[60];
	//initSdcard();
	initFirstTimeSDCard();
	_delay_ms(1);
	sprintf(str,"%02u %02u %02u %02u %01u %01u %01u %01u %01u %01u %01u",
		Time->hour,Time->minute,Time->second,Time->date,SystemReset,ModemReset,OnOff,ChangeSchedule,ChangeTimeDate,Setting,ResetEnergy);
	sprintf(file_name,"%02u_%02u.txt",Time->month,Time->year%100);	
	//sprintf(file_name,"minheffft.txt",Time->month,Time->year%100);
	if(f_open(&file0,file_name, FA_READ | FA_WRITE) == 0){
		do{
			f_read(&file0, buff, sizeof(buff)-1, &nread);
		}while(nread != 0);
	}else{
		f_open(&file0,file_name, FA_READ | FA_WRITE | FA_CREATE_NEW);
		//f_sync(&file0);
	}
	f_puts(str,&file0);
	f_putc('\r',&file0);
	f_putc('\n',&file0);
	//f_sync(&file1);
	f_close (&file0);  
}
void readSD(unsigned char date,unsigned char month,unsigned char year,unsigned char hour1,unsigned char minute1,unsigned char hour2,unsigned char minute2){
//Moi mot lan doc, doc dung mot khung du lieu
//Neu trong khung sai du lieu thi bo qua va doc khung tiep theo
	char FileName[20];
	//unsigned char buff[90];
	char desData[60];
	unsigned int start,stop;
	start = (int)hour1*60 + (int)minute1;
	stop = (int)hour2*60 + (int)minute2;
	sprintf(FileName,"%02u_%02u_%02u.txt",date,month,year);	
	//initSdcard();
	initFirstTimeSDCard();
	f_open(&file0,FileName, FA_READ | FA_WRITE);
	do{
		if(isEnergyTime()){   // 5 giay mot lan doc
			initADE();		  // Trong qua trinh doc File van tich phan duoc Active Energy
			readTotalEnergy();
			resetModemWatchDog();//Khong cho MODEM reset
			initSdcard();
		}
		f_read(&file0, buff, 87, &nread);
		buff[nread] = 0; 	
		while(buff[86] != '\n'){ //neu co sai data thi bo qua va doc khung ke tiep
			f_read(&file0, buff, 1, &nread);//neu ko co '\n' trong file thi se thoat ra sau khi doc het file
			if(buff[0]=='\n'){
				f_read(&file0, buff, 87, &nread);
				buff[nread] = 0; 
				//if(tBuff[68] == '\n') break;
			} 
			if(nread == 0) break;
		}	
		if((((int)(buff[0] - '0')*10 + (int)(buff[1] - '0'))*60+(int)(buff[3] - '0')*10 + (int)(buff[4] - '0')) >= start){						
			break;
		}	
	}while(nread != 0);	
	do{
		if(isEnergyTime()){   // 5 giay mot lan doc
			initADE();		  // Trong qua trinh doc File van tich phan duoc Active Energy
			readTotalEnergy();
			resetModemWatchDog();//Khong cho MODEM reset
			initSdcard();
		}
		while(buff[86] != '\n'){ //neu co sai data thi bo qua va doc khung ke tiep
			f_read(&file0, buff, 1, &nread);//neu ko co '\n' trong file thi se thoat ra sau khi doc het file
			if(buff[0]=='\n'){
				f_read(&file0, buff, 87, &nread);
				buff[nread] = 0; 
				//if(tBuff[68] == '\n') break;
			} 
			if(nread == 0) break;
		}
		if((((int)(buff[0] - '0')*10 + (int)(buff[1] - '0'))*60+(int)(buff[3] - '0')*10 + (int)(buff[4] - '0')) > stop){						
			break;
		}

		convertData(buff,desData);
		writeStringUART2("AT+SENDFILE=\"");
		writeMStringUART2(desData);
		writeStringUART2("\"\n\r");
			
		f_read(&file0,buff,87,&nread);
		buff[nread] = 0;	
		//_delay_ms(10);				
	}while(nread != 0);	
	writeStringUART2("AT+SENDFILE=\"");
	writeStringUART2(" END");
	writeStringUART2("\"\n");
	f_close (&file0);	
}
void readSystemLog(unsigned char month,unsigned char year){
//Moi mot lan doc, doc dung mot khung du lieu
//Neu trong khung sai du lieu thi bo qua va doc khung tiep theo
	char FileName[15];
	//unsigned char buff[30];
	char desData[25];
	sprintf(FileName,"%02u_%02u.txt",month,year);	
	//initSdcard();
	initFirstTimeSDCard();
	f_open(&file0,FileName, FA_READ | FA_WRITE);
	do{
		if(isEnergyTime()){   // 2 giay mot lan doc
			initADE();		  // Trong qua trinh doc File van tich phan duoc Active Energy
			readTotalEnergy();
			resetModemWatchDog();//Khong cho MODEM reset
			initSdcard();
		}
		f_read(&file0,buff,27,&nread);
		buff[nread] = 0; 	
		while(buff[26] != '\n'){ //neu co sai data thi bo qua va doc khung ke tiep
			f_read(&file0, buff, 1, &nread);//neu ko co '\n' trong file thi se thoat ra sau khi doc het file
			if(buff[0]=='\n'){
				f_read(&file0,buff,27,&nread);
				buff[nread] = 0; 
			} 
			if(nread == 0) break;
		}
		if(nread >= 27){
			convertData2(buff,desData);
			writeStringUART2("AT+SENDFILE=\"");
			writeMStringUART2(desData);
			writeStringUART2("\"\n\r");
		}	
	}while(nread != 0);	
	writeStringUART2("AT+SENDFILE=\"");
	writeStringUART2(" END");
	writeStringUART2("\"\n");
	f_close (&file0);	
}

//Write File
void writeFile(char * str){
/*

	//unsigned char buff[400];
	char file_name[15];
	//initSdcard();
	initFirstTimeSDCard();
	_delay_ms(1);	
	sprintf(file_name,"file1.txt");
	if(f_open(&file0,file_name, FA_READ | FA_WRITE) == 0){
		do{
			f_read(&file0, buff, sizeof(buff)-1, &nread);
		}while(nread != 0);
	}else{
		f_open(&file0,file_name, FA_READ | FA_WRITE | FA_CREATE_NEW);
		//f_sync(&file0);
	}
	f_puts(str,&file0);
	//f_putc('M',&file0);
	//f_putc('\n',&file0);
	//f_sync(&file0);
	f_close (&file0); */

}
void readFile(void){
/*
	//char FileName[15];
	//unsigned char desData[25];
	char tBuff[92],tempBuff[2];
	unsigned int i = 0;
	//sprintf(FileName,"%02u_%02u.txt",month,year);	
	//initSdcard();
	writeStringUART2("\r\nsendhttp");
	writeStringUART2("\r\n");
	_delay_ms(200);
	initFirstTimeSDCard();
	f_open(&file0,"Addnode.txt", FA_READ | FA_WRITE);
	do{
		//if(isEnergyTime()){   // 2 giay mot lan doc
		//	initADE();		  // Trong qua trinh doc File van tich phan duoc Active Energy
		//	readTotalEnergy();
		//	resetModemWatchDog();//Khong cho MODEM reset
		//	initSdcard();
		//}
		f_read(&file0, tempBuff, 1, &nread);
		tBuff[i] = tempBuff[0];
		i++; 
		if(tempBuff[0] == '\n'){
			tBuff[i] = 0;
			i = 0;	
			writeMStringUART2(tBuff);
			_delay_ms(1);
		}
	}while(nread != 0);	
	writeStringUART2("\r\n");
	f_close (&file0);	
	_delay_ms(2000);	*/
}

/*
void copyFile(char * sourName,char * destName){
	unsigned char Buff[500],str[5];
	unsigned int temp,temp1;
	writeStringUART2("Coping....");
	TimeStruct time;
	readRealTime(&time);
	temp = time.minute*60+time.second;
	initSdcard();
	f_unlink(destName); //Delete file 2.txt
	//_delay_ms(1);
	f_open(&file0,destName, FA_READ | FA_WRITE | FA_CREATE_NEW);
	f_sync(&file0);
	f_open(&file1,sourName, FA_READ | FA_WRITE);
	do{	
		f_read(&file1, Buff,499, &nread);
		f_write(&file0,Buff,nread,&nwrite);
		//f_write(&file1,"M ",2,&nwrite);		
	}while(nread != 0);	
	f_close (&file1);
	f_close (&file0);
	writeStringUART2(" Copy Done !!!\n\r");
	readRealTime(&time);
	temp1 = time.minute*60+time.second;
	temp = temp1 - temp;
	wordToString(temp,str);
	writeStringUART2("Copy time : ");
	writeMStringUART2(str);
	writeStringUART2(" seconds\n\r");
}

void deleteFile(char * Name){
	initSdcard();
	f_unlink(Name); //Delete file 
}
*/
unsigned long checkCapacitor(unsigned long* free,unsigned long* user){
	unsigned long clu;
	FATFS* fs;
	initFirstTimeSDCard();
	if((/*type=*/f_getfree(0, &clu, &fs/*FS_FAT32*/)))		//check the nho co hay ko
		return 0;
	else
	{
		(*user)  = (fs->max_clust - clu - 2) * (fs->csize) * 512;
		(*free)  = clu*(fs->csize)*512;		// Free = cluter * number sector per cluter * 512Byte
		return (fs->max_clust - 2) * (fs->csize) * 512;
	}
}
