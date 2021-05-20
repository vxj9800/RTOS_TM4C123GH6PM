//  Author: Vatsal Asitkumar Joshi
//  Date: Oct 2nd, 2019
//  This code is a set of functions to work with UART0.
//
//  "If you are done writing the code, now is a good time to debug it."
//

#include "uart0.h"

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);                // wait if uart0 tx fifo full
//		yield();
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    uint8_t i;
    for (i = 0; i < strlen(str); i++)
      putcUart0(str[i]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{
    while (UART0_FR_R & UART_FR_RXFE)                // wait if uart0 rx fifo empty
		yield();
    return UART0_DR_R & 0xFF;                        // get character from fifo
}

// Blocking function that writes a string when the UART buffer is not full

void getsUart0(struct termInput* userInput)
{
	uint8_t count = 0;
	char c;

	while (count<MAX_CHARS)
	{
		c = getcUart0();
		if (c==8 || c==127)
		{
			if (count==0)
				continue;
			count--;
		}
		else if (c==13 || c==10)
		{
			userInput->str[count] = '\0';
			break;
		}
		else if (c >= 32)
		{
			userInput->str[count++] = c;
		}
		else
		{
			continue;
		}
	}
	parseString(userInput);
	putsUart0("\r\n");
}

uint32_t intPow (uint32_t x, uint32_t p)
{
	uint32_t i = 1;
	for (uint32_t j = 0; j < p; j++) 
		i *= x;
	return i;
}

void itoa(uint32_t iVal, char* str, uint8_t strLength)
{
	uint8_t p = 255;
	while (iVal/intPow(10,++p) != 0);
	if (p)
	{
		str[p] = '\0';
		for(uint8_t i = 1; i<strLength-1; ++i)
		{
			str[p-i] = '0' + (iVal % 10);
			iVal /= 10;
			if (i==p)
			{
				strLength = p;
				break;
			}
		}
	}
	else
	{
		str[0] = '0'; str[1] = '\0';
	}
	str[strLength] = '\0';
}

void getPercentage(uint32_t iVal, char* str, uint8_t strLength)
{
	str[4] = iVal%10 + '0'; iVal /= 10;
	str[3] = iVal%10 + '0'; iVal /= 10; str[2] = '.';
	str[1] = iVal%10 + '0'; iVal /= 10;
	str[0] = iVal%10 + '0'; iVal /= 10; str[5] = '\0';
}
