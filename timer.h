//  Author: Vatsal Asitkumar Joshi
//  Date: Oct 28th, 2019
//  This code is a header file for the functions provided in timer.c
//
//  "If you are done writing the code, now is a good time to debug it."
//

#ifndef __TIMER_DEFINED__
#define __TIMER_DEFINED__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "bitDefines.h"

#define initTimer0()												            \
{																		        \
    setRegisterBit(SYSCTL_RCGCTIMER_R, SYSCTL_RCGCTIMER_R0);			        \
																		        \
    clrRegisterBit(TIMER0_CTL_R, TIMER_CTL_TAEN | TIMER_CTL_TBEN);		        \
    setRegisterBit(TIMER0_CFG_R, TIMER_CFG_32_BIT_TIMER);				        \
    setRegisterBit(TIMER0_TAMR_R, TIMER_TAMR_TAMR_PERIOD | TIMER_TAMR_TACDIR);	\
    setRegisterVal(TIMER0_TAILR_R, 0xFFFFFFFF);    					            \
    setRegisterBit(TIMER0_CTL_R, TIMER_CTL_TAEN);						        \
}

#define initTimer1()													\
{																		\
    setRegisterBit(SYSCTL_RCGCTIMER_R, SYSCTL_RCGCTIMER_R1);			\
																		\
    clrRegisterBit(TIMER1_CTL_R, TIMER_CTL_TAEN | TIMER_CTL_TBEN);		\
    setRegisterBit(TIMER1_CFG_R, TIMER_CFG_32_BIT_TIMER);				\
    setRegisterBit(TIMER1_TAMR_R, TIMER_TAMR_TAMR_PERIOD);				\
    setRegisterVal(TIMER1_TAILR_R, 40000); /*400*/						\
    setRegisterBit(TIMER1_IMR_R, TIMER_IMR_TATOIM);						\
    setRegisterBit(NVIC_EN0_R, 1 << (INT_TIMER1A - 16));				\
    setRegisterBit(TIMER1_CTL_R, TIMER_CTL_TAEN);						\
}

#define enTimer0()                                                              \
{                                                                               \
    setRegisterVal(TIMER0_TAV_R, 0);                                            \
    setRegisterBit(TIMER0_CTL_R, TIMER_CTL_TAEN);						        \
}

#define disTimer0() TIMER0_TAR_R; clrRegisterBit(TIMER0_CTL_R, TIMER_CTL_TAEN | TIMER_CTL_TBEN);

#endif
