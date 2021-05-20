// RTOS Framework - Spring 2020
// J Losh

// Student Name:
// TO DO: Add your name on this line.  Do not include your ID number in the file.

// Add xx_ prefix to all files in your project
// xx_rtos.c
// xx_tm4c123gh6pm_startup_ccs.c
// xx_other files (except uart0.x and wait.x)
// (xx is a unique number that will be issued in class)
// Please do not change any function name in this code or the thread priorities

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// 6 Pushbuttons and 5 LEDs, UART
// LEDs on these pins:
// Blue:   PF2 (on-board)
// Red:    PA2
// Orange: PA3
// Yellow: PA4
// Green:  PE0
// PBs on these pins
// PB0:    PC4
// PB1:    PC5
// PB2:    PC6
// PB3:    PC7
// PB4:    PD6
// PB5:    PD7
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bitDefines.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "timer.h"
#include "kernel.h"

// LED and PB defines
#define BLUE_MASK       bit2
#define RED_MASK        bit2
#define GREEN_MASK      bit0
#define YELLOW_MASK     bit4
#define ORANGE_MASK     bit3
#define PB0_MASK        bit4
#define PB1_MASK        bit5
#define PB2_MASK        bit6
#define PB3_MASK        bit7
#define PB4_MASK        bit6
#define PB5_MASK        bit7

// BitBanded adresses of LEDs and PBs
#define BLUE_LED		peripheralBitBand(GPIO_PORTF_DATA_R, 2) // on-board blue LED
#define RED_LED			peripheralBitBand(GPIO_PORTA_DATA_R, 2) // off-board red LED
#define GREEN_LED		peripheralBitBand(GPIO_PORTE_DATA_R, 0) // off-board green LED
#define YELLOW_LED		peripheralBitBand(GPIO_PORTA_DATA_R, 4) // off-board yellow LED
#define ORANGE_LED		peripheralBitBand(GPIO_PORTA_DATA_R, 3) // off-board orange LED
#define PUSHBUTTON0		peripheralBitBand(GPIO_PORTC_DATA_R, 4) // off-board push button 0
#define PUSHBUTTON1		peripheralBitBand(GPIO_PORTC_DATA_R, 5) // off-board push button 1
#define PUSHBUTTON2		peripheralBitBand(GPIO_PORTC_DATA_R, 6) // off-board push button 2
#define PUSHBUTTON3		peripheralBitBand(GPIO_PORTC_DATA_R, 7) // off-board push button 3
#define PUSHBUTTON4		peripheralBitBand(GPIO_PORTD_DATA_R, 6) // off-board push button 4
#define PUSHBUTTON5		peripheralBitBand(GPIO_PORTD_DATA_R, 7) // off-board push button 5



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
// Add initialization for blue, orange, red, green, and yellow LEDs
//       6 pushbuttons, and uart
void initHw()
{
	setRegisterVal(SYSCTL_RCC_R, SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S));
	setRegisterBit(SYSCTL_RCGC2_R, SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF);										// SYSCTL_RCGC2_GPIOF -> Enable GPIO port F peripherals

	// Activate LEDs
	setRegisterBit(GPIO_PORTF_DIR_R, BLUE_MASK);		// Sets PF2(BLUE LED) to be an output
	setRegisterBit(GPIO_PORTF_DEN_R, BLUE_MASK);		// Sets PF2(BLUE LED) to be a digital I/O
	setRegisterBit(GPIO_PORTA_DIR_R, RED_MASK | ORANGE_MASK | YELLOW_MASK);		// Sets PA2, PA3, PA4(red, orange and yellow LED) to be an output
	setRegisterBit(GPIO_PORTA_DEN_R, RED_MASK | ORANGE_MASK | YELLOW_MASK);		// Sets PA2, PA3, PA4(red, orange and yellow LED) to be a digital I/O
	setRegisterBit(GPIO_PORTE_DIR_R, GREEN_MASK);		// Sets PE0(Green LED) to be an output
	setRegisterBit(GPIO_PORTE_DEN_R, GREEN_MASK);		// Sets PE0(Green LED) to be a digital I/O

	// Activate PBs
	setRegisterBit(GPIO_PORTC_PUR_R, PB0_MASK | PB1_MASK | PB2_MASK | PB3_MASK);
	setRegisterBit(GPIO_PORTC_DEN_R, PB0_MASK | PB1_MASK | PB2_MASK | PB3_MASK);
	setRegisterBit(GPIO_PORTD_PUR_R, PB4_MASK);
	setRegisterBit(GPIO_PORTD_DEN_R, PB4_MASK);
	setRegisterVal(GPIO_PORTD_LOCK_R, GPIO_LOCK_KEY);
	setRegisterBit(GPIO_PORTD_CR_R, PB5_MASK);
	setRegisterVal(GPIO_PORTD_LOCK_R, GPIO_LOCK_M);
	setRegisterBit(GPIO_PORTD_PUR_R, PB5_MASK);
	setRegisterBit(GPIO_PORTD_DEN_R, PB5_MASK);

	// Activate SysTick
	setRegisterBit(NVIC_ST_CTRL_R, NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN);
	setRegisterVal(NVIC_ST_RELOAD_R, 40000);

	// Initialize UART
	initUART0();

	// Initialize Timers
	initTimer0();
	initTimer1();
	clrRegisterBit(TIMER1_CTL_R, TIMER_CTL_TAEN);
}

