// ksr.c, 159

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include <spede.h>
#include <const-type.h>
#include <ext-data.h>
#include <tools.h>
#include <ksr.h>

// to create a process: alloc PID, PCB, and process stack
// build trapframe, initialize PCB, record PID to ready_que
void SpawnSR(func_p_t p) {     // arg: where process code starts
   int pid;

   /*use a tool function to check if available queue is empty:
      a. cons_printf("Panic: out of PID!\n");
      b. and go into GDB*/
   if(QueEmpty(&pid_que))
   {
      cons_printf("Panic: out of PID!\n");
      breakpoint(); //Calling breakpoint(); to enter GDB
      return;
   }

   //get 'pid' initialized by dequeuing the available queue
   pid = DeQue(&pid_que); //Fill this function with the available que
   
   //use a tool function to clear the content of PCB of process 'pid'
   Bzero((char *) &pcb[pid], sizeof(pcb_t));

   //set the state of the process 'pid' to READY
   pcb[pid].state = READY;
   
   //if 'pid' is not IDLE, use a tool function to enqueue it to the ready queue 
   if(pid != IDLE)
   {
      EnQue(pid, &ready_que);
   }
   

   //use a tool function to copy from 'p' to DRAM_START, for STACK_MAX bytes
   MemCpy((char *) DRAM_START, (char *) IDLE, STACK_MAX); //Not sure whether we need IDLE or soem other Idle.
   
   /*create trapframe for process 'pid:'
   1st position trapframe pointer in its PCB to the end of the stack*/
   pcb[pid].tf_p = (tf_t *)(DRAM_START + STACK_MAX - sizeof(tf_t));
   
   pcb[pid].tf_p->efl = EF_DEFAULT_VALUE | EF_INTR;   //set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
   pcb[pid].tf_p->cs  = get_cs();                     //set cs in trapframe to return of calling get_cs() // duplicate from CPU
   pcb[pid].tf_p->eip =  DRAM_START;                  //set eip in trapframe to DRAM_START                // where code copied

}

// count run time and switch if hitting time limit
void TimerSR(void) 
{
   outportb(PIC_CONT_REG, TIMER_SERVED_VAL); //1st notify PIC control register that timer event is now served
   sys_time_count++;                         //increment system time count by 1
   pcb[run_pid].time_count++;                //increment the time count of the process currently running by 1
   pcb[run_pid].total_count++;               //increment the life span count of the process currently running by 1

   /*if the time count of the process is reaching maximum allowed runtime
      move the process back to the ready queue
      alter its state to indicate it is not running but ...
      reset the PID of the process in run to NONE*/
   if(pcb[run_pid].time_count == TIME_MAX)
   {
      pcb[run_pid].state = READY;
      EnQue(run_pid, &ready_que);
      run_pid = NONE;
   }
}
