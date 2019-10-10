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
      	default:
        	cons_printf("Kernel Panic: no such syscall!\n");
        	breakpoint();
   }	
	
   /*if run_pid is not NONE, we penalize it by
      a. downgrade its state to READY
      b. moving it to the back of the ready-to-run process queue
      c. reset run_pid (is now NONE)*/
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
   	pcb[run_pid].wake_time = sys_time_count + (sleep_sec * 10);   //Updated this from 100 to 10. Faster now.
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
    int exit_code, cpid;
	
	exit_code = *pcb[run_pid].tf_p->ebx; // Have to dereference ebx as we pass pointer to sys_exit.
	cpid = pcb[run_pid].tf_p->ecx;
	
	if(WAIT != pcb[run_pid].state)
    {
        /*running process cannot exit, it becomes a zombie
        no running process anymore*/
        pcb[cpid].state = ZOMBIE;
    }
    else
    {
    	pcb[run_pid].state = READY;	// upgrade parent's state
		EnQue(run_pid, &ready_que);	// move parent to be ready to run again
				
		pcb[run_pid].tf_p->ecx = cpid;	//pass over exiting PID to parent
		pcb[run_pid].tf_p->ebx = exit_code;	//pass over exit code to parent
		
		//reclaim child resources (alter state, move it)
        //no running process anymore
    }
}

void SysWait(void)
{
	//search for any child that called to exit?
      	if(){//if found one:
         	pcb[run_pid].tf_p->ecx = cpid;//pass over its PID to parent
         	pcb[run_pid].tf_p->ebx = exit_code;//pass over its exit code to parent
         	//reclaim child resources (alter state, move it) */
		else{//if not any found:
         	pcb[run_pid].state = WAIT;//parent is blocked into WAIT state
         	run_pid = NONE;//no running process anymore
		}
}
