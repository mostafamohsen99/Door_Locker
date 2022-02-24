/*
 * timer.h
 *
 *  Created on: Aug 31, 2021
 *      Author: dell
 */

//#ifndef TIMER_H_
//#define TIMER_H_
//#include "std_types.h"
//#include "common_macros.h"
//#include "micro_config.h"
//typedef enum{
//	NORMAL,PWM,CTC,F_PWM
//} TIMER0_MODE;
//typedef enum
//{
//	None,OC0_Toggle,OC0_Clear,OC0_Set
//} TIMER0_CTC;
//typedef enum
//{
//	No,Inverting,Non_Inverting
//}TIMER0_PWM;
//typedef enum
//{
//	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
//}TIMER0_CLOCK;
//typedef struct
//{
// TIMER0_MODE mode;
// TIMER0_CTC ctc;
// TIMER0_PWM pwm;
// TIMER0_CLOCK clock;
// uint8 OCR;
//}TIMER0_CONFIGTYPE;
//void Timer0_init(TIMER0_CONFIGTYPE *config);
//void ISR_COUNT(void);
//void Timer_set_Callback(void (*ptr)(void));
//void Timer0_deinit(void);

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"


typedef enum{
	NORMAL_MODE,PWM_PC,CTC,F_PWM
}TIMER0_MODE;

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer0_Clock;

typedef struct{
	TIMER0_MODE mode;
	Timer0_Clock clock;
	uint8 OCR;
}Timer0_ConfigType;

//prototypes
void Timer0_init(const Timer0_ConfigType * Config_Ptr);
void ISR_COUNT(void);

void Set_callBack(void(*a_ptr)(void));

void timer0_DeInit(void);


#endif





