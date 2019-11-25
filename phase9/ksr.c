// ksr.c, 159

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "proc.h"
#include "syscall.h"

void SpawnSR(func_p_t p)
{
   	int pid;

   	if(QueEmpty(&avail_que))
   	{
    	cons_printf("Panic: out of PID!\n");
      	breakpoint();
   	}

   	pid = DeQue(&avail_que);
   	Bzero((char *) &pcb[pid], sizeof(pcb_t));
   	pcb[pid].state = READY;

   	if(pid != IDLE)
    	EnQue(pid, &ready_que);

   	MemCpy((char *)(DRAM_START + (pid*STACK_MAX)), (char *) p, STACK_MAX);
   	pcb[pid].tf_p = (tf_t *)(DRAM_START + (pid + 1)*STACK_MAX - sizeof(tf_t));
   	pcb[pid].tf_p->efl = EF_DEFAULT_VALUE | EF_INTR;
   	pcb[pid].tf_p->cs  = get_cs();
   	pcb[pid].tf_p->eip = (DRAM_START + (pid*STACK_MAX));
	
	pcb[pid].dir = KDir;  //set Dir in PCB to KDir for the new process (so it'll use real memory),
	page[pid].pid = pid;
	/*mark down the equivalent DRAM page to be occupied by the new process
   (e.g., Idle and Login), so the page array can skip these already used*/
	if(pid==0){  //if new pid is 0, set STDOUT in its PCB to CONSOLE; for others, to TTY
		pcb[pid].STDOUT = CONSOLE;
	}
	else{
		pcb[pid].STDOUT = TTY;
	}
}

void TimerSR(void)
{
	int i;
   	outportb(PIC_CONT_REG, TIMER_SERVED_VAL); 	//1st notify PIC control register that timer event is now served
   	sys_time_count++;                         	//increment system time count by 1
   	pcb[run_pid].time_count++;                	//increment the time count of the process currently running by 1
   	pcb[run_pid].total_time++;			//increment the life span count of the process currently running by 1

   	for(i = 0; i < QUE_MAX; i++)
   	{
		if((pcb[i].state == SLEEP) && (pcb[i].wake_time == sys_time_count))
		{
	    	EnQue(i, &ready_que);
	    	pcb[i].state = READY;
		}
   	}

  	if(run_pid == IDLE)
		return;

  	if(pcb[run_pid].time_count == TIME_MAX)
  	{
    	pcb[run_pid].state = READY;
      	EnQue(run_pid, &ready_que);
      	run_pid = NONE;
   	}
}

void SyscallSR(void)
{
   	switch(pcb[run_pid].tf_p->eax)
	{
    	case SYS_GET_PID:
	   		pcb[run_pid].tf_p->ebx = run_pid;
	     	break;
    	case SYS_GET_TIME:
	    	pcb[run_pid].tf_p->ebx = sys_time_count;
	    	break;
    	case SYS_SLEEP:
	    	SysSleep();
	   		break;
		case SYS_WRITE:
	    	SysWrite();
			break;
    	case SYS_SET_CURSOR:
			SysSetCursor();
       		break;
		case SYS_FORK:
			SysFork();
       		break;
    	case SYS_GET_RAND:
       		pcb[run_pid].tf_p->ebx = sys_rand_count;
       		break;
    	case SYS_LOCK_MUTEX:
      		SysLockMutex();
       		break;
    	case SYS_UNLOCK_MUTEX:
       		SysUnlockMutex();
       		break;
		case SYS_WAIT:
			SysWait();
			break;
		case SYS_EXIT:
			SysExit();
			break;
		case SYS_SIGNAL:
			SysSignal();
			break;
		case SYS_KILL:
			SysKill();
			break;
		case SYS_READ:
			SysRead();
			break;
		case SYS_VFORK:
			SysVFork();
			break;
		default:
        	cons_printf("Kernel Panic: no such syscall!\n");
        	breakpoint();
   	}
	
	if (run_pid != NONE)
    {
	   	pcb[run_pid].state = READY;
	   	EnQue(run_pid, &ready_que);
        run_pid = NONE;
    }
	
    set_cr3(KDir);
	pcb[run_pid].dir=KDir;
}

void SysSleep(void)
{
   	int sleep_sec = pcb[run_pid].tf_p->ebx;
   	pcb[run_pid].wake_time = sys_time_count + (sleep_sec * 10);
   	pcb[run_pid].state = SLEEP;
   	run_pid = NONE;
}

