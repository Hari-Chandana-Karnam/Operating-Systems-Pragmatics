// ksr.c, 159

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "proc.h"

void SpawnSR(func_p_t p) // arg: where process code starts
{     
   int pid;

   if(QueEmpty(&avail_que))
   {
      cons_printf("Panic: out of PID!\n");
      breakpoint(); //Calling breakpoint(); to enter GDB
   }

   pid = DeQue(&avail_que);
   Bzero((char *) &pcb[pid], sizeof(pcb_t));
   pcb[pid].state = READY;
   
   if(pid != IDLE)
   {
      EnQue(pid, &ready_que);
   }
   
   MemCpy((char *)(DRAM_START + (pid*STACK_MAX)), (char *) p, STACK_MAX);
   pcb[pid].tf_p = (tf_t *)(DRAM_START + (pid + 1)*STACK_MAX - sizeof(tf_t));
   pcb[pid].tf_p->efl = EF_DEFAULT_VALUE | EF_INTR;	//set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
   pcb[pid].tf_p->cs  = get_cs();                     	//set cs in trapframe to return of calling get_cs() // duplicate from CPU
   pcb[pid].tf_p->eip = (DRAM_START + (pid*STACK_MAX)); //set eip in trapframe to DRAM_START                // where code copied
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
      default:
         cons_printf("Kernel Panic: no such syscall!\n");
         breakpoint();
   }
}

void SysSleep(void) 
{
   int sleep_sec = pcb[run_pid].tf_p->ebx;
   pcb[run_pid].wake_time = sys_time_count + (sleep_sec * 100);
   pcb[run_pid].state = SLEEP;
   run_pid = NONE;
}

void SysWrite(void) 
{
   char *str = (char *) pcb[run_pid].tf_p->ebx;
   int i = 0;
   while(str[i] != '\0') //While we do not hit the null pointer
   {	
	if(sys_cursor == VIDEO_END)
	     sys_cursor = VIDEO_START;	
	*sys_cursor = str[i] + VGA_MASK_VAL;
	sys_cursor++;
	i++;
   }
}

/*changes sys_cursor to the position of the row and column numbers
   in the trapframe CPU registers (as inserted when called by Init).
   Hint: the video memory address for row 0, column 0 is the VIDEO_START.*/
void SysSetCursor(void) 
{
    int row = pcb[run_pid].tf_p->eax;
    int column = pcb[run_pid].tf_p->ebx;
    
    *sys_cursor = VIDEO_START; //Set the sys_cursor to the very top left corner.
    sys_cursor += row * 80 + column;
}

void SysFork(void) 
{
    //1. allocate a new PID and add it to ready_que (similar to start of SpawnSR)
    int pid;
    if(QueEmpty(&avail_que))
    {
        cons_printf("Panic: out of PID!\n");
        breakpoint(); //Calling breakpoint(); to enter GDB
    }
    pid = DeQue(&avail_que);
    Bzero((char *) &pcb[pid], sizeof(pcb_t));
    pcb[pid].state = READY;   
    if(pid != IDLE)
        EnQue(pid, &ready_que);

    /*2. copy PCB from parent process, but alter these:
         process state, the two time counts, and ppid*/
    pcb[pid].tf_p = pcb[run_pid].tf_p;	//Copying PCB from run_pid (Parent's Process ID)
    pcb[pid].state = READY;		//Changing the state of the child to READY.
    pcb[pid].time_count = 0;		//Resetting the time_count.
    pcb[pid].total_time = 0;		//Resetting the total_time.
    pcb[pid].ppid = run_pid;		//Changinhg the PPID to the run_pid.
    
    /* 3. copy the process image (the 4KB DRAM) from parent to child:
          figure out destination and source byte addresses
          use tool MemCpy() to do the copying*/
    MemCpy((char *) (DRAM_START + pid*STACK_MAX), (char *) (DRAM_START + pid*STACK_MAX + STACK_MAX), STACK_MAX);    // <--Do not know what to do about this guy, yet.
	
    /*4. calculate the byte distance between the two processes
         = (child PID - parent PID) * 4K*/
    int distance = (pid - run_pid) * (STACK_MAX);
    
    /*5. apply the distance to the trapframe location in child's PCB*/
    *pcb[pid].tf_p = *pcb[run_pid].tf_p + distance;	
	
    /*6. use child's trapframe pointer to adjust these in the trapframe:
         eip (so it points to child's own instructions),
         ebp (so it points to child's local data),
         also, the value where ebp points to:
            treat ebp as an integer pointer and alter what it points to*/
	
    /*7. correctly set return values of sys_fork():
         ebx in the parent's trapframe gets the new child PID
         ebx in the child's trapframe gets ?*/
   
} 
