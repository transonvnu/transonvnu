
/*****************************************************
Project	: LightController
Version	: 3.00
Date		: 18/03/2010
Modify	: 29/05/2015
Author  : minheft + transonvnu
Company : Mems + SCEC                
Comments: 

Chip type           : ATmega128...
Program type        : Application
Clock frequency     : 14.7456 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*****************************************************/

#include "main.h"
#include "integer.h"
//#include "xitoa.h"
#include "tff.h"
#include "diskio.h"
#include "MBAT128.h"

#include "spi.h" 
#include "manual_i2c.h" 
#include "ade7758.h"
#include "ds1307.h"
#include "relays.h"       
#include "schedule.h"
#include "sysComm.h"
#include "timming.h"
#include "string1.h"
#include "uart.h"  
#include "adc.h" 
#include "sdcard.h" 
#include "t6963c.h"
//#include "font.h"
#include "lcd.h"

/*EEPROM memory map

0  --> 24 : Schedule
25 --> 36 : Reserved
37 --> 38 : MODEM timeout
39 --> 40 : Write Log's time
41 --> 47 : Reserved
48 --> 71 : E,U,I Coeff Calibration
72 --> 75 : TI Coef
// Added 04/03/2009
76 --> 77 : send state time 1
78 --> 79 : send state time 2
80 --> 81 : send state time 3
82 --> 83 : send state time 4
84 --> 85 : send state time 5
86 --> 107: 22 config bytes
108 --> 118: 11 sever's numbers
119 --> 121: 3 bytes PHACALIB
// Added 30/07/2009
122 --> 124: 3 Bytes U1,U2,U3
125 --> 130: 6 Bytes h1,m1,h2,m2,h3,m3
131 --> 172: 42 Bytes decorate schedule //22/11/2010
*/
uint8_t		scheduleTasks[5][5] EEMEM;
uint32_t	phaseActiveEnergy[3] EEMEM;
uint16_t	GSMmodemTimeOut EEMEM;
uint16_t	logDataTime EEMEM;
uint8_t		reserved[7] EEMEM;
uint16_t	parTICoef1 EEMEM,parTICoef2 EEMEM;
uint32_t	energyCalPar[3] EEMEM;
uint16_t	voltageCalPar[3] EEMEM;
uint16_t	ampereCalPar[3] EEMEM;
uint16_t	TiCoefPar1 EEMEM, TiCoefPar2 EEMEM;
uint16_t	_sendStateTime[5] EEMEM;
uint8_t		centerServerPort[5] EEMEM;	// PORT	02015
uint8_t		slc100ID[5]	EEMEM;			// ID		000xx
uint8_t		serverStaticIP[4][3] EEMEM;	// IP		113.160.251.175
uint8_t		slcPhoneNum[11]	EEMEM;		// Phone No 00915888888
uint8_t		phaseCalib[3]	EEMEM;		//
	

#define MAX_GROUP_NODE	4
#define BIT(x)     (1<<x)

#define DEBUG_MODE	0

char args[600];// __attribute__ ((section (".init1")));
char tBuffer[100];// __attribute__ ((section (".init1")));
char SERVER_NUMBERS[12];
//extern unsigned char RegBytes[10000] __attribute__ ((section (".init1")));

//char Uart_Select;

extern unsigned int voltageDim[3];// = {55555, 55555, 55555};
char resetWatchDog = 0;
char realTimeError = 0;
char scheduleError = 0;
char schedule1Error = 0;


void init (void){
//		int temp;
		//TimeStruct time;
		wdt_disable();    // Disable Watch Dog timer
		wdt_enable(WDTO_2S);
		cli();            // Global disable interrupts
		_delay_ms(100);  
		externalMemoryInit(); 
		initKeyboard();
		initRelays();	  // relays.h
		initSpiInOut();   // sdCard.h
        initUART();		  // uart.h
        initADC();        // adc.h
// DS1307 Real Time Clock initialization
// Square wave output on pin SQW/OUT: On
// Square wave frequency: 1Hz	
        rtc_init(0,1,0);  // ds1307.h
		_delay_ms(50);
		//readRealTime(&time);
        initTimer0();     // timming.h
		timer2_init();		
		InitMBSer();
        initModemWatchDog(); //sysComm.h
        interrupt_init(); 
        initSchedule();
		initDecorateSchedule();
		initTransducer();	
		
		sei();					 //  Global enable interrupts	
		_delay_ms(10);
		lcd_setup();
		lcd_init();
		lcd_clear_graph();    	 //  clear graphics memory of LCD
		lcd_clear_text();  		 //  clear text memory of LCD
		initFirstTimeSDCard();   //  Init First time for SD card	
		_delay_ms(100);
		//wdt_enable(WDTO_2S);     //  Enable Watch Timer with expire time ~ 2 seconds
}

int main(void)
{       
		TimeStruct currentTime1,timeReset,time; 
        EnergySruct tEnergy;//, tvEnergy, teen[3];
        unsigned int voltage[3] = {22000, 22000, 22000};//,periodTimeSendNodeData = 0;
		unsigned int current[3] = {800, 800, 800},tempSendTime[5],countResetModem = 0,countACmainsDetector = 0,i = 0,i2=0,scanNode = 1;
		unsigned char phase[3] = {84, 84, 84},CL[24],softResetModem = 0;
	    char strE1[9],strE2[9],strE3[9];
		int cU[4],cI[4];
        signed char tempSchedule[5][5],tempSchedule1[14][3];
		unsigned char command,lastCurrentTask1 = 0,lastCurrentOnOff1 = 0,manualOnOff1 = 0;//,i=0,statusBattery;//,isError,buffer[56];
		char cmd[23],cmdEchelon[23];//,U = 170;//,args[350];
		char resetSystem = 1,Disconnect = 1,resetBoard = 0;//,dem = 19;//,modBusOk = 0;
		long cE[4];//temp,temp1,ac,ap,re,
        //char mmm = 1;
		//unsigned char nrID[6] = {0x05,0x01,0xF8,0x65,0x24,0x00};
		unsigned long TotalSD = 0,FreeSD = 0,UserSD = 0;//add 23/06/2011 sd card status
		char echelonCMDpending = 0,numOfBytesEchelonCmd = 0;
		init();
		initADE();
		tEnergy = readTotalEnergy();
		modem_on_off(1);   
        acquy_on_off(1); 
		readRealTime(&time);
		//chargBattery(1);
	    /*	
		time.year = 9;
		time.month = 9;
		time.date = 9;
		time.minute = 9;
		time.hour = 9;
		time.second = 0;
		writeRealTime(&time);
		*/
		/*
		cE[0] = 5625;//12500
		cE[1] = 5625;
		cE[2] = 5625;
		cU[0] = 56;//56
		cU[1] = 56;
		cU[2] = 56;
		cI[0] = 137;//
		cI[1] = 137;
		cI[2] = 137;
		setCalibrationParam(cE,cU,cI);
		//setModemTimeout(30);
		*/
		//setModemTimeout(10);
	
#if DEBUG_MODE == 1  
		/* 
		while(1){
		//	writeLog();
			//testWriteLog();
			chargBattery(1);
			//acquy_on_off(1); 
			//modem_on_off(1); 
			//relay2(1); 
			_delay_ms(3000);
			chargBattery(0);
			//acquy_on_off(0); 
			modem_on_off(0);
			relay2(0);  
			_delay_ms(3000); 
			//PORTA ^=BIT(6);
		}
		*/
		resetTotalEnergy();
		while(1){
			calibADE();
			_delay_ms(100);
		}
	
#else	
		draw_table();
		//testWriteLog();
		readRealTime(&timeReset);
		writeSystemLog(&timeReset,1,0,0,0,0,0,0);//System Reset
		_delay_ms(100);
		// Read Server's Numbers
		readEEPROM(tBuffer,108,118);
		if(tBuffer[1] == 0){
			for(i=1;i<11;i++){	
				SERVER_NUMBERS[i-1] = tBuffer[i] + '0';
			}
			SERVER_NUMBERS[10] = 0;//end of string
		}
		else{
			for(i=0;i<11;i++){	
				SERVER_NUMBERS[i] = tBuffer[i] + '0';
			}
			SERVER_NUMBERS[11] = 0;//end of string
		}
		// End of read Server's Numbers	
        while (1){	
			//check current status 	
			if(resetBoard == 0){
				resetWatchDog = 0;
			}
			//else{
			//	modem_on_off(0);//cho MODEM cat nguon 1 thoi gian 20s
			//}		
			if(isNextSecond()){ //Moi giay mot lan doc			
			    readRealTime(&currentTime1); 
				initADE();
				voltage[0] = readVoltage(1);
                for (i = 0; i< 3; i++){
			    	voltage[i] = readVoltage(i + 1);
			      	current[i] = readCurrent(i + 1);
			        phase[i]   = readPhase(i + 1);		
		        } 
				getSchedule(tempSchedule,tempSendTime);
				if(getScreen()==0){
					DisplayU_I_P(voltage,current,phase);
					DisplaySchedule(tempSchedule);
					DisplayTime(&currentTime1);
					DisplayDecorateLightStatus(getDecorateLightStatus());
				}
		        if(isSchedule(&currentTime1)){
			        changeLightStatus(checkCurrentTask());
			        jumpNextTask();
					if(getModemConnected()){
				        writeUART2Header();
						threeByteToHexString(0x010000,tBuffer);
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)      //added 04_05_2010
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						prepareLightStatus(getLightStatus(), tBuffer);
						writeMStringUART2(tBuffer);
						prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
						writeMStringUART2(tBuffer);	
						writeUART2Footer();
					}
					//Write Log	File
					l_to_a(tEnergy.energy[0],strE1);
					l_to_a(tEnergy.energy[1],strE2);
					l_to_a(tEnergy.energy[2],strE3);
					sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
							currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),voltage[0],voltage[1],voltage[2],
							current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
					writeLog(&currentTime1,tBuffer);
					//if((getModemConnected() == 0)&&(countResetModem > 3)){
						//send_SMS(SERVER_NUMBERS,tBuffer);
					//}
					if(getScreen()==0) DisplayLightStatus(getLightStatus());
		        }
				//******** Decorate Light, Add 24/11/2010 *********************
				lastCurrentOnOff1 = checkCurrentTask1();
				lastCurrentTask1 = getCurrentTask1();
				findCurrentTask1(&currentTime1);
				if(manualOnOff1 == 0){
					changeDecorateLightStatus(checkCurrentTask1());
				}else{
					if(checkNextTask1(lastCurrentTask1) == getCurrentTask1()){
						manualOnOff1 = 0;
						changeDecorateLightStatus(checkCurrentTask1());
					
					}
				}
				//*************************************************************
				if(isSdCardTime()){
					l_to_a(tEnergy.energy[0],strE1);
					l_to_a(tEnergy.energy[1],strE2);
					l_to_a(tEnergy.energy[2],strE3);
					sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
							currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),voltage[0],voltage[1],voltage[2],
							current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
					//sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
					//		currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),(unsigned int)22000,(unsigned int)32000,(unsigned int)36000,
					//		current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
					writeLog(&currentTime1,tBuffer);
				}
				if(getModemConnected()){
					if(isSendStateTime()){//Truyen trang thai ve dinh ky
						if(realTimeError > 0){//Neu loi Realtime truyen yeu cau set lai realtime
							writeUART2Header();
							threeByteToHexString(0x10FEFE,tBuffer);//command 16 yeu cau set lai Realtime
							writeMStringUART2(tBuffer);
							for(i=0;i<16;i++)      //added 04_05_2010
								tBuffer[i] = '0';
							tBuffer[i] = 0;
							writeMStringUART2(tBuffer);
							writeUART2Footer();
							_delay_ms(500);
						}
						if(scheduleError > 0){//Neu loi schedule truyen yeu cau set lai schedule
							writeUART2Header();
							threeByteToHexString(0x11FEFE,tBuffer);//command 17 yeu cau set lai schedule
							writeMStringUART2(tBuffer);
							for(i=0;i<16;i++)      //added 04_05_2010
								tBuffer[i] = '0';
							tBuffer[i] = 0;
							writeMStringUART2(tBuffer);
							writeUART2Footer();
							_delay_ms(500);
						}
						if(schedule1Error > 0){//Neu loi schedule1 truyen yeu cau set lai schedule1
							writeUART2Header();
							threeByteToHexString(0x12FEFE,tBuffer);//command 18 yeu cau set lai decorate schedule
							writeMStringUART2(tBuffer);
							for(i=0;i<16;i++)      //added 04_05_2010
								tBuffer[i] = '0';
							tBuffer[i] = 0;
							writeMStringUART2(tBuffer);
							writeUART2Footer();
							_delay_ms(500);
						}
						writeUART2Header();
						threeByteToHexString(0x1FEFE,tBuffer);
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)      //added 04_05_2010
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						prepareLightStatus(getLightStatus(), tBuffer);
						writeMStringUART2(tBuffer);
						prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
						writeMStringUART2(tBuffer);	
						writeUART2Footer();
					}	
					if(resetSystem == 1){
						resetSystem = 0;
						writeUART2Header();
						threeByteToHexString(0x13FEFE,tBuffer);//command 19
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)      //added 04_05_2010
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&timeReset, tBuffer);
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(countResetModem,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(0xFFFF,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
						countResetModem = 0;
					}
					if(Disconnect == 1){
						Disconnect = 0;
						writeUART2Header();
						threeByteToHexString(0x14FEFE,tBuffer);//command 20
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)      //added 04_05_2010
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&timeReset, tBuffer);
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						wordToHexString(countResetModem,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
						countResetModem = 0;
					}

					//Truyen trang thai tung Node ve Server
					while((RegBytes[32+(scanNode<<5)] == 0)&&(scanNode<=MAX_NODE)){
						scanNode++;
					}
					if(scanNode>MAX_NODE){
						scanNode = 1;
					}
					// == 3 reponse cua lenh Read					==255    Auto send status								==1 response cua lenh Write
					if(((RegBytes[32+32*scanNode] == 3)||(RegBytes[32+32*scanNode] == 255))&&(RegBytes[33+32*scanNode] == 1)){
						// 28/05/2015	su dung th? t? register làm ID cho node. ( Son )
						RegBytes[34+32*scanNode] = scanNode;
						//----------------------------------------------------------
						writeUART2Header();
						threeByteToHexString(0x38FEFE,tBuffer);//truyen trang thai Node
						writeMStringUART2(tBuffer);
						for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
							tBuffer[i2] = '0';
						tBuffer[i2] = 0;
						writeMStringUART2(tBuffer);
						for(i2=32+32*scanNode;i2<60+32*scanNode;i2++){
							byteToHexString((char)RegBytes[i2],tBuffer);
							writeMStringUART2(tBuffer);
						}
						writeUART2Footer();	
						RegBytes[32+32*scanNode] = 0;//clear Flag
					}//End if(((RegBytes[32+32*i] == 3)...
					if(((RegBytes[32+32*scanNode] == 1)||(RegBytes[32+32*scanNode] == 3))&&(RegBytes[33+32*scanNode] == 3)){//truyen node schedule
						// 28/05/2015	su dung th? t? register làm ID cho node. ( Son )
						RegBytes[34+32*scanNode] = scanNode;
						//----------------------------------------------------------
						writeUART2Header();
						threeByteToHexString(0x3AFEFE,tBuffer);//truyen node schedule
						writeMStringUART2(tBuffer);
						for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
							tBuffer[i2] = '0';
						tBuffer[i2] = 0;
						writeMStringUART2(tBuffer);
						for(i2=32+32*scanNode;i2<64+32*scanNode;i2++){
							byteToHexString((char)RegBytes[i2],tBuffer);
							writeMStringUART2(tBuffer);
						}
						writeUART2Footer();	
						RegBytes[32+32*scanNode] = 0;//clear Flag
					}//End if(((RegBytes[32+32*i] == 3)...              
					//End truyen Data Node ve Server

				}//end if(getModemConnected())...
				else{
					Disconnect = 1;
				}
				if(isEnergyTime()){   // 1 giay mot lan doc
					initADE();
					voltage[0] = readVoltage(1);
					tEnergy = readTotalEnergy();
					if(getScreen() == 0) DisplayE(tEnergy);
				}
				//reset modem if lin timeout    
				if (isModemTimeout()){
					if(countResetModem <= 1000){
						countResetModem++;
					}	
					if(softResetModem < 10){
						softResetModem++;	
						writeStringUART2("AT+CFUN=1\r\n");//soft reset modem
						_delay_ms(100);
					}else{	
						softResetModem = 0;
						DDRB |= 0x20;	
						_delay_ms(50);
						modem_on_off(0);//hard reset modem
						_delay_ms(2000);
						_delay_ms(2000);
						modem_on_off(1);
						_delay_ms(500);
						DDRB &= 0xDF;
						writeSystemLog(&currentTime1,0,1,0,0,0,0,0);//Modem Reset
					}					
				} 
				if(voltage[0] <= 500){//Add 05/08/2010
					countACmainsDetector++;
					if(countACmainsDetector >= 600){ // phase 1  <= 50V trong 30 giay lien tiep
													// Mat pha 1
						//Luu Data vao SD Card
						countACmainsDetector = 0;
						l_to_a(tEnergy.energy[0],strE1);
						l_to_a(tEnergy.energy[1],strE2);
						l_to_a(tEnergy.energy[2],strE3);
						sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
								currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),voltage[0],voltage[1],voltage[2],
								current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
						writeLog(&currentTime1,tBuffer);
						writeSystemLog(&currentTime1,2,0,0,0,0,0,0);//Mat dien
						//Gui Data ve Server
						writeUART2Header();
						threeByteToHexString(0x1FEFE,tBuffer);
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)      //added 04_05_2010
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						prepareLightStatus(getLightStatus(), tBuffer);
						writeMStringUART2(tBuffer);
						prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
						writeMStringUART2(tBuffer);	
						writeUART2Footer();
						_delay_ms(500);
						acquy_on_off(0);             // Ngat Ac Quy ra khoi mach
					}
				}else{
					countACmainsDetector = 0;
				}	
			 
			}//End if(isNextSecond())
			//check command		
			command = readCommand(args,cmd);			//read command from server

			switch (command){
			
				case 0:	//0x00
				    //Lenh dong cat	tu Server
					//Truyen ve trang thai + schedule table 					
					changeLightStatus(args[0]);
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareRealtimeStatus(&currentTime1, tBuffer);
					writeMStringUART2(tBuffer);
					prepareLightStatus(getLightStatus(), tBuffer);
					writeMStringUART2(tBuffer);
					getSchedule(tempSchedule,tempSendTime);//Modified 4/3/2009
					prepareScheduleStatus(tempSchedule,tempSendTime,tBuffer);//Modified 4/3/2009
					writeMStringUART2(tBuffer);
					prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
					//prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
					writeMStringUART2(tBuffer);	
					writeUART2Footer();
					l_to_a(tEnergy.energy[0],strE1);//a number long to charater array
					l_to_a(tEnergy.energy[1],strE2);
					l_to_a(tEnergy.energy[2],strE3);
					sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
							currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),voltage[0],voltage[1],voltage[2],
							current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
					writeLog(&currentTime1,tBuffer);
					if(getScreen()==0) DisplayLightStatus(getLightStatus());
					messageBox(cmd);
					_delay_ms(50);
					writeSystemLog(&currentTime1,0,0,2,0,0,0,0);//Sever controled on/off
					break;
				case 1:	//0x01
				    //Lenh yeu cau truyen trang thai
					//khong truyen schedule table
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareRealtimeStatus(&currentTime1, tBuffer);
					writeMStringUART2(tBuffer);
					prepareLightStatus(getLightStatus(), tBuffer);
					writeMStringUART2(tBuffer);
					prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();	
					//messageBox(cmd);
					break;
			    case 2:	//0x02
			        //Lenh set real time
	                currentTime1.hour = args[0];
					currentTime1.minute = args[1];
					currentTime1.date = args[2]; 
					currentTime1.month = args[3];
					currentTime1.year = args[4];
					currentTime1.day = args[5];//06/12/2010
					currentTime1.second = 0;
					if(writeRealTime(&currentTime1)){
						realTimeError = 0;//Reset Error
						writeUART2Header();
						writeMStringUART2(cmd);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
						messageBox(cmd);
						writeSystemLog(&currentTime1,0,0,0,0,2,0,0);
					}else{
						messageERROR();
					}
					break;	
				case 3:	//0x03 //Modified 04/03/2009
				    //change schedule
					//h1 p1 s1  t1   h2 p2 s2 t2   h3 p3 s3   t3    h4 p4 s4   t4    h5  p5  s5    t5
					//0  1  2   3,4  5  6  7  8,9  10 11 12  13,14  15 16 17  18,19  20  21  22   23,24
					tempSchedule[0][0] = args[0];//18
					tempSchedule[0][1] = args[1];//00
					tempSchedule[0][2] = args[5];//03
					tempSchedule[0][3] = args[6];//21
					tempSchedule[0][4] = args[2];//00
				
					tempSchedule[1][0] = args[5];
					tempSchedule[1][1] = args[6];
					tempSchedule[1][2] = args[10];
					tempSchedule[1][3] = args[11];
					tempSchedule[1][4] = args[7];
				
					tempSchedule[2][0] = args[10];
					tempSchedule[2][1] = args[11];
					tempSchedule[2][2] = args[15];
					tempSchedule[2][3] = args[16];
					tempSchedule[2][4] = args[12];
				
					tempSchedule[3][0] = args[15];
					tempSchedule[3][1] = args[16];
					tempSchedule[3][2] = args[20];
					tempSchedule[3][3] = args[21];
					tempSchedule[3][4] = args[17];
				
					tempSchedule[4][0] = args[20];
					tempSchedule[4][1] = args[21];
					tempSchedule[4][2] = args[0];
					tempSchedule[4][3] = args[1];
					tempSchedule[4][4] = args[22];
					
					tempSendTime[0] = ((unsigned int)args[3]<<8) + args[4];
					tempSendTime[1] = ((unsigned int)args[8]<<8) + args[9];
					tempSendTime[2] = ((unsigned int)args[13]<<8) + args[14];
					tempSendTime[3] = ((unsigned int)args[18]<<8) + args[19];
					tempSendTime[4] = ((unsigned int)args[23]<<8) + args[24];

					if(setSchedule(tempSchedule,tempSendTime)){
						scheduleError = 0;//Reset Error Schedule
						writeUART2Header();
						writeMStringUART2(cmd);
						getSchedule(tempSchedule,tempSendTime);
						prepareScheduleStatus(tempSchedule,tempSendTime,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
						writeSystemLog(&currentTime1,0,0,0,2,0,0,0);//sever changed schedule is OK
						messageBox(cmd);
					}else{
						writeSystemLog(&currentTime1,0,0,0,3,0,0,0);//sever changed schedule is Error
						messageErrorSchedule();
					}
					
					break;	
				
				case 4:
				    //Lenh yeu cau doc the nho
					//writeStringUART2("Data date ");
					//sprintf(tBuffer," %02u_%02u_%04u : ",args[0],args[1],2000 + args[2]);
					//writeMStringUART2(tBuffer);
					//writeStringUART2("\r\n");	
					readSD(args[0],args[1],args[2],args[3],args[4],args[5],args[6]);//date month year hour1 minute1 hour2 minute2
					messageBox(cmd);
				    break;	
				case 5:
				    //Lenh Ping giu ket noi 
				    resetModemWatchDog(); 
					break;						
				case 6:	
				    //Reset energy
					resetTotalEnergy();
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareRealtimeStatus(&currentTime1, tBuffer);
					writeMStringUART2(tBuffer);
					prepareLightStatus(getLightStatus(), tBuffer);
					writeMStringUART2(tBuffer);
					prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
					writeMStringUART2(tBuffer);	
					writeUART2Footer();	
					messageBox(cmd);
					writeSystemLog(&currentTime1,0,0,0,0,0,0,2);
					break;
					
				case 7:	//update log time
					if (args[0] < 3) args[0] = 3;//min = 3 --> Minimum ModemTimeOut = 30 seconds
					setModemTimeout(args[0]);
					if (args[1] < 3) args[1] = 3;//min = 3 --> Minimum Write Log Time = 30 seconds				//writeLogTime(args[1]);
					sprintf(tBuffer," %02u  %02u ",args[0],args[1]);
					writeMStringUART2(tBuffer);
					writeStringUART2("\r\n");
					messageBox(cmd);
					break;	
				
				case 8:	
				    // Read System Log
					readSystemLog(args[0],args[1]);//month,Year
					messageBox(cmd);
					break;
				
				case 9://Read current schedule 20/09/2010
					writeUART2Header();
					writeMStringUART2(cmd);
					getSchedule(tempSchedule,tempSendTime);
					prepareScheduleStatus(tempSchedule,tempSendTime,tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();	
					messageBox(cmd);
					break;	
				case 10://0x0A
					//Thay doi trang thai den trang tri 06/12/2010
					manualOnOff1 = 1;
					changeDecorateLightStatus(args[0]);
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareLightStatus(getDecorateLightStatus(),tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();
					messageBox(cmd);
					break;
				
				case 11://0x0B
					// Thay doi schedule den trang tri - 06/12/2010		
					for(i=0;i<7;i++){
						tempSchedule1[i*2][0] = args[i*5];
						tempSchedule1[i*2][1] = args[i*5+1];
						tempSchedule1[i*2+1][0] = args[i*5+2];
						tempSchedule1[i*2+1][1] = args[i*5+3];
						tempSchedule1[i*2][2] = args[i*5+4];
						tempSchedule1[i*2+1][2] = 0;
					}
					if(setSchedule1(tempSchedule1)){
						schedule1Error = 0;
						writeUART2Header();
						writeMStringUART2(cmd);
						prepareSchedule1Status(args,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
						messageBox(cmd);
					}else{
						messageErrorSchedule();
					}
					break;
				case 12://0x0C
					// Yeu cau truyen trang thai den trang tri - 06/12/2010
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareRealtimeStatus(&currentTime1, tBuffer);
					writeMStringUART2(tBuffer);
					prepareLightStatus(getDecorateLightStatus(),tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();
					messageBox(cmd);
					break;
				case 13://0x0D
					// Yeu cau truyen schedule den trang tri - 07/12/2010
					getSchedule1(tempSchedule1);
					for(i=0;i<7;i++){
						args[i*5]   = tempSchedule1[i*2][0];
						args[i*5+1] = tempSchedule1[i*2][1];
						args[i*5+2] = tempSchedule1[i*2+1][0];
						args[i*5+3] = tempSchedule1[i*2+1][1];
						args[i*5+4] = tempSchedule1[i*2][2];
					}
					writeUART2Header();
					writeMStringUART2(cmd);
					prepareSchedule1Status(args,tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();
					messageBox(cmd);
					break;
				case 15:
					resetBoard = 1;
					writeUART2Header();
					writeMStringUART2(cmd);
					writeUART2Footer();
					messageBox(cmd);
					break;
				case 19:
					messageBox(cmd);
					//writeSystemLog(&currentTime1,0,0,0,0,0,2,0);
					break;					
				case 20://0x14
					//writeStringUART1("memslab140000");
					//byteToHexString(args[0],tBuffer);
					//writeMStringUART1(tBuffer);
					//byteToHexString(args[1],tBuffer);
					//writeMStringUART1(tBuffer);
					messageBox(cmd);
					break;	
				case 21://0x15 - check SD Card Status
					TotalSD = checkCapacitor(&FreeSD,&UserSD);
					writeUART2Header();
					writeMStringUART2(cmd);
					dwordToHexString(TotalSD,tBuffer);
					writeMStringUART2(tBuffer);
					dwordToHexString(FreeSD,tBuffer);
					writeMStringUART2(tBuffer);
					writeUART2Footer();
					messageBox(cmd);
					break;
				case 22://0x16 Doc cac thong so calib cho he thong
					//cE1,cE2,cE3,cU1,cU2,cU3,cI1,cI2,cI3,tiCoeff1,tiCoeff2,modemTimeOut
					//4    4   4   2   2   2   2   2   2     2        2         2         --> 30 bytes <-> 60 Hexa characters
					writeUART2Header();
					writeMStringUART2(cmd);
					for(i=0;i<30;i++){
						byteToHexString(args[i],tBuffer);
						writeMStringUART2(tBuffer);
					}
					writeUART2Footer();	
					messageBox(cmd);
					break;		
				case 23://0x17: Doc SystemLog
					readSystemLog(args[0],args[1]);//month, year
					messageBox(cmd);
					break;
				case 24://0x18: Reset Modem tu SMS
					resetModem();
					writeSystemLog(&currentTime1,0,2,0,0,0,0,0);//SMS Reset Modem 
					messageBox(cmd);
					break;
				case 25://0x19: 30-07-2009 lenh SET DIMMING
					//setDimmingPara(args);
					writeUART2Header();
					writeMStringUART2(cmd);
					for(i=0;i<9;i++){
						byteToHexString(args[i],tBuffer);
						writeMStringUART2(tBuffer);
					}
					writeUART2Footer();
					messageBox(cmd);
					break;
				case 26://0x1A: 28-01-2010 lenh nhan File					
					messageBox(cmd);
					break;
				case 27://0x1B: 28-01-2010 truyen File tu SD Card cho Console
					messageBox(cmd);
					break;
				case 30://0x1E : 23-08-2011 Lenh dieu khien LED bo ho
					/*
					modBusDeselect();
					messageBox(cmd);
					WriteArr(2,117,args);
					if(sendLedCmd(&currentTime1,args,tBuffer)){
						//writeMStringUART1(tBuffer);
						writeUART2Header();
						writeMStringUART2(cmd);
						for(i=0;i<tBuffer[0];i++){
							byteToHexString(tBuffer[i+1],strE1);
							writeMStringUART2(strE1);
						}
						writeUART2Footer();						
					}
					_delay_ms(100);
					modBusSelect();*/
					break;
				case 31://0x1F : 13-09-2011 Lenh dieu khien LED bo ho qua SMS
				/*
					modBusDeselect();
					messageBox(cmd);
					WriteArr(2,117,args);
					if(sendLedCmd2(&currentTime1,args,tBuffer)){
						writeUART2Header();
						writeMStringUART2(cmd);
						for(i=0;i<tBuffer[0];i++){
							byteToHexString(tBuffer[i+1],strE1);
							writeMStringUART2(strE1);
						}
						writeUART2Footer();	
						_delay_ms(500);	
						//Gui tin nhan tra ve so dien thoai da gui SMS dieu khien LED					
						writeStringUART2("AT+CMGS=\"");					
						resetTime();
						i = 0;						
						while(args[i] != '*'){//Loc bo noi dung tin nhan
							if (getTime() > 2000){
								return 0;					
							}
							i++;
						}
						i++;
						while(args[i] != '#'){//Send So Dien Thoai
							if (getTime() > 2000){
								return 0;					
							}
							writeUART2(args[i]);
							i++;				
						}
						writeStringUART2("\"");
						writeUART2(0x0D);
						_delay_ms(200);
						writeMStringUART2(tBuffer+1);//Noi dung thiet bi Ledtek tra ve
						writeUART2(0x1A);
						//End gui tin nhan tra ve
					}	
					_delay_ms(50);
					modBusSelect();   */
					break;
				case 32://0x20 : 15-09-2011 Lenh dieu khien LED bo ho 2 qua SMS
				/*
					modBusDeselect();
					messageBox(cmd);
					WriteArr(2,117,args);
					if(sendLedCmd3(&currentTime1,args,tBuffer)){
						writeUART2Header();
						writeMStringUART2(cmd);
						for(i=0;i<tBuffer[0];i++){
							byteToHexString(tBuffer[i+1],strE1);
							writeMStringUART2(strE1);
						}
						writeUART2Footer();	
						_delay_ms(500);	
						//Gui tin nhan tra ve so dien thoai da gui SMS dieu khien LED					
						writeStringUART2("AT+CMGS=\"");					
						resetTime();
						i = 0;						
						while(args[i] != '*'){//Loc bo noi dung tin nhan
							if (getTime() > 2000){
								return 0;					
							}
							i++;
						}
						i++;
						while(args[i] != '#'){//Send So Dien Thoai
							if (getTime() > 2000){
								return 0;					
							}
							writeUART2(args[i]);
							i++;				
						}
						writeStringUART2("\"");
						writeUART2(0x0D);
						_delay_ms(200);
						writeMStringUART2(tBuffer+1);//Noi dung thiet bi Ledtek tra ve
						writeUART2(0x1A);
						//End gui tin nhan tra ve
					}	
					_delay_ms(50);
					modBusSelect();    */
					break;
				case 48://0x30 : Feedback lenh Dim den tung nhom bong den				
					messageBox(cmd);
					break;
				//New Echelon commands 10/08/2011
				case 51://0x33 : Lenh set ID cho Node
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 9;
					//sendEchelonCmd(args);
					for(i=0;i<9;i++){
						RegBytes[i] = args[i];
					}
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					messageBox(cmd);
					break;
				case 52://0x34 lenh Dim bang tay
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 4;
					for(i=0;i<4;i++){
						RegBytes[i] = args[i];
					}
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					messageBox(cmd);
					break;
				case 53://0x35 lenh set thoi gian cho node
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 10;
					for(i=0;i<10;i++){
						RegBytes[i] = args[i];
					}
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					messageBox(cmd);
					break;
				case 54://0x36 lenh set schedule
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 30;
					for(i=0;i<args[3]*3+4;i++){
						RegBytes[i] = args[i];
					}
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					//16/12/2015 transovnu
					//------storage node schedule in SLC-100 eeprom
					if(args[2]==0xFF){	// if broadcast ID of node
						for(i=0;i<MAX_GROUP_NODE;i++){
							eeprom_write_block(args+4,(void*)(NODE_SCHEDULE_X+27*i),27);
						}
					}else{
						eeprom_write_block(args+4,(void*)NODE_SCHEDULE_X+27*(args[2]-1),27);	
					}
					//-----end
					messageBox(cmd);
					break;
				case 55://0x37 Read Unconfigured list
					echelonCMDpending = 1;
					messageBox(cmd);
					break;
				case 56://0x38: Doc trang thai cua Node	
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 3;
					for(i=0;i<3;i++){
						RegBytes[i] = args[i];
					}	
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					messageBox(cmd);
					break;
				case 57://0x39: set Polling Rate
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 5;
					for(i=0;i<5;i++){
						RegBytes[i] = args[i];
					}	
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];
					}
					messageBox(cmd);
					break;
				case 58://0x3A : Read Node's schedule
					echelonCMDpending = 1;
					numOfBytesEchelonCmd = 3;
					for(i=0;i<3;i++){
						RegBytes[i] = args[i];
					}	
					for(i=0;i<23;i++){
						cmdEchelon[i] = cmd[i];		// báo v? modem
					}
					messageBox(cmd);
					break;
				case 60://0x3C : 					
					modBusSelect();
					break;
				case 69:
					//set calibrate parameters
					for(i=0;i<24;i++){
						CL[i] = args[i];
					}
					cE[0] = (((long) CL[0]) << 24) + (((long) CL[1]) << 16) + (((long) CL[2]) << 8) + ((long) CL[3]);
					cE[1] = (((long) CL[4]) << 24) + (((long) CL[5]) << 16) + (((long) CL[6]) << 8) + ((long) CL[7]);
					cE[2] = (((long) CL[8]) << 24) + (((long) CL[9]) << 16) + (((long) CL[10]) << 8) + ((long) CL[11]);
					cU[0] = (((int) CL[12]) << 8) + (((int) CL[13]));
					cU[1] = (((int) CL[14]) << 8) + (((int) CL[15]));
					cU[2] = (((int) CL[16]) << 8) + (((int) CL[17]));
					cI[0] = (((int) CL[18]) << 8) + (((int) CL[19]));
					cI[1] = (((int) CL[20]) << 8) + (((int) CL[21]));
					cI[2] = (((int) CL[22]) << 8) + (((int) CL[23]));
					setCalibrationParam((long *)cE,(int *)cU,(int *)cI);
					//feedBackCmd(cmd,tBuffer);
					messageBox(cmd);
					writeUART2Header();
					writeMStringUART2(cmd);
					writeUART2Footer();
					writeSystemLog(&currentTime1,0,0,0,0,0,2,0);
					break;	
				case 70://0x46: Dat gia tri dien nang cho tung Phase				
					writeNVRAM(args,25,36);//Write NVRAM
					_delay_ms(10);
					initTransducer();
					break;
				case 255://neu co loi clear bo dem nhan
					flushUART2RX();
					break;	
				default :
					break;						
	        } 

			command = readButtonCommand(args);
			clearButtonCmd();
			switch (command){
				case 0://manual On/Off 
					//Truyen ve trang thai + schedule table 				
					changeLightStatus(args[0]);
					if(getScreen()==0) DisplayLightStatus(getLightStatus());
					if(getModemConnected()){
						writeUART2Header();
						threeByteToHexString(0x0FEFE,tBuffer);//feedback On/Off
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						prepareLightStatus(getLightStatus(), tBuffer);
						writeMStringUART2(tBuffer);
						getSchedule(tempSchedule,tempSendTime);
						prepareScheduleStatus(tempSchedule,tempSendTime,tBuffer);
						writeMStringUART2(tBuffer);
						prepareTransducerStatus(voltage, current, phase, tEnergy, voltageDim, tBuffer);
						writeMStringUART2(tBuffer);	
						writeUART2Footer();	
					}
					// Write Log File
					l_to_a(tEnergy.energy[0],strE1);
					l_to_a(tEnergy.energy[1],strE2);
					l_to_a(tEnergy.energy[2],strE3);
					sprintf(tBuffer,"%02u %02u %02u %01u %05u %05u %05u %05u %05u %05u %03u %03u %03u %8s %8s %8s",
							currentTime1.hour,currentTime1.minute,currentTime1.second,getLightStatus(),voltage[0],voltage[1],voltage[2],
							current[0],current[1],current[2],phase[0],phase[1],phase[2],strE1,strE2,strE3);
					writeLog(&currentTime1,tBuffer);
					//if((getModemConnected()==0)&&(countResetModem > 3)){
						//send_SMS(&SERVER_NUMBERS,&tBuffer);
					//}
					//Write System Log
					_delay_ms(50);
					writeSystemLog(&currentTime1,0,0,1,0,0,0,0);
					break;
				case 2://manual set Realtime
					if(getModemConnected()){
						readRealTime(&currentTime1); 
						writeUART2Header();
						threeByteToHexString(0x2FEFE,tBuffer);//feedback set realtime code
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareRealtimeStatus(&currentTime1, tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
					}
					writeSystemLog(&currentTime1,0,0,0,0,1,0,0);
					break;
				case 3://manual set schedule
					if(getModemConnected()){
						getSchedule(tempSchedule,tempSendTime);//Modified 04/03/2009
						writeUART2Header();
						threeByteToHexString(0x3FEFE,tBuffer);//feedback set schedule code
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareScheduleStatus(tempSchedule,tempSendTime,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
					}
					writeSystemLog(&currentTime1,0,0,0,1,0,0,0);//Manual set schedule
					break;
				case 4://manual set Decorate schedule
					if(getModemConnected()){
						getSchedule1(tempSchedule1);
						for(i=0;i<7;i++){
							args[i*5]   = tempSchedule1[i*2][0];
							args[i*5+1] = tempSchedule1[i*2][1];
							args[i*5+2] = tempSchedule1[i*2+1][0];
							args[i*5+3] = tempSchedule1[i*2+1][1];
							args[i*5+4] = tempSchedule1[i*2][2];
						}
						writeUART2Header();
						threeByteToHexString(0xDFEFE,tBuffer);//feedback set schedule code
						writeMStringUART2(tBuffer);
						for(i=0;i<16;i++)
							tBuffer[i] = '0';
						tBuffer[i] = 0;
						writeMStringUART2(tBuffer);
						prepareSchedule1Status(args,tBuffer);
						writeMStringUART2(tBuffer);
						writeUART2Footer();
					}
					//writeSystemLog(&currentTime1,0,0,0,1,0,0,0);//Manual set schedule
					break;
				default :
					break;
			}   
			scanKey(); 
			//Clear Message Box after two seconds 
			if((getScreen() >= 250)&&(getBackLightTime() >= 3)){
				draw_table();
			}
			//test ModBus			
			//modBusSelect();
			//DisplayNumber(1,117,(long)11);
			//setIDnode(nrID,69);
			//dimOneNode(69,69);
			//SetTimeNode(69,&currentTime1);
			if(nMBState == STATE_MB_FRM_CHECK){
				//DisplayNumber(26,117,(long)22);
				DisplayNumber(101,117,(long)RegBytes[2]);
				DisplayNumber(126,117,(long)RegBytes[3]);
				mb_crc (pInBuff,nInBuffCount - 2);
				if(crc == (((unsigned int)pInBuff[nInBuffCount-2]<<8)+pInBuff[nInBuffCount-1])){
					//DisplayNumber(1,117,(long)pInBuff[1]);
					//DisplayNumber(26,117,(long)pInBuff[2]);
					//DisplayNumber(51,117,(long)pInBuff[3]);
					//DisplayNumber(76,117,(long)pInBuff[5]);
					nMBState = STATE_MB_IDLE;
					if(pInBuff[1] == 3){//truyen Command tu PC -> SLC100 -> SmartServer -> Node
						//pInBuff[0]:			     Dia chi Slave
						//pInBuff[1]: 			     Ma lenh ModBus
						//pInBuff[2],pInBuff[3] H,L: Dia chi thanh ghi bat dau Read
						//pInBuff[4],pInBuff[5] H,L: So luong thanh ghi can Read
						//ReplyReadHoldingRegister(pInBuff[0],pInBuff[5],(pInBuff[2]*256+pInBuff[3])/2,outRegs);
						ReplyReadHoldingRegister(pInBuff[0],pInBuff[5],(((unsigned int)pInBuff[2]<<8)+pInBuff[3]),RegBytes);
						if(echelonCMDpending == 1){// Truyen lenh tu SLC100 -> SmartServer
							echelonCMDpending = 0;
							writeUART2Header();	   // Bao truyen thanh cong
							writeMStringUART2(cmdEchelon);
							for(i=0;i<numOfBytesEchelonCmd;i++){
								byteToHexString((char)RegBytes[i],tBuffer);
								writeMStringUART2(tBuffer);
							}
							writeUART2Footer();	
						}
					}
					if(pInBuff[1] == 16){//truyen Data tu Node -> SmartServer -> SLC100 -> PC
						for(i=0;i<2*pInBuff[5];i++){
							//pInBuff[0]:			     Dia chi Slave
							//pInBuff[1]: 			     Ma lenh ModBus
							//pInBuff[2],pInBuff[3] H,L: Dia chi thanh ghi bat dau Write
							//pInBuff[4],pInBuff[5] H,L: So luong thanh ghi can Write
							//pInBuff[6]:				 Byte Count
							//pInBuff[7],pInBuff[8] H,L  Register Value 1
							// .................
							//pInBuff[x],pInBuff[x+1] H,L  Register Value N	
							//outRegs[(pInBuff[2]*256+pInBuff[3])/2+i] = pInBuff[7+2*i]*256+pInBuff[8+2*i];
							RegBytes[(((unsigned int)pInBuff[2]<<8)+pInBuff[3])+i] = pInBuff[7+i];//Nhan toan bo Data tu SmartServer va
																				   //ghi vao RegBytes
						}
						ReplyWriteMultipleRegister(pInBuff[0],((unsigned int)pInBuff[4]<<8)+pInBuff[5],((unsigned int)pInBuff[2]<<8)+pInBuff[3]);
						DisplayNumber(1,117,(long)RegBytes[64]);
						DisplayNumber(26,117,(long)RegBytes[96]);
						DisplayNumber(51,117,(long)RegBytes[128]);
						//DisplayNumber(76,117,(long)RegBytes[99]);
						/*
						if(getModemConnected()){
							for(i = 1;i<100;i++){
								if(((RegBytes[32+32*i] == 3)||(RegBytes[32+32*i] == 255))&&(RegBytes[33+32*i] == 1)){
									writeUART2Header();
									threeByteToHexString(0x38FEFE,tBuffer);//truyen trang thai Node
									writeMStringUART2(tBuffer);
									for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
										tBuffer[i2] = '0';
									tBuffer[i2] = 0;
									writeMStringUART2(tBuffer);
									for(i2=32+32*i;i2<64+32*i;i2++){
										byteToHexString((char)RegBytes[i2],tBuffer);
										writeMStringUART2(tBuffer);
									}
									writeUART2Footer();	
									RegBytes[32+32*i] = 0;//clear Flag
									_delay_ms(50);
								}//End if(((RegBytes[32+32*i] == 3)...
								if(((RegBytes[32+32*i] == 1)||(RegBytes[32+32*i] == 3))&&(RegBytes[33+32*i] == 3)){//truyen node schedule
									writeUART2Header();
									threeByteToHexString(0x3AFEFE,tBuffer);//truyen node schedule
									writeMStringUART2(tBuffer);
									for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
										tBuffer[i2] = '0';
									tBuffer[i2] = 0;
									writeMStringUART2(tBuffer);
									for(i2=32+32*i;i2<64+32*i;i2++){
										byteToHexString((char)RegBytes[i2],tBuffer);
										writeMStringUART2(tBuffer);
									}
									writeUART2Footer();	
									RegBytes[32+32*i] = 0;//clear Flag
									_delay_ms(50);
								}//End if(((RegBytes[32+32*i] == 3)...
							}//End for(i = 1;i<100;i++)...
						}//End if(getModemConnected())...
						*/
					}//End if(pInBuff[1] == 16)...
				}
				else{
					_delay_ms(200);
					nMBState = STATE_MB_IDLE;
				}
			}//End if(nMBState == STATE_MB_FRM_CHECK)...
			if(nMBState == STATE_MB_FRM_NOK){
				WriteArr(50,117,"Frame Error");
				_delay_ms(200);
				nMBState = STATE_MB_IDLE;
			}//End if(nMBState == STATE_MB_FRM_NOK)...
			/*
			if(getModemConnected()){
				//for(i = 1;i<100;i++){
					if(((RegBytes[32+32*scanNode] == 3)||(RegBytes[32+32*scanNode] == 255))&&(RegBytes[33+32*scanNode] == 1)){
						writeUART2Header();
						threeByteToHexString(0x38FEFE,tBuffer);//truyen trang thai Node
						writeMStringUART2(tBuffer);
						for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
							tBuffer[i2] = '0';
						tBuffer[i2] = 0;
						writeMStringUART2(tBuffer);
						for(i2=32+32*scanNode;i2<60+32*scanNode;i2++){
							byteToHexString((char)RegBytes[i2],tBuffer);
							writeMStringUART2(tBuffer);
						}
						writeUART2Footer();	
						RegBytes[32+32*scanNode] = 0;//clear Flag
						_delay_ms(200);
					}//End if(((RegBytes[32+32*i] == 3)...
					if(((RegBytes[32+32*scanNode] == 1)||(RegBytes[32+32*scanNode] == 3))&&(RegBytes[33+32*scanNode] == 3)){//truyen node schedule
						writeUART2Header();
						threeByteToHexString(0x3AFEFE,tBuffer);//truyen node schedule
						writeMStringUART2(tBuffer);
						for(i2=0;i2<16;i2++)//16 Bytes Blanks '0'
							tBuffer[i2] = '0';
						tBuffer[i2] = 0;
						writeMStringUART2(tBuffer);
						for(i2=32+32*scanNode;i2<64+32*scanNode;i2++){
							byteToHexString((char)RegBytes[i2],tBuffer);
							writeMStringUART2(tBuffer);
						}
						writeUART2Footer();	
						RegBytes[32+32*scanNode] = 0;//clear Flag
						_delay_ms(200);
					}//End if(((RegBytes[32+32*i] == 3)...
					scanNode++;
					if(scanNode > MAX_NODE){
						scanNode = 1;
					}
				//}//End for(i = 1;i<100;i++)...
			}//End if(getModemConnected())...
			*/
	}//end while(1)

#endif
return 0;

}//End int main();
