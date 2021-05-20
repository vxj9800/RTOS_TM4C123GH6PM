//  Author: Vatsal Asitkumar Joshi
//  Date: Feb 12th, 2019
//  This code is a header file for the functions provided in funs.s
//
//  "If you are done writing the code, now is a good time to debug it."
//

#include <stdint.h>

#ifndef __FUNS_DEFINED__
#define __FUNS_DEFINED__

extern void actPriv();
extern void setSp(uint32_t* sp);
extern uint32_t* getSp();

extern void actPsp();
extern void setPsp(uint32_t* psp);
extern uint32_t* getPsp();

extern uint32_t* pushR42R11(uint32_t* add);
extern uint32_t* popR42R11(uint32_t* add);

extern uint8_t getSvcNo();
extern uint32_t getR0();
extern uint32_t getR1();
#define getSleepTicksR0()	getR0()
#define getFunNameR0() ((char*) getR0())
#define getSemaphoreR0()	((enum semaType) getR0())
#define getThreadR0()		((_fn)getR0())
#define getInheritingThreadR1()	((_fn)getR1())
#define getPriorityR1()	((uint8_t)getR1())
#define getSchedTypeR0()    ((bool)getR0())
#define getPreempStateR0()  ((bool)getR0())
#define getPiStateR0()  ((bool)getR0())
#define getPsDataR0()   ((struct psInfo*)getR0())
#define getIpcsDataR0() ((struct ipcsInfo*)getR0())
#define getSemaCountR1()	((uint8_t*)getR1())
#endif
