/*
 * screen.h
 *
 * Created: 30/10/2013 3:25:58 CH
 *  Author: SCEC
 */ 


#ifndef SCREEN_H_
#define SCREEN_H_

#define MAINOBJ 0
#define MENUOBJ	1
#define MCTROBJ 2
#define SCHEOBJ	3
#define RLTCOBJ 4
#define SETTOBJ 5
#define POWEOBJ	6
#define TAS0OBJ	7
#define TAS1OBJ	8
#define TAS2OBJ	9
#define TAS3OBJ	10
#define TAS4OBJ	11
#define TAS5OBJ	12
#define TAS6OBJ	13
#define TAS7OBJ	14
#define TAS8OBJ	15
#define SECOOBJ	16
#define MINUOBJ	17
#define HOUROBJ	18
#define DAY_OBJ	19
#define DATEOBJ	20
#define MONTOBJ	21
#define YEAROBJ	22
#define ID__OBJ	23
#define NRIDOBJ	24
#define V124OBJ	25
#define LOWVOBJ	26

#define TMM0OBJ	27
#define TMM1OBJ	28
#define TMM2OBJ	29
#define TMM3OBJ	30
#define TMM4OBJ	31
#define TMM5OBJ	32
#define TMM6OBJ	33
#define TMM7OBJ	34
#define TMM8OBJ	35

#define TPW0OBJ	36
#define TPW1OBJ	37
#define TPW2OBJ	38
#define TPW3OBJ	39
#define TPW4OBJ	40
#define TPW5OBJ	41
#define TPW6OBJ	42
#define TPW7OBJ	43
#define TPW8OBJ	44
#define TERMOBJ 45
#define CHGVOBJ	46
#define INFOOBJ	47
#define CALIOBJ	48
#define CHG1OBJ	49
#define CHG2OBJ	50
#define ILAMOBJ	51
#define PTX1OBJ	52
#define PTY1OBJ	53
#define PTX2OBJ	54
#define PTY2OBJ	55
#define UBINOBJ	56
#define UB01OBJ	57
#define ECHGOBJ	58

#define MAINSCR	0
#define MENUSCR	1
#define MCTRSCR 2
#define SCHESCR	3
#define RLTCSCR	4
#define SETTSCR	5
#define CALBSCR 6
#define CHG1SCR 7
#define CHG2SCR 8
#define ILAMSCR 9
#define UBINSCR	10
#define UB01SCR	11


#define TERMSCR 20
#define INFOSCR	21

#define V12BAT	0
#define V24BAT	1

#define SIZE_TEMPTABLE	27

struct _HISTORYSCR{
	unsigned char historyQuece[10];
	unsigned char idxHistory;
};

typedef struct _OBJPAR1{
	unsigned char objType;
	char * objStr;
}menu_type;

typedef struct _OBJPAR2{
	unsigned char objType;
	char * objStr;
	void * varAddr;
	unsigned int minVar;
	unsigned int maxVar;
}int_type;

typedef struct _OBJPAR4{
	unsigned char objType;
	char * objStr;
	void * varAddr0;
	void * varAddr1;
	void * varAddr2;
}int3_type;

typedef struct _OBJPAR3{
	unsigned char objType;
	char * objStr;
	void * varAddr;
	unsigned int minVar;
	unsigned int maxVar;
	void * lookupVar;
}lookup_type;

extern unsigned int eechg1x1,eechg1y1,eechg1x2,eechg1y2;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
extern unsigned int eechg2x1,eechg2y1,eechg2x2,eechg2y2;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
extern unsigned int eeilampx1,eeilampy1,eeilampx2,eeilampy2;	// calib point A(x1,y1) B(x2,y2), y = kx + a;
extern unsigned int eeUb1x1, eeUb1y1,eeUbinx1, eeUbiny1;

extern struct _HISTORYSCR historyScreen;

unsigned char getCurrentScreen(void);
void loadPar_Scr(unsigned char _IdxScr);
void changeCurrentScreen(unsigned char _IdxScr);
void toPreviousScreen(void);
void toMainScreen(void);
void setChangedInScr(void);
unsigned char getChangedInScr(void);

void incValSelObj(int _val);
void desValSelObj(int _val);

unsigned char getSelectedObj();
void selectMovUp(void);
void selectMovDw(void);

void saveVarInScr(unsigned char _idxSCR);
//void desIdxSelObj(void);
//void incIdxSelObj(void);

void printSCREEN (unsigned char _idxSCR);
void * getDayStr(unsigned char _dayofWeek);
unsigned char getbatType(void);
unsigned int getlowV(void);
unsigned int getchgV(void);
unsigned int getEchg(void);
#endif /* SCREEN_H_ */