void SysWrite(void)
{
	int column, row;
	char *str;
	str = (char *) pcb[run_pid].tf_p->ebx;
	if(STDOUT=CONSOLE)
	{
		while(*str != '\0')
		{
			if(sys_cursor == VIDEO_END)
			{
				row = 0;
				sys_cursor = VIDEO_START;
				while(row < 25)
				{
					column = 0;
					while(column < 80)
					{
						*sys_cursor = ' ' + VGA_MASK_VAL;
						sys_cursor++;
						column++;
					}
					row++;
				}
				sys_cursor = VIDEO_START;
			}

			if(*str == '\r') //If '\r' is pressed then go to the start of the next line and return.
			{
				column = (sys_cursor - VIDEO_START) % 80;
				sys_cursor += 80 - column;
				return;
			}
			else
			{
				*sys_cursor = *str + VGA_MASK_VAL;
				sys_cursor++;
			}
			str++;
		}
	}
	
	else if(STDOUT=TTY)
	{
		
		tty.str = &str;
		Enque(run_pid,tty.wait.que); //2. suspend the process in the wait queue of 'tty'
		pcb[run_pid].state=IO_WAIT;		
		run_pid=NONE;
		TTYSR();
    	
		/* else if it's TTY:
        	1. copy the string address to the 'str' in 'tty'
        	2. suspend the process in the wait queue of 'tty'
        	3. demote its state to IO_WAIT
        	4. run_pid is NONE
        	5. call TTYSR()
        else
        	panic: no such device!*/
	}
	else
		{
			cons_printf("Panic: no such device!*/!\n");
			breakpoint();				
		}
	
	
	
}

void SysSetCursor(void)
{
    int  cursor_position;
    sys_cursor = VIDEO_START;
    cursor_position = pcb[run_pid].tf_p->ebx;
    sys_cursor += cursor_position;
}

void SysFork(void)
{
    int PID; 		//Child's pid
    int distance;   //Stores the distance between child and parent trapframe
    int *initBP;	//stores the Base pointer address of the parent process.

    if(QueEmpty(&avail_que))
    {
		pcb[run_pid].tf_p->ebx = NONE;
       	return;
    }

    PID = DeQue(&avail_que);
    Bzero((char *) &pcb[PID], sizeof(pcb_t));
    pcb[PID].state = READY;
    if(PID != IDLE)
    	EnQue(PID, &ready_que);

    distance = (PID - run_pid) * (STACK_MAX);

    pcb[PID].tf_p       = (tf_t *) ((int) pcb[run_pid].tf_p + distance);
    pcb[PID].state      = READY;		//Changing the state of the child to READY.
    pcb[PID].ppid       = run_pid;		//Changinhg the PPID to the run_pid.

    MemCpy((char *) (DRAM_START + PID*STACK_MAX), (char *) (DRAM_START +run_pid*STACK_MAX), STACK_MAX);

    pcb[PID].tf_p->eip = pcb[PID].tf_p->eip + distance;	//set eip in trapframe to eip+distance
    pcb[PID].tf_p->ebp = pcb[PID].tf_p->ebp + distance; //set ebp in trapframe to ebp+distance

    initBP      = (int *)pcb[PID].tf_p->ebp;
    *initBP++   = *initBP + distance;
    *initBP 	= *initBP + distance;

    pcb[run_pid].tf_p->ebx = PID;		//set ebx to new pid in parent process's trapframe
	pcb[PID].tf_p->ebx = 0;
	
	pcb[PID].dir = KDir;  //set Dir in PCB to KDir for the new process (so it'll use real memory),
	page[PID].pid = PID;/*mark down the equivalent DRAM page to be occupied by the new process
   (e.g., Idle and Login), so the page array can skip these already used*/
}

void SysLockMutex(void) {   // phase4
    int mutex_id;
    mutex_id = pcb[run_pid].tf_p->ebx;

    if(mutex_id == VIDEO_MUTEX)
    {
    	if(video_mutex.lock == UNLOCKED)
		{
            video_mutex.lock = LOCKED;
      	}
		else
		{
	    	EnQue(run_pid, &video_mutex.suspend_que);
	    	pcb[run_pid].state = SUSPEND;
	    	run_pid = NONE;
      	}
    }
    else
    {
        cons_printf("Panic: no such mutex ID!\n");
      	breakpoint();
    }
}

