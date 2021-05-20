//  Author: Vatsal Asitkumar Joshi
//  Date: Oct 2nd, 2019
//  This code is a header file for the functions provided in termInput.c
//
//  "If you are done writing the code, now is a good time to debug it."
//

#ifndef __TERMINAL_INPUT__
#define __TERMINAL_INPUT__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "bitDefines.h"

#define MAX_CHARS 80
#define MAX_FIELDS 6

struct termInput
{
	char str[MAX_CHARS+1];
	uint8_t pos[MAX_FIELDS], argCount;
};

extern void putsUart0(char* str);
void parseString(struct termInput* userInput);
char* getArgString(struct termInput* userInput, uint8_t argNumber);

#endif
