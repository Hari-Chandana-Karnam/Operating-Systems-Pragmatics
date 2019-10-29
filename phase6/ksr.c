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
   	char *str = (char *) pcb[run_pid].tf_p->ebx;
   	int i = 0;
   	while(str[i] != '\0')
   	{	
		if(sys_cursor == VIDEO_END)
	    	sys_cursor = VIDEO_START;	
		*sys_cursor = str[i] + VGA_MASK_VAL;
		sys_cursor++;
		i++;
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
    int distance;       //Stores the distance between child and parent trapframe
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
	 
	parentPID = pcb[run_pid].ppid;
	
	if(WAIT != pcb[parentPID].state)
    {
        /*running process cannot exit, it becomes a zombie
        no running process anymore*/
        pcb[run_pid].state = ZOMBIE;
		run_pid = NONE;
    }
    else
    {
		exit_code = pcb[run_pid].tf_p->ebx;
    	
		pcb[parentPID].state = READY;	// upgrade parent's state
		EnQue(parentPID, &ready_que);	// move parent to be ready to run again
				
		pcb[parentPID].tf_p->ecx = run_pid;		//pass over exiting PID to parent
		* (int *) pcb[parentPID].tf_p->ebx = exit_code;	//pass over exit code to parent
		
		//reclaim child resources (alter state, move it)
	    pcb[run_pid].state = AVAIL;
		EnQue(run_pid, &avail_que);
	    run_pid = NONE;	//no running process anymore
    }
}

void SysWait(void)
{
	int PID; // To store Zombie child's pid.
	
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
		pcb[run_pid].tf_p->ecx = PID; 					// pass over child's PID to parent
		*(int *) pcb[run_pid].tf_p->ebx = pcb[PID].tf_p->ebx;	// pass over its exit code to parent
		
		pcb[PID].state = AVAIL;	// reclaim child resources by altering state
		EnQue(PID, &avail_que); // reclaim child resources by moving it to avail_que
	}
}

void SysSignal(void)
{                                                                                                                                                                         
	int signal_name, syscall;
	
	signal_name = pcb[run_pid].tf_p->ebx;
	syscall = *pcb[run_pid].tf_p->ecx;
		
	pcb[run_pid].signal_handler[signal_name] = syscall;
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
	tf_t temporaryTF;
	
	tempEFL = &pcb[pid].tf_p->efl;
	temporaryTF = *pcb[pid].tf_p;
	
	*tempEFl = temporaryTF.eip;				
	*(int *) temporaryTF.eip = *p;			// Replacing EIP in trapframe with 'p'
	pcb[pid].tf_p = pcb[pid].tf_p - 4;		// Lowering trapframe by 4 bytes [1 Register]
	pcb[pid].tf_p = temporaryTF;			// Insert the original EIP between lowered trapframe and what originally above
}
