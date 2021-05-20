
#include "kernel.h"

uint8_t semaphoreCount = 0;
uint8_t taskCurrent = 0;   // index of last dispatched task
uint8_t taskCount = 0;     // total number of valid tasks

bool prioritySch = true;			// Variable that decides on the scheduler (priority or round-robin)
bool preempState = true;			// Variable to decide whether preemption is on or off
bool priorityInheritance = false;	// Variable to decide whether priority inheritance is on or off
bool MPU_ISR_VALUE = false;

//-----------------------------------------------------------------------------
// RTOS Kernel Functions
//-----------------------------------------------------------------------------

//  initialize systick for 1ms system timer
void initRtos()
{
	uint8_t i;
	// no tasks running
	taskCount = 0;
	// clear out tcb records
	for (i = 0; i < MAX_TASKS; i++)
	{
		tcb[i].state = STATE_INVALID;
		tcb[i].pid = 0;
		tcb[i].semaphore = noSema;
	}
}

//  Implement prioritization to 16 levels
int rtosScheduler()
{
	uint8_t priority = 0xFF;
	static uint8_t task = 0xFF;
	if (prioritySch)
	{
		task = (++task >= MAX_TASKS) ? 0 : task;
		while (true)
		{
			priority = (++priority > 15) ? 0 : priority;
			for (uint8_t i = 0; i < MAX_TASKS; ++i)
			{
				uint8_t j = (task + i) % MAX_TASKS;
				if ((tcb[j].currentPriority == priority) && (tcb[j].state == STATE_READY || tcb[j].state == STATE_UNRUN))
					return task = j;
			}
		}
	}
	else
	{
		bool ok = false;
		while (!ok)
		{
			task = (++task >= MAX_TASKS) ? 0 : task;
			ok = (tcb[task].state == STATE_READY || tcb[task].state == STATE_UNRUN);
		}
	}
	return task;
}

bool createThread(_fn fn, const char name[], uint8_t priority, uint32_t stackBytes)
{
	bool ok = false;
	uint8_t i = 0;
	bool found = false;
	// store the thread name
	// add task if room in task list
	// allocate stack space for a thread and assign to sp below
	if (taskCount < MAX_TASKS)
	{
		// make sure fn not already in list (prevent reentrancy)
		while (!found && (i < MAX_TASKS))
		{
			found = (tcb[i++].pid == fn);
		}
		if (!found)
		{
			// find first available tcb record
			i = 0;
			while (tcb[i].state != STATE_INVALID) {i++;}
			uint8_t j = 0;
			while(name[j] != '\0')
			{
				tcb[i].name[j] = name[j]; ++j;
			}
			for(; j<15; ++j)
				tcb[i].name[j] = ' ';
			tcb[i].name[15] = '\0';
			tcb[i].state = STATE_UNRUN;
			tcb[i].pid = (_fn)((uint32_t)fn | 1);
			tcb[i].sp = &threadStack[i][0] + 512;
			tcb[i].spInit = &threadStack[i][0] + 512;
			tcb[i].priority = priority;
			tcb[i].currentPriority = priority;
			tcb[i].semaphore = noSema;
			// increment task count
			taskCount++;
			ok = true;
		}
	}
	// allow tasks switches again
	return ok;
}

void killErrantThread(uint32_t pid)
{
	_fn fn = (_fn)pid;
	uint8_t i; bool found = false;
	for (i = 1; !(found = tcb[i].pid == fn) && i<MAX_TASKS; ++i);
	if (found)
	{
		if (tcb[i].state == STATE_BLOCKED)
		{
			for (uint8_t k = 0; k<MAX_QUEUE_SIZE; ++k)
				semaphores[tcb[i].semaphore].processQueue[k] = (i == semaphores[tcb[i].semaphore].processQueue[k]) ? 0 : semaphores[tcb[i].semaphore].processQueue[k];
		}
		else if (tcb[i].semaphore != noSema)
		{
			semaphores[tcb[i].semaphore].count++;
			tcb[i].semaphore = noSema;
		}
		tcb[i].state = STATE_INVALID;
	}
}

