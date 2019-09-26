// ksr.c, 159

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "proc.h"

void SpawnSR(func_p_t p) {     // arg: where process code starts
   int pid;
   /*use a tool function to check if available queue is empty:
      a. cons_printf("Panic: out of PID!\n");
      b. and go into GDB*/
   if(QueEmpty(&avail_que))
   {
      cons_printf("Panic: out of PID!\n");
      breakpoint(); //Calling breakpoint(); to enter GDB
   }

   //get 'pid' initialized by dequeuing the available queue
   pid = DeQue(&avail_que); //Fill this function with the available que
   
   //use a tool function to clear the content of PCB of process 'pid'
   Bzero((char *) &pcb[pid], sizeof(pcb_t));
   
   //set the state of the process 'pid' to READY
   pcb[pid].state = READY;
   
   //if 'pid' is not IDLE, use a tool function to enqueue it to the ready queue 
   if(pid != IDLE)
   {
      EnQue(pid, &ready_que);
   }
   
   // copy code to DRAM, both code & stack are separated among processes, phase2
   MemCpy((char *)(DRAM_START + (pid*STACK_MAX)), (char *) p, STACK_MAX);
   // point tf_p to stack & fill TF out
   pcb[pid].tf_p = (tf_t *)(DRAM_START + (pid + 1)*STACK_MAX - sizeof(tf_t));
   pcb[pid].tf_p->efl = EF_DEFAULT_VALUE | EF_INTR;   //set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
   pcb[pid].tf_p->cs  = get_cs();                     //set cs in trapframe to return of calling get_cs() // duplicate from CPU
   pcb[pid].tf_p->eip = (DRAM_START + (pid*STACK_MAX));                   //set eip in trapframe to DRAM_START                // where code copied
	//bpreakpoint();
}

// count run time and switch if hitting time limit
void TimerSR(void) {
   int i;
   outportb(PIC_CONT_REG, TIMER_SERVED_VAL); //1st notify PIC control register that timer event is now served
   sys_time_count++;                         //increment system time count by 1
   pcb[run_pid].time_count++;                //increment the time count of the process currently running by 1
   pcb[run_pid].total_time++;//increment the life span count of the process currently running by 1
   
   for(i = 0; i < QUE_MAX; i++)
   {
	if((pcb[i].state == SLEEP) && (pcb[i].wake_time <= sys_time_count))
	{
		EnQue(i, &ready_que);
		pcb[i].state = READY;
	}
   }

  if(pcb[run_pid].time_count == TIME_MAX) {  // if runs long enough
      pcb[run_pid].state = READY;
      EnQue(run_pid, &ready_que);
      run_pid = NONE;
   }
}

void SyscallSR(void) {
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

void SysSleep(void) {
   int sleep_sec = pcb[run_pid].tf_p->ebx;
   pcb[run_pid].wake_time = sys_time_count + sleep_sec*100;
   pcb[run_pid].state = SLEEP;
   run_pid = NONE;
}

void SysWrite(void) {
   char *str = (char *) pcb[run_pid].tf_p->ebx;
   int i = 0;
   while(str[i] != '\0')            //show the str one char at a time (use a loop)
   {
	if(sys_cursor == VIDEO_END)
	{
	     sys_cursor = VIDEO_START;	
	}
	cons_printf("%i",str[i]);
	i++;
   }
}
