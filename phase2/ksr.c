// ksr.c, 159

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "proc.h"

void SpawnSR(func_p_t p) {     // arg: where process code starts
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

void TimerSR(void) {
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
   while(str[i] != '\0')
   {	
	if(sys_cursor == VIDEO_END)
	     sys_cursor = VIDEO_START;	
	*sys_cursor = str[i]+VGA_MASK_VAL;
	sys_cursor++;
	i++;
   }
}