void SysUnlockMutex(void)
{
    int mutex_id, released_pid;
    mutex_id = pcb[run_pid].tf_p->ebx;

    if(mutex_id == VIDEO_MUTEX)
    {
        if(!QueEmpty(&video_mutex.suspend_que))
		{
	    	released_pid = DeQue(&video_mutex.suspend_que);
	    	EnQue(released_pid, &ready_que);
	    	pcb[released_pid].state = READY;
      	}
		else
		{
	    	video_mutex.lock = UNLOCKED;
     	}
    }
    else
    {
        cons_printf("Panic: no such mutex ID!\n");
      	breakpoint();
    }
}

void SysExit(void)
{
    int exit_code;
	int parentPID;
	int i = 0;

	parentPID = pcb[run_pid].ppid;
	
	if(WAIT != pcb[parentPID].state)
    {
        pcb[run_pid].state = ZOMBIE;
		run_pid = NONE;

		set_cr3(pcb[parentPID].dir);
		if(pcb[parentPID].signal_handler[SIGCHLD] != NULL)
			AlterStack(parentPID, pcb[parentPID].signal_handler[SIGCHLD]);
		set_cr3(pcb[run_pid].dir);
    }
    else
    {
		exit_code = pcb[run_pid].tf_p->ebx;

		set_cr3(pcb[parentPID].dir);
		pcb[parentPID].state = READY;	// upgrade parent's state
		EnQue(parentPID, &ready_que);	// move parent to be ready to run again

		pcb[parentPID].tf_p->ecx = run_pid;		//pass over exiting PID to parent
		* (int *) pcb[parentPID].tf_p->ebx = exit_code;	//pass over exit code to parent
		
		set_cr3(pcb[run_pid].dir);
		
		//reclaim child resources (alter state, move it)
	    pcb[run_pid].state = AVAIL;
		EnQue(run_pid, &avail_que);

		for(i = 0; i < PAGE_MAX; i++)
		{
			if(page[i].pid == run_pid)
				page[i].pid = NONE;		
		}

		run_pid = NONE;	//no running process anymore
		
    }
}

void SysWait(void)
{
	int PID; // To store Zombie child's pid.
	int i = 0;
	//search for any child that called to exit?
	for(PID = 0; PID < QUE_MAX; PID++)
		if((pcb[PID].state == ZOMBIE) && (pcb[PID].ppid == run_pid))
			break;

	if(PID == QUE_MAX) 		// No Zombie Process
	{
		pcb[run_pid].state = WAIT;	// parent is blocked into WAIT state
		run_pid = NONE;				// no running process anymore
	}
	else
	{
		pcb[run_pid].tf_p->ecx = PID; 	// pass over child's PID to parent
		*(int *) pcb[run_pid].tf_p->ebx = pcb[PID].tf_p->ebx;	// pass over its exit code to parent

		set_cr3(pcb[PID].dir);
		pcb[PID].state = AVAIL;	// reclaim child resources by altering state
		
		for(i = 0; i < PAGE_MAX; i++)
		{
			if(page[i].pid == PID)
				page[i].pid = NONE;		
		}

		EnQue(PID, &avail_que); // reclaim child resources by moving it to avail_que
		set_cr3(pcb[run_pid].dir);
	}
}

void SysSignal(void)
{
	int signal_name;
	signal_name = pcb[run_pid].tf_p->ebx;

	pcb[run_pid].signal_handler[signal_name] = (func_p_t) pcb[run_pid].tf_p->ecx;;
}

void SysKill(void)
{
	int i, pid, signal_name;

	pid = pcb[run_pid].tf_p->ebx;			// ebx of run_pid has the pid
	signal_name = pcb[run_pid].tf_p->ecx;	// ecx of the run_pid has the signal_name

	if ((pid == 0) && (signal_name == SIGCONT))
	{
		for(i = 0; i < QUE_MAX; i++) // We check for the children of run_pid who are sleeping and wake them.
		{
			if((pcb[i].ppid == run_pid) && (pcb[i].state == SLEEP))
			{
				pcb[i].state = READY;
				EnQue(i, &ready_que);
			}
		}
	}
}