void createSemaphore(uint8_t count, const char name[])
{
	if (semaphoreCount < MAX_SEMAPHORES)
	{
	    semaphores[semaphoreCount].count = count;
		uint8_t j = 0;
		while(name[j] != '\0')
		{
		    semaphores[semaphoreCount].name[j] = name[j]; ++j;
		}
		for(; j<15; ++j)
			(semaphores[semaphoreCount].name[j]) = ' ';
		semaphores[semaphoreCount].name[15] = '\0';
	}
	++semaphoreCount;
}

// modify this function to start the operating system, using all created tasks
void startRtos()
{
    actPsp();
    setRegisterVal(NVIC_MPU_NUMBER_R, 0);
    setRegisterBit(NVIC_MPU_ATTR_R,(17 << 1) | (0b110 << 24) | NVIC_MPU_ATTR_CACHEABLE | 1);
    setRegisterVal(NVIC_MPU_NUMBER_R, 1);
    setRegisterBit(NVIC_MPU_BASE_R, 0x40000000);
    setRegisterBit(NVIC_MPU_ATTR_R,(29 << 1) | (0b011 << 24) | (1 << 28) | NVIC_MPU_ATTR_SHAREABLE | NVIC_MPU_ATTR_BUFFRABLE | 1);
    setRegisterVal(NVIC_MPU_NUMBER_R, 2);
    setRegisterBit(NVIC_MPU_BASE_R, 0x20000000);
    setRegisterBit(NVIC_MPU_ATTR_R,(13 << 1) | (0b011 << 24) | NVIC_MPU_ATTR_CACHEABLE | NVIC_MPU_ATTR_SHAREABLE | 1);
    setRegisterVal(NVIC_MPU_NUMBER_R, 3);
    setRegisterBit(NVIC_MPU_BASE_R, 0x20004000);
    setRegisterBit(NVIC_MPU_ATTR_R,(13 << 1) | (0b011 << 24) | NVIC_MPU_ATTR_CACHEABLE | NVIC_MPU_ATTR_SHAREABLE | 1);

    setRegisterBit(NVIC_MPU_CTRL_R, NVIC_MPU_CTRL_ENABLE | NVIC_MPU_CTRL_PRIVDEFEN);

    uint32_t val = NVIC_SYS_HND_CTRL_R;
    setRegisterVal(NVIC_FAULT_STAT_R,0);
    setRegisterBit(val, NVIC_SYS_HND_CTRL_MEM | NVIC_SYS_HND_CTRL_USAGE | NVIC_SYS_HND_CTRL_BUS );
    setRegisterVal(NVIC_SYS_HND_CTRL_R,val);
    taskCurrent = rtosScheduler();
    tcb[taskCurrent].state = STATE_READY;
    setRegisterBit(NVIC_ST_CTRL_R, NVIC_ST_CTRL_ENABLE);
    setRegisterBit(TIMER1_CTL_R, TIMER_CTL_TAEN);
    setSp((uint32_t*)tcb[taskCurrent].sp);
    actPriv();
  //  NVIC_CFG_CTRL_R |= 0x00000004;   //Usage Fault Setting
  //  NVIC_FAULT_STAT_R |= 0x00001000;  //Bus Fault Setting
    _fn fn = (_fn)tcb[taskCurrent].pid;
    (*fn)();
}