// add code to return a value from 0-63 indicating which of 6 PBs are pressed
uint8_t readPbs()
{
	return (!PUSHBUTTON5 << 5) | (!PUSHBUTTON4 << 4) | (!PUSHBUTTON3 << 3) | (!PUSHBUTTON2 << 2) | (!PUSHBUTTON1 << 1) | !PUSHBUTTON0;
}

//-----------------------------------------------------------------------------
// YOUR UNIQUE CODE
// add any custom code in this space
//-----------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// Task functions
// ------------------------------------------------------------------------------
void idle()
{
	while(true)
	{
		ORANGE_LED = 1;
		waitMicrosecond(1000);
		ORANGE_LED = 0;
		yield();
	}
}

void flash4Hz()
{
	while(true)
	{
		GREEN_LED ^= 1;
		sleep(125);
	}
}

void oneshot()
{
	while(true)
	{
		wait(flashReq);
		YELLOW_LED = 1;
		sleep(1000);
		YELLOW_LED = 0;
	}
}

void partOfLengthyFn()
{
	// represent some lengthy operation
	waitMicrosecond(990);
	// give another process a chance to run
	yield();
}

void lengthyFn()
{
	uint16_t i;
	while(true)
	{
		wait(resource);
		for (i = 0; i < 5000; i++)
		{
			partOfLengthyFn();
		}
		RED_LED ^= 1;
		post(resource);
	}
}

void readKeys()
{
	uint8_t buttons;
	while(true)
	{
		wait(keyReleased);
		buttons = 0;
		while (buttons == 0)
		{
			buttons = readPbs();
			yield();
		}
		post(keyPressed);
		if ((buttons & 1) != 0)
		{
			YELLOW_LED ^= 1;
			RED_LED = 1;
		}
		if ((buttons & 2) != 0)
		{
			post(flashReq);
			RED_LED = 0;
		}
		if ((buttons & 4) != 0)
		{
			restartThread("Flash4Hz");
		}
		if ((buttons & 8) != 0)
		{
			destroyThread(flash4Hz);
		}
		if ((buttons & 16) != 0)
		{
			setThreadPriority(lengthyFn, 4);
		}
		yield();
	}
}

void debounce()
{
	uint8_t count;
	while(true)
	{
		wait(keyPressed);
		count = 10;
		while (count != 0)
		{
			sleep(10);
			if (readPbs() == 0)
				count--;
			else
				count = 10;
		}
		post(keyReleased);
	}
}

