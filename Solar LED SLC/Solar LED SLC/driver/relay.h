/*
 * relay.h
 *
 * Created: 19/07/2013 2:25:30 CH
 *  Author: SCEC
 */ 


#ifndef RELAY_H_
#define RELAY_H_

#define RL_SEL_SOURCE		5
#define PORT_RL_SEL_SOURCE	PORTE
#define DDR_RL_SEL_SOURCE	DDRE
#define SEL_BATTERY			1
#define SEL_MAIN			0

#define RL_ENB_PSU			7
#define PORT_ENB_PSU		PORTC
#define DDR_ENB_PSU			DDRC
#define PSU_OFF				0
#define PSU_ON				1

#define RL_SW_LAMP			2
#define PORT_SW_LAMP		PORTE
#define DDR_SW_LAMP			DDRE
#define LAMP_OFF			0
#define LAMP_ON				50

extern unsigned char psu_status;
extern unsigned char sel_source;

void init_sel_source_pw(void);
void sel_source_pw(unsigned char);

void init_enb_psu();
void sw_psu(unsigned char);

void changeLampState(unsigned char	_state);
unsigned char getLampState();
void taskManageLampState(void);
void init_sw_lamp(void);
void switch_Lamp(unsigned char _val);
#endif /* RELAY_H_ */