void MPUIsr()
{
    NVIC_MPU_CTRL_R &= ~NVIC_MPU_CTRL_ENABLE;
    uint32_t *point=0 , faultaddr=0,busfaultaddress=0,ui32_faultstatvalue;
    uint32_t g_ui32SysHndCtrl;
    busfaultaddress =  NVIC_FAULT_ADDR_R;
    faultaddr =  NVIC_MM_ADDR_R ;
    ui32_faultstatvalue = NVIC_FAULT_STAT_R;

    MPU_ISR_VALUE = true;
    char trial[10];
    itoc((int)tcb[taskCurrent].pid,trial);
	putsUart0("\r\n");
    putsUart0("\r\n\tMPU FAULT IN TASK: ");
    putsUart0(trial);
    putsUart0("\r\n\t");
    putsUart0("Task Name:- ");
    putsUart0(tcb[taskCurrent].name);
    putsUart0("\r\n\t");
    point = getPsp();
    putsUart0("PSP                  : 0x");
    dectohex((uint32_t)point);
    putsUart0("\r\n\t");
    point = getSp();
    putsUart0("MSP                  : 0x");
    dectohex((uint32_t)point);
    putsUart0("\r\n\t");
   // g_ui32SysHndCtrl = NVIC_SYS_HND_CTRL_R;
    putsUart0("MFAULT_SYSHNDCTRL_R  : 0x");
   // putsUart0("SYSHNDCTRL_R: 0x000");
    dectohex((uint32_t) NVIC_SYS_HND_CTRL_R);
    putsUart0("\r\n\t");
    putsUart0("MPUFAULT ADDR        : 0x");
    dectohex((uint32_t)faultaddr);
    putsUart0("\r\n\t");
    putsUart0("BUS FAULT ADDR       : 0x");
    dectohex((uint32_t)busfaultaddress);
    putsUart0("\r\n\t");
    point = (uint32_t *)busfaultaddress;
    putsUart0("MPU FAULT ADDR-VALUE : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    putsUart0("NVIC FAULT STAT      : 0x");
    dectohex((uint32_t)ui32_faultstatvalue);
    putsUart0("\r\n\t");
    point = getPsp();
    putsUart0("R0                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("R1                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("R2                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("R3                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("R12                  : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("LR                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("PC                   : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n\t");
    point++;
    putsUart0("xPSR                 : 0x");
    dectohex((uint32_t)*point);
    putsUart0("\r\n");
	putsUart0("\r\n>");
    g_ui32SysHndCtrl = NVIC_SYS_HND_CTRL_R;
    g_ui32SysHndCtrl = (g_ui32SysHndCtrl & 0xFFFEDFFF);
    NVIC_SYS_HND_CTRL_R = g_ui32SysHndCtrl;

   //NVIC_FAULT_STAT_R ^= ~(NVIC_FAULT_STAT_IERR|NVIC_FAULT_STAT_DERR);
    NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;
}

void busFaultIsr()
{
    char trial[10];
    itoc((int)tcb[taskCurrent].pid,trial);
	putsUart0("\r\n");
    putsUart0("\r\n\tBUS FAULT IN TASK : ");
    putsUart0(trial);
    putsUart0(tcb[taskCurrent].name);
    putsUart0("\r\n");
	putsUart0("\r\n>");
	while(true);
}

void hardFaultIsr()
{
    uint32_t *point;
    char trial[10];
    itoc((int)tcb[taskCurrent].pid,trial);
	putsUart0("\r\n");
    putsUart0("\r\n\tHARD FAULT IN TASK : ");
    putsUart0(trial);
    putsUart0(tcb[taskCurrent].name);
    putsUart0("\r\n\t");
    point = getPsp();
    putsUart0("PSP                  : 0x");
    dectohex((uint32_t)point);
    putsUart0("\r\n\t");
    point = getSp();
    putsUart0("MSP                  : 0x");
    dectohex((uint32_t)point);
    putsUart0("\r\n\t");
    putsUart0("NVIC_HARD_FAULT_STAT_R : ");
    dectohex((uint32_t)NVIC_HFAULT_STAT_R);
    putsUart0("\r\n");
	putsUart0("\r\n>");
	while(true);
}

void usageFaultIsr()
{
    char trial[10];
    itoc((int)tcb[taskCurrent].pid,trial);
	putsUart0("\r\n");
    putsUart0("\r\n\tUSAGE FAULT IN TASK : ");
    putsUart0(trial);
    putsUart0(tcb[taskCurrent].name);
    putsUart0("\r\n");
	putsUart0("\r\n>");
	while(true);
}

// modify this function to add support for the system timer
// in preemptive code, add code to request task switch
void systickIsr()
{
	if (preempState)
		setRegisterBit(NVIC_INT_CTRL_R,NVIC_INT_CTRL_PEND_SV);
}

void timer1Isr()
{
	for (uint8_t i = 0; i<MAX_TASKS; ++i)
		if (tcb[i].state == STATE_DELAYED)
			tcb[i].state = !(--tcb[i].ticks) ? STATE_READY : STATE_DELAYED;
	setRegisterBit(TIMER1_ICR_R,TIMER_ICR_TATOCINT);
}

// in coop and preemptive, modify this function to add support for task switching
// process UNRUN and READY tasks differently
void pendSvIsr()
{
    if(MPU_ISR_VALUE == true)
    {
        uint32_t g_ui32FaultStatus=0;
        char trial[10];
        NVIC_MPU_CTRL_R =  MPU_CONFIG_PRIV_DEFAULT |MPU_RGN_ENABLE|MPU_CONFIG_HARDFLT_NMI;
        NVIC_SYS_HND_CTRL_R |= 0x00010000;
        killErrantThread((uint32_t)tcb[taskCurrent].pid);
        itoc((int)tcb[taskCurrent].pid,trial);
		putsUart0("\r\n\t");
        putsUart0("\r\n\tPendSV in Process : ");
        putsUart0(trial);
        putsUart0("\r\n\t");
        putsUart0("Task Name:- ");
        putsUart0(tcb[taskCurrent].name);
        putsUart0("\r\n");
        g_ui32FaultStatus = NVIC_FAULT_STAT_R;
        if(((g_ui32FaultStatus & 0x00000001) == 0b01) || (g_ui32FaultStatus & 0x00000002) == 0b10)
        {
            NVIC_FAULT_STAT_R = g_ui32FaultStatus;
            putsUart0("\r\n\tCalled from MPU\r\n");
            putsUart0("\r\n>");
        }
        MPU_ISR_VALUE = false;
    }
	uint32_t processTime = disTimer0();
	uint32_t* psp = getPsp();
	psp = pushR42R11(psp);
	tcb[taskCurrent].sp = psp;
	for (uint8_t i = 0; i<MAX_TASKS; ++i)
		tcb[i].processTime = ((i == taskCurrent ? processTime : 0) + 15*tcb[i].processTime)>>4;
	taskCurrent = rtosScheduler();
    if (taskCurrent < 8)
    {
        setRegisterVal(NVIC_MPU_NUMBER_R,2);
        clrRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
        setRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_SRD_M);
        flpRegisterBit(NVIC_MPU_ATTR_R,1<<(taskCurrent+8));
        setRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
        setRegisterVal(NVIC_MPU_NUMBER_R,3);
        clrRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
    }
    else
    {
        setRegisterVal(NVIC_MPU_NUMBER_R,3);
        clrRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
        setRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_SRD_M);
        flpRegisterBit(NVIC_MPU_ATTR_R,1<<(taskCurrent));
        setRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
        setRegisterVal(NVIC_MPU_NUMBER_R,2);
        clrRegisterBit(NVIC_MPU_ATTR_R,NVIC_MPU_ATTR_ENABLE);
    }

	psp = (uint32_t*)tcb[taskCurrent].sp;
	if (tcb[taskCurrent].state == STATE_UNRUN)
	{
		tcb[taskCurrent].state = STATE_READY;
		*(psp - 2) = (uint32_t)tcb[taskCurrent].pid;
		*(psp - 1) = bit24;
		setPsp(psp - 8);
	}
	else
	{
		psp = popR42R11(psp);
		setPsp(psp);
	}
	enTimer0();
}

// modify this function to add support for the service call
// in preemptive code, add code to handle synchronization primitives
void svCallIsr()
{
	uint8_t n = getSvcNo();
	switch (n)
	{
	case YIELD:
		setRegisterBit(NVIC_ST_CURRENT_R,1);
		setRegisterBit(NVIC_INT_CTRL_R,NVIC_INT_CTRL_PEND_SV);
		break;
	case SLEEP:
		tcb[taskCurrent].state = STATE_DELAYED;
		tcb[taskCurrent].ticks = getSleepTicksR0();
		setRegisterBit(NVIC_ST_CURRENT_R,1);
		setRegisterBit(NVIC_INT_CTRL_R,NVIC_INT_CTRL_PEND_SV);
		break;
	case WAIT:
	{
		enum semaType s;
		s = getSemaphoreR0();
		tcb[taskCurrent].semaphore = s;
		if (semaphores[s].count > 0)
		{
			--semaphores[s].count;
			break;
		}
		else if (((semaphores[s].head + 1) & 7) != semaphores[s].tail)
		{
		    semaphores[s].head = (++semaphores[s].head) & 7;
		    semaphores[s].processQueue[semaphores[s].head] = taskCurrent;
			tcb[taskCurrent].state = STATE_BLOCKED;
			setRegisterBit(NVIC_ST_CURRENT_R,1);
			setRegisterBit(NVIC_INT_CTRL_R,NVIC_INT_CTRL_PEND_SV);
		}
		break;
	}
	case POST:
	{
		enum semaType s;
		s = getSemaphoreR0();
		++semaphores[s].count; tcb[taskCurrent].semaphore = noSema;
		tcb[taskCurrent].currentPriority = tcb[taskCurrent].priority;
		if (semaphores[s].tail != semaphores[s].head)
		{
			while (semaphores[s].tail != semaphores[s].head && semaphores[s].processQueue[semaphores[s].tail] == 0)
			    semaphores[s].tail = (++semaphores[s].tail) & 7;
			if (semaphores[s].processQueue[semaphores[s].tail] != 0)
			{
				tcb[semaphores[s].processQueue[semaphores[s].tail]].state = STATE_READY;
				tcb[semaphores[s].processQueue[semaphores[s].tail]].semaphore = s;
				--semaphores[s].count; semaphores[s].processQueue[semaphores[s].tail] = 0;
			}
		}
		break;
	}
	case SHELL_PS:
	{
		struct psInfo* psData = getPsDataR0();
		for (uint8_t i = 0; i<MAX_TASKS; ++i)
		{
			strcpy(psData[i].name,tcb[i].name);
			psData[i].pid = (uint32_t)tcb[i].pid;
			psData[i].priority = tcb[i].priority;
			psData[i].processTime = tcb[i].processTime;
			psData[i].state = tcb[i].state;
		}
		break;
	}
	case SHELL_KILL:
	{
		_fn fn = getThreadR0();
		uint8_t i; bool found = false;
		for (i = 1; !(found = tcb[i].pid == fn) && i<MAX_TASKS; ++i);
		if (found)
		{
			if (tcb[i].state == STATE_BLOCKED)
			{
				for (uint8_t k = 0; k<MAX_QUEUE_SIZE; ++k)
				    semaphores[tcb[i].semaphore].processQueue[k] = (i == semaphores[tcb[i].semaphore].processQueue[k]) ? 0 : semaphores[tcb[i].semaphore].processQueue[k];
			}
			else if (tcb[i].semaphore != noSema)
			{
			    semaphores[tcb[i].semaphore].count++;
				tcb[i].semaphore = noSema;
			}
			tcb[i].state = STATE_INVALID;
		}
		break;
	}
	case SHELL_RESTART:
	{
	    char* command = getFunNameR0();
	    bool found = false; _fn fn;
        for (uint8_t i = 0; i < MAX_TASKS; ++i)
        {
            uint8_t j = 0;
            while (tcb[i].name[j]==command[j])
                ++j;
            if (tcb[i].name[j] == ' ' && command[j] == '\0')
            {
                found = true;
                fn = (_fn)tcb[i].pid;
            }
        }
        if (!found)
			strcpy(command,"\tInvalid\r\n");
		else
        {
			strcpy(command,"\tRestarted\r\n");
            uint8_t i = 1;
            while (tcb[i].pid != fn){++i;}
            tcb[i].state = STATE_UNRUN;
            tcb[i].sp = tcb[i].spInit;
           // tcb[i].priority = 8;
            tcb[i].currentPriority = tcb[i].priority;
        }
		break;
	}
	case PIWAIT:
	{
		enum semaType s;
		s = getSemaphoreR0();
		tcb[taskCurrent].semaphore = s;
		if (semaphores[s].count > 0)
		{
			--semaphores[s].count;
			break;
		}
		else if (((semaphores[s].head + 1) & 7) != semaphores[s].tail)
		{
		    semaphores[s].head = (++semaphores[s].head) & 7;
		    semaphores[s].processQueue[semaphores[s].head] = taskCurrent;
			tcb[taskCurrent].state = STATE_BLOCKED;
			if (priorityInheritance)
			{
				_fn fn = getInheritingThreadR1();
				for (uint8_t i = 0; i<MAX_TASKS; ++i)
				{
					if (tcb[i].pid == fn && tcb[i].semaphore == s)
						if (tcb[i].currentPriority >= tcb[taskCurrent].currentPriority)
						{
							tcb[i].currentPriority = tcb[taskCurrent].currentPriority;
							break;
						}
				}
			}
			setRegisterBit(NVIC_ST_CURRENT_R,1);
			setRegisterBit(NVIC_INT_CTRL_R,NVIC_INT_CTRL_PEND_SV);
		}
		break;
	}
	case SHELL_SETPRI:
	{
		_fn fn = getThreadR0(); uint8_t priority = getPriorityR1();
		for (uint8_t i = 0; i<MAX_TASKS; ++i)
		if (tcb[i].pid == fn)
		{
			tcb[i].priority = priority;
			tcb[i].currentPriority = priority;
		}
	}
	case SHELL_IPCS:
	{
		struct ipcsInfo* ipcsData = getIpcsDataR0();
		uint8_t* semaCount = getSemaCountR1();
		*semaCount = semaphoreCount;
        for (uint8_t i = 0; i<semaphoreCount; ++i)
        {
            strcpy(ipcsData[i].semaName,semaphores[i].name);
            ipcsData[i].count = semaphores[i].count;
            for (uint8_t j = 0; j<MAX_TASKS; ++j)
                if (tcb[j].state != STATE_BLOCKED && tcb[j].semaphore == i)
				{
					strcpy(ipcsData[i].running,tcb[j].name);
					break;
				}
				else
					strcpy(ipcsData[i].running,"             ");
            for (uint8_t j = 0; j<MAX_TASKS; ++j)
                if (tcb[j].state == STATE_BLOCKED && tcb[j].semaphore == i)
                    strcpy(ipcsData[i].blocked,tcb[j].name);
        }
        break;
	}
	case SHELL_REBOOT:
	{
		setRegisterVal(NVIC_APINT_R,NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ);
		break;
	}
	case SHELL_PIDOF:
	{
		char* threadName = getFunNameR0();
		bool found = false;
		for (uint8_t i = 0; i < MAX_TASKS; ++i)
		{
			uint8_t j = 0;
			while (tcb[i].name[j] == threadName[j])
				++j;
			if (tcb[i].name[j] == ' ' || threadName[j] == '\0')
			{
				found = true; strcpy(threadName,"\t");
				itoa((uint32_t)tcb[i].pid,threadName+1,15);
				strcat(threadName,"\r\n");
			}
		}
		if (!found)
			strcpy(threadName,"\tInvalid\r\n");
		break;
	}
	case SHELL_SCHED:
	{
		prioritySch = getSchedTypeR0();
		break;
	}
	case SHELL_PREEM:
	{
		preempState = getPreempStateR0();
		break;
	}
	case SHELL_PI:
	{
		priorityInheritance = getPiStateR0();
		break;
	}
	default :
		break;
	}
}

void restartThread(char* fn)
{
	__asm(" svc #6");
}

void destroyThread(_fn fn)
{
	__asm(" svc #5");
}

void setThreadPriority(_fn fn, uint8_t priority)
{
	__asm(" svc	#8");
}

void yield()
{
	__asm(" svc #0");
}

void sleep(uint32_t tick)
{
	__asm(" svc	#1");
}

void wait(enum semaType pSemaphore)
{
	__asm(" svc	#2");
}

void piWait(enum semaType pSemaphore, _fn fn)
{
	__asm(" svc	#7");
}

void post(enum semaType pSemaphore)
{
	__asm(" svc	#3");
}

void reboot()
{
	__asm(" svc #10");
}

void pidof(char* fn)
{
	__asm(" svc #13");
}

void setSched(bool sched)
{
	__asm(" svc #14");
}

void setPreempState(bool preemp)
{
	__asm(" svc #15");
}

void setPiState(bool preemp)
{
	__asm(" svc #16");
}

void processStatus(struct psInfo* psData)
{
	__asm(" svc #11");
}

void interProcessCommStat(struct ipcsInfo* ipcsData, uint8_t* semaCount)
{
	__asm(" svc #12");
}