//void uncooperative()
//{
//	while(true)
//	{
//		while (readPbs() == 32)
//		{
//		}
//		yield();
//	}
//}

void errant()
{
    uint32_t* p = (uint32_t*)0x20007FFC;
    while(true)
    {
        while (readPbs() == 32)
        {
            *p = 0;
        }
        yield();
    }
}


void important()
{
	while(true)
	{
		// wait(resource);
		piWait(resource,lengthyFn);
		BLUE_LED = 1;
		sleep(1000);
		BLUE_LED = 0;
		post(resource);
	}
}

// add processing for the shell commands through the UART here
//       your solution should not use C library calls for strings, as the stack will be too large
void shell()
{
	struct termInput uartInput;
	putsUart0("\r\n\r\n"); sleep(100);
	putsUart0("==============================================================>\r\n"); sleep(100);
	putsUart0("EE 6314 Advanced Embedded Microcontroller Systems\r\n\r\n"); sleep(100);
	putsUart0("Project - 2: Real Time Operating System with MPU\r\n\r\n"); sleep(100);
	putsUart0("Author: Jaynil Vasantbhai Patel and Vatsal Asitkumar Joshi\r\n"); sleep(100);
	putsUart0("==============================================================>\r\n"); sleep(100);

	while (true)
	{
		putsUart0("\r\n>");
		getsUart0(&uartInput);
		char* command = getArgString(&uartInput,0);
		if (!strcmp(command,"reboot") && uartInput.argCount)
		{
			reboot();
			while(1);
		}

		else if (!strcmp(command,"redled") && 1<=uartInput.argCount)
		{
			RED_LED = atoi(getArgString(&uartInput,1));
		}

		else if (!strcmp(command,"blueled") && 1<=uartInput.argCount)
		{
			BLUE_LED = atoi(getArgString(&uartInput,1));
		}

		else if (!strcmp(command,"greenled") && 1<=uartInput.argCount)
		{
			GREEN_LED = atoi(getArgString(&uartInput,1));
		}

		else if (!strcmp(command,"ps") && 1<=uartInput.argCount)
		{
			struct psInfo psData[MAX_TASKS];
			processStatus(psData);
			uint64_t totalProcessTime = 0;
			for (uint8_t i=0; i<MAX_TASKS; ++i)
				totalProcessTime += psData[i].processTime;
			char str[16];
			putsUart0("\tPID\tName\t\tPriority\tState\t\t%CPU Time\r\n");
			putsUart0("\t-----------------------------------------------------------------\r\n");
			for (uint8_t i = 0; i<MAX_TASKS && psData[i].pid!=0; ++i)
			{
				putsUart0("\t"); itoa(psData[i].pid, str, 15); putsUart0(str); putcUart0('\t');
				putsUart0(psData[i].name); putcUart0('\t');
				itoa(psData[i].priority, str, 15); putsUart0(str); putsUart0("\t\t");
				switch (psData[i].state)
				{
				case STATE_BLOCKED:
					putsUart0("BLOCKED\t\t"); break;
				case STATE_READY:
					putsUart0("READY\t\t"); break;
				case STATE_UNRUN:
					putsUart0("UNRUN\t\t"); break;
				case STATE_DELAYED:
					putsUart0("DELAYED\t\t"); break;
				default:
					putsUart0("INVALID\t\t"); break;
				}
				getPercentage((psData[i].processTime*10000/totalProcessTime), str, 15); putsUart0(str); putcUart0('\t');
				putsUart0("\r\n");
			}
		}

		else if (!strcmp(command,"ipcs") && 1<=uartInput.argCount)
		{
			struct ipcsInfo ipcsData[MAX_SEMAPHORES];
			uint8_t semaCount;
			interProcessCommStat(ipcsData,&semaCount); char str[16];
			putsUart0("\tName\t\tCount\tRunning\t\tBlocked\r\n");
        	putsUart0("\t------------------------------------------------------------------\r\n");
        	for (uint8_t i = 0; i<semaCount; ++i)
        	{
				putsUart0("\t"); putsUart0(ipcsData[i].semaName); putsUart0("\t");
				itoa(ipcsData[i].count, str, 15); putsUart0(str); putsUart0("\t");
				putsUart0(ipcsData[i].running);putsUart0("\t");
				putsUart0(ipcsData[i].blocked);putsUart0("\r\n");
			}
		}

		else if (!strcmp(command,"kill") && 2<=uartInput.argCount)
		{
			_fn fn = (_fn)atoi(getArgString(&uartInput,1));
			destroyThread(fn);
		}

		else if (!strcmp(getArgString(&uartInput,1),"&") && 2<=uartInput.argCount)
		{
			restartThread(command);
			putsUart0(command);
		}

		else if (!strcmp(command,"pidof") && 2<=uartInput.argCount)
		{
			char* threadName = getArgString(&uartInput,1);
			pidof(threadName);
			putsUart0(threadName);
		}

		else if (!strcmp(command,"sched") && 2<=uartInput.argCount)
		{
			if (!strcmp(getArgString(&uartInput,1),"priority"))
				setSched(true);
			else if (!strcmp(getArgString(&uartInput,1),"roundrobin"))
				setSched(false);
			else
				putsUart0("\tInvalid Scheduler Type\r\n");
		}

		else if (!strcmp(command,"preemption") && 2<=uartInput.argCount)
		{
			if (!strcmp(getArgString(&uartInput,1),"on"))
				setPreempState(true);
			else if (!strcmp(getArgString(&uartInput,1),"off"))
				setPreempState(false);
			else
				putsUart0("\tInvalid Input\r\n");
		}

		else if (!strcmp(command,"pi") && 2<=uartInput.argCount)
		{
			if (!strcmp(getArgString(&uartInput,1),"on"))
				setPiState(true);
			else if (!strcmp(getArgString(&uartInput,1),"off"))
				setPiState(false);
			else
				putsUart0("\tInvalid Input\r\n");
		}

		else if (!strcmp(command,"setpri") && 3<=uartInput.argCount)
		{
			_fn fn = (_fn)atoi(getArgString(&uartInput,1));
			uint8_t priority = atoi(getArgString(&uartInput,2));
			setThreadPriority(fn,priority);
		}

		else if (!uartInput.argCount)
		{}

		else
		{
			putsUart0("\t"); putsUart0("Invalid Command\r\n");
		}
	}
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	bool ok;

	// Initialize hardware
	initHw();
	initRtos();

	// Power-up flash
	GREEN_LED = 1;
	waitMicrosecond(250000);
	GREEN_LED = 0;
	waitMicrosecond(250000);

	// Initialize semaphores
	createSemaphore(1,"KeyPressed");
	createSemaphore(0,"KeyReleased");
	createSemaphore(5,"FlashReq");
	createSemaphore(1,"Resource");

	// Add required idle process at lowest priority
	ok = createThread(idle, "Idle", 15, 1024);

	// Add other processes
	ok &= createThread(lengthyFn, "LengthyFn", 12, 1024);
	ok &= createThread(flash4Hz, "Flash4Hz", 8, 1024);
	ok &= createThread(oneshot, "OneShot", 4, 1024);
	ok &= createThread(readKeys, "ReadKeys", 12, 1024);
	ok &= createThread(debounce, "Debounce", 12, 1024);
	ok &= createThread(important, "Important", 0, 1024);
//	ok &= createThread(uncooperative, "Uncoop", 12, 1024);
	ok &= createThread(errant, "Errant", 12, 1024);
	ok &= createThread(shell, "Shell", 12, 1024);

	// Start up RTOS
	if (ok)
		startRtos(); // never returns
	else
		RED_LED = 1;

	return 0;
}
