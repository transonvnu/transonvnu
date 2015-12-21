#ifndef _LCD_INCLUDE_
#define _LCD_INCLUDE_

#define MAX_GROUP_NODE	4

void WriteArr(char x,char y, char* Arr);
void DisplayNumber(char x,char y, long Number);
void scanKey(void);
unsigned char readButtonCommand(char * args);
void clearButtonCmd(void);
void messageBox(char * cmd);
void messageErrorSchedule(void);
void messageOK(void);
void messageERROR(void);
void DisplayE(EnergySruct tEnergy);
void DisplayU_I_P(unsigned int * voltage,unsigned int * current,unsigned char * phase);
void DisplaySchedule(signed char schedule[5][5]);
void DisplayLightStatus(unsigned char status);
void DisplayDecorateLightStatus(unsigned char status);
void DisplayTime(TimeStruct * time);
unsigned char getScreen(void);
void initKeyboard(void);
void changeSchedule(char * command);
void changeDecorateLightSchedule(char * command);
void changeStreetLightSchedule(char * command);
void convertDay(char day,char * buffer);
void changeTimeDate(char * command);
void modemSettings(char * command);
void calibADE(void);
void calibrationSCR(char * command);
void lightControl(char * command);
void menu(char * command);
unsigned char readButtonCommand(char * args);
void clearButtonCmd(void);
void draw_table(void);
void nodesSchedule(char * command);
void controlNodes(char * command);
void setNodesSchedule(char * command);
void deviceInfor(char * command);

#endif