void AlterStack(int pid, func_p_t p)
{
	int *tempEFL;
	int tempEIP;
	tf_t temporaryTF;

	tempEFL 	= &pcb[pid].tf_p->efl;
	tempEIP 	= pcb[pid].tf_p->eip;
	temporaryTF = *pcb[pid].tf_p;

	(int) pcb[pid].tf_p    -= 4;				// Lowering trapframe by 4 bytes [1 Register]
	temporaryTF.eip 		= (unsigned int) p;	// replacing EIP in trapframe with 'p'
	*pcb[pid].tf_p 			= temporaryTF;
	*tempEFL 				= tempEIP;			// Insert the original EIP between lowered trapframe and what originally above
}

void SysRead(void)
{
	if(!QueEmpty(&kb.buffer))
		pcb[run_pid].tf_p->ebx = DeQue(&kb.buffer);
   	else
	{
		EnQue(run_pid, &kb.wait_que);
   		pcb[run_pid].state = IO_WAIT;
   		run_pid = NONE;
	}
}

void KBSR(void)
{
	char ch;
	int pid;
	if(!cons_kbhit())
		return;

	ch = cons_getchar();
    if(ch == '$')
	{
       	cons_printf("You Pressed %c. Entering the breakpoint for GDB.\n", ch);
    	breakpoint();
    }

	if(QueEmpty(&kb.wait_que)) // NO process awaits (KB wait queue is empty):
		EnQue(ch, &kb.buffer); //enqueue the key to the KB buffer
	else
	{
    	pid = DeQue(&kb.wait_que); //release a waiting process from the wait queue
		
		set_cr3(pcb[pid].dir);
    	EnQue(pid, &ready_que);    //queue it to the ready-to-run queue
    	pcb[pid].state = READY;    //update its state
    	pcb[pid].tf_p->ebx = ch;   //give it the key which means to copy the key into ebx trapframe
		set_cr3(pcb[run_pid].dir);
	}
}

void SysVFork(void)
{
	int DIR, IT, DT, IP, DP;// DIR_addr, IT_addr, DT_addr, IP_addr, DP_addr;
	int new_pid;
	int index[5];	//To store the page numbers that are not occupied.
	int i = 0;		//For lopping PAGE_MAX times
	int j = 0;		//To be used by index[]; 
	
	new_pid = DeQue(&avail_que);
    Bzero((char *) &pcb[new_pid], sizeof(pcb_t));
	if(new_pid != IDLE)
    	EnQue(new_pid, &ready_que);
	
	pcb[new_pid] = pcb[run_pid];

    pcb[new_pid].state      	= READY;		//Changing the state of the child to READY.
	pcb[new_pid].tf_p       	= (tf_t *) (G2-sizeof(tf_t));
    pcb[new_pid].time_count 	= 0;
	pcb[new_pid].total_time 	= 0;
    pcb[new_pid].ppid       	= run_pid;		//Changinhg the PPID to the run_pid.

	for(i = 0; i < PAGE_MAX; i++)	
	{
		if((page[i].pid == NONE) && (j != 5))	
		{
			index[j] = i;
			j++;
			if(j == 5)
				break;
		}
	}
	
	if(j != 5)	
	{
		cons_printf("SysVFork Panic: We do not have enough pages!\n");
        breakpoint();
	} 

	DIR = index[0];
	IT  = index[1];
	DT  = index[2];
	IP  = index[3];
	DP  = index[4];

	for(i = 0; i < 5; i++)	
	{
		page[index[i]].pid = new_pid;
	}
		
	Bzero(page[DIR].u.content, STACK_MAX);
	Bzero(page[IT].u.content, STACK_MAX);
	Bzero(page[DT].u.content, STACK_MAX);
	Bzero(page[IP].u.content, STACK_MAX);
	Bzero(page[DP].u.content, STACK_MAX);

	MemCpy( (char *)page[DIR].u.entry, (char *) KDir, 16*4);
	page[DIR].u.entry[256] 	= page[IT].u.addr | PRESENT | RW;
    page[DIR].u.entry[511] 	= page[DT].u.addr | PRESENT | RW;
	page[IT].u.entry[0] 	= page[IP].u.addr | PRESENT | RO;
	page[DT].u.entry[1023] 	= page[DP].u.addr | PRESENT | RW;
    MemCpy((char *) page[IP].u.entry, (char *) pcb[run_pid].tf_p->ebx, PAGE_SIZE);	
	page[DP].u.entry[1023] 	= EF_DEFAULT_VALUE | EF_INTR;
	page[DP].u.entry[1022] 	= get_cs();
   	page[DP].u.entry[1021] 	= G1;
	
    pcb[new_pid].dir = page[DIR].u.addr;
}
