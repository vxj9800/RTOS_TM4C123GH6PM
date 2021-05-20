//  Author: Vatsal Asitkumar Joshi
//  Date: Oct 2nd, 2019
//  This code is a header file for the functions provided in uart0.c
//
//  "If you are done writing the code, now is a good time to debug it."
//

#ifndef __UART_DEFINED__
#define __UART_DEFINED__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "bitDefines.h"
#include "termInput.h"
#include "kernel.h"

#define initUART0()																																			\
{																																							\
	setRegisterBit(SYSCTL_RCGC2_R, SYSCTL_RCGC2_GPIOA);																										\
	setRegisterBit(SYSCTL_RCGCUART_R, SYSCTL_RCGCUART_R0);										/*Activate UART0*/											\
																																							\
	setRegisterBit(GPIO_PORTA_DIR_R, bit1);														/*Enable output on UART0 TX pin*/							\
	setRegisterBit(GPIO_PORTA_DEN_R, bit0 | bit1);												/*Enable digital I/O on UART0 pins*/						\
	setRegisterBit(GPIO_PORTA_AFSEL_R, bit0 | bit1);											/*Use peripheral to drive PA0, PA1 as UART0*/				\
	clrRegisterBit(GPIO_PORTA_PCTL_R, bit0 | bit1 | bit2 | bit3 | bit4 | bit5 | bit6 | bit7);	/*Set fields for PA0 and PA1 to zero*/						\
	setRegisterBit(GPIO_PORTA_PCTL_R, bit0 | bit4);												/*Assign UART signals to pin PA0 and PA1*/					\
																																							\
	clrRegisterBit(UART0_CTL_R, UART_CTL_UARTEN);												/*Turn-off UART0 to allow safe programming*/				\
	setRegisterVal(UART0_IBRD_R, 21);															/*BRD = BRDI + BRDF = Clk / (16 * Baud Rate)*/				\
	setRegisterVal(UART0_FBRD_R, 45);															/*BRDI = floor(BRD) and BRDF = floor((BRD-BRDI)*64+0.5)*/	\
	setRegisterBit(UART0_LCRH_R, UART_LCRH_WLEN_8 | UART_LCRH_FEN);								/*Configure for 8N1 w/ 16-level FIFO*/						\
	clrRegisterBit(UART0_CC_R, bit0 | bit1 | bit2 | bit3);										/*Set the clock source to zero (For safety)*/				\
	setRegisterBit(UART0_CTL_R, UART_CTL_UARTEN);												/*Turn on UART0*/											\
}

extern void yield();
void putcUart0(char c);
void putsUart0(char* str);
char getcUart0();
void getsUart0(struct termInput* userInput);
void itoa(uint32_t iVal, char* str, uint8_t strLength);
void getPercentage(uint32_t iVal, char* str, uint8_t strLength);

#endif
