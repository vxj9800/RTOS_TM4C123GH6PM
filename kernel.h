//  Author: Vatsal Asitkumar Joshi
//  Date: May 1st, 2020
//  This code is a header file for the functions provided in uart0.c
//
//  "If you are done writing the code, now is a good time to debug it."
//

#ifndef __RTOS_KERNEL_DEFINED__
#define __RTOS_KERNEL_DEFINED__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bitDefines.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "funs.h"
#include "timer.h"
#include "mpu.h"
#include "stringReplacement.h"

//-----------------------------------------------------------------------------
// RTOS Defines and Kernel Variables
//-----------------------------------------------------------------------------

// function pointer
typedef void (*_fn)();

// semaphore
#define MAX_SEMAPHORES 5
#define MAX_QUEUE_SIZE 8
struct semaphore
{
	char name[16];
	uint8_t head, tail;
	uint16_t count;
	uint16_t queueSize;
	uint32_t processQueue[MAX_QUEUE_SIZE]; // store task index here
} semaphores[MAX_SEMAPHORES];

//struct semaphore *keyPressed, *keyReleased, *flashReq, *resource;
enum semaType {keyPressed, keyReleased, flashReq, resource, noSema = 100};

// task
#define STATE_INVALID    0 // no task
#define STATE_UNRUN      1 // task has never been run
#define STATE_READY      2 // has run, can resume at any time
#define STATE_DELAYED    3 // has run, but now awaiting timer
#define STATE_BLOCKED    4 // has run, but now blocked by semaphore

#define MAX_TASKS 10       // maximum number of valid tasks

//  add store and management for the memory used by the thread stacks
//           thread stacks must start on 1 kiB boundaries so mpu can work correctly
uint32_t threadStack[MAX_TASKS][512];

struct _tcb
{
	uint32_t processTime;		   // Variable to store time taken by the process
	uint8_t state;                 // see STATE_ values above
	void *pid;                     // used to uniquely identify thread
	void *spInit;                  // location of original stack pointer
	void *sp;                      // location of stack pointer for thread
	int8_t priority;               // 0=highest to 15=lowest
	int8_t currentPriority;        // used for priority inheritance
	uint32_t ticks;                // ticks until sleep complete
	char name[16];                 // name of task used in ps command
	enum semaType semaphore;   // pointer to the semaphore that is blocking the thread
} tcb[MAX_TASKS];

struct psInfo
{
	uint32_t pid;
	char name[16];
	uint8_t priority;
	uint8_t state;
	uint32_t processTime;
};

struct ipcsInfo
{
	char semaName[16];
	uint16_t count;
	char running[16];
	char blocked[16];
};

// SVC types
#define YIELD			0
#define SLEEP			1
#define WAIT			2
#define POST			3
#define SHELL_KILL 		5
#define SHELL_RESTART	6
#define PIWAIT			7
#define SHELL_SETPRI	8
#define SHELL_REBOOT	10
#define SHELL_PS		11
#define SHELL_IPCS		12
#define SHELL_PIDOF		13
#define SHELL_SCHED		14
#define SHELL_PREEM		15
#define SHELL_PI		16

void initRtos();
bool createThread(_fn fn, const char name[], uint8_t priority, uint32_t stackBytes);
void createSemaphore(uint8_t count, const char name[]);
void startRtos();
void restartThread(char* fn);
void destroyThread(_fn fn);
void setThreadPriority(_fn fn, uint8_t priority);
void yield();
void sleep(uint32_t tick);
void wait(enum semaType pSemaphore);
void piWait(enum semaType pSemaphore, _fn fn);
void post(enum semaType pSemaphore);
void reboot();
void pidof(char* fn);
void setSched(bool sched);
void setPreempState(bool preemp);
void setPiState(bool preemp);
void processStatus(struct psInfo* psData);
void interProcessCommStat(struct ipcsInfo* ipcsData, uint8_t* semaCount);

#endif
