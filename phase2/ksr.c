// ksr.c, 159

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"

void SpawnSR(func_p_t p) {     // arg: where process code starts
   int pid;
   /*use a tool function to check if available queue is empty:
      a. cons_printf("Panic: out of PID!\n");
      b. and go into GDB*/
   if(QueEmpty(&avail_que))
   {
      cons_printf("Panic: out of PID!\n");
      breakpoint(); //Calling breakpoint(); to enter GDB
      return;
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
   MemCpy((char *)DRAM_START + ...?

   // point tf_p to stack & fill TF out
   pcb[pid].tf_p = (tf_t *)(DRAM_START + STACK_MAX - sizeof(tf_t));
   pcb[pid].tf_p->efl = EF_DEFAULT_VALUE | EF_INTR;   //set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
   pcb[pid].tf_p->cs  = get_cs();                     //set cs in trapframe to return of calling get_cs() // duplicate from CPU
   pcb[pid].tf_p->eip = DRAM_START;                   //set eip in trapframe to DRAM_START                // where code copied
}

// count run time and switch if hitting time limit
void TimerSR(void) {
   outportb(PIC_CONT_REG, TIMER_SERVED_VAL); //1st notify PIC control register that timer event is now served
   sys_time_count++;                         //increment system time count by 1
   pcb[run_pid].time_count++;                //increment the time count of the process currently running by 1
   pcb[run_pid].total_time++;                //increment the life span count of the process currently running by 1
   
   //if run_pid is IDLE, just simply return;    // Idle exempt from below, phase2
   if(run_pid == IDLE)
   {
      return;
   }

   //Use a loop to look for any processes that need to be waken up!                                     <-- need work here.  
         
   if(pcb[run_pid].time_count == TIME_MAX) {  // if runs long enough
      pcb[run_pid].state = READY;
      EnQue(run_pid, &ready_que);
      run_pid = NONE;
   }
}

void SyscallSR(void) {
   //switch by the eax in the trapframe pointed to by pcb[run_pid].tf_p
   switch(pcb[run_pid].tf_p->eax)
   {
       /*if it's  SYS_GET_PID,
            copy run_pid to ebx in the trapframe of the running process 
         if it's SYS_GET_TIME,
            copy the system time count to ebx in the trapframe of the running process
         if it's SYS_SLEEP,
            call SysSleep()
         if it's SYS_WRITE
            call SysWrite()
         default:
            cons_printf("Kernel Panic: no such syscall!\n");
            breakpoint();*/
      case SYS_GET_PID:
         pcb[pid].tf_p->ebx = run_pid;
         break;
      case SYS_GET_TIME:
         pcb[pid].tf_p->ebx = sys_time_count;
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
   int sleep_sec = ... from a register value wtihin the trapframe
   calculate the wake time of the running process using the current system
   time count plus the sleep_sec times 100
   alter the state of the running process to SLEEP
   alter the value of run_pid to NONE
}

void SysWrite(void) {
   char *str =  ... passed over by a register value wtihin the trapframe
   show the str one char at a time (use a loop)
      onto the console (at the system cursor position)
      (while doing so, the cursor may wrap back to the top-left corner if needed)
}

