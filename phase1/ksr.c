// ksr.c, 159

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include <spede.h>
#include <const-type.h>
#include <ext-data.h>
#include <tools.h>

// to create a process: alloc PID, PCB, and process stack
// build trapframe, initialize PCB, record PID to ready_que
void SpawnSR(func_p_t p) {     // arg: where process code starts
   int pid;

   /*use a tool function to check if available queue is empty:
      a. cons_printf("Panic: out of PID!\n");
      b. and go into GDB*/
   if(QueEmpty(&pid_q))
   {
      cons_printf("Panic: out of PID!\n");
      breakpoint(); //Calling breakpoint(); to enter GDB
      return;
   }

   //get 'pid' initialized by dequeuing the available queue
   pid = DeQue(); //Fill this function with the available que
   
   //use a tool function to clear the content of PCB of process 'pid'
   void Bzero(char *start, Unsigned int bytes) //work needed
   {
      for(i=0; i<Bytes;i++)
      {
         *start=(char)0;
         start++
      }
      
   }
   //set the state of the process 'pid' to READY
   pcb[pid].state = READY; //Work needed.
   
   //if 'pid' is not IDLE, use a tool function to enqueue it to the ready queue 
   if(pid > IDLE)
   {
      EnQue(pid ...); //Work needed.
   }
   
  
               
   use a tool function to copy from 'p' to DRAM_START, for STACK_MAX bytes
      //This might work?
 //void MemCpy((char *) Dram_START, (char *)IDLE, STAT_MAX);
      
      
   create trapframe for process 'pid:'
   1st position trapframe pointer in its PCB to the end of the stack
   set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
      
      pcb[pid].tf_p = (tf_t *)(DRAM_START + STACK_MAX - size of(tf_t0));
   
   set cs in trapframe to return of calling get_cs() // duplicate from CPU
   
   
      
   set eip in trapframe to DRAM_START                // where code copied
}

// count run time and switch if hitting time limit
void TimerSR(void) {
   1st notify PIC control register that timer event is now served

   increment system time count by 1
   increment the time count of the process currently running by 1
   increment the life span count of the process currently running by 1

   if the time count of the process is reaching maximum allowed runtime
      move the process back to the ready queue
      alter its state to indicate it is not running but ...
      reset the PID of the process in run to NONE
   }
}

