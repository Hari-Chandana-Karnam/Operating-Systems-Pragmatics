// kernel.c, 158, phase 2
//
// Team Name: "RuntimeErrors" (Members: Angad Pal Dhanoa & Dalton Caraway)

#include "spede.h"
#include "const-type.h"
#include "entry.h"    //entries to kernel (TimerEntry, etc.)
#include "tools.h"    //small handy functions
#include "ksr.h"      //kernel service routines
#include "proc.h"     //all user process code here

/* declare kernel data */
int run_pid;                  //declare an integer: run_pid;  // current running PID; if -1, none selected
que_t avail_que, ready_que;   //declare 2 queues: avail_que and ready_que;  // avail PID and those created/ready to run
pcb_t pcb[PROC_MAX];          //declare an array of PCB type: pcb[PROC_MAX];  // Process Control Blocks

unsigned int sys_time_count;  //declare an unsigned integer: sys_time_count
struct i386_gate *idt;        //interrupt descriptor table
int i; 			               //To use with for loops
char ch;                      //To store character from keyboard

unsigned short *sys_cursor;   //Add the new cursor position that OS keeps

void BootStrap(void) {              // set up kernel!

   sys_time_count = 0;                    //set sys time count to zero
   sys_cursor = VIDEO_START               // have it set to VIDEO_START in BootStrap()
   Bzero((char *) &avail_que, QUE_MAX);   //call tool Bzero() to clear avail queue
   Bzero((char *) &ready_que, QUE_MAX);   //call tool Bzero() to clear ready queue
   
   //enqueue all the available PID numbers to avail queue
   for(i = 0; i < QUE_MAX; i++)
   {
      EnQue(i, &avail_que);
   }
   
   //The following follow the same structure as Prep 4 of Phase 0.
   idt = get_idt_base();                                                      //get IDT location
   fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0); //addr of TimerEntry is placed into proper IDT entry
   outportb(PIC_MASK_REG, PIC_MASK_VAL); //send PIC control register the mask value for timer handling

   //use fill_gate() to set entry # SYSCALL_EVENT to SyscallEntry
   fill_gate(); //Have to program this one.
}

int main(void) {               // kernel boots

   BootStrap();               //do the boot strap things 1st
   SpawnSR(Idle);             //create Idle thread
   SpawnSR(Init);             //create Init thread
   run_pid = IDLE;            //set run_pid to IDLE
   Loader(pcb[run_pid].tf_p); //call Loader() to load the trapframe of Idle
   return 0; //never would actually reach here
}

void Scheduler(void) // choose a run_pid to run
{              
   if(run_pid > IDLE) 
   {   
      return;                       //a user PID is already picked
   }
   if(QueEmpty(&ready_que)) 
   {
      run_pid = IDLE;               //use the Idle thread
   } else 
   {
      pcb[IDLE].state = READY;
      run_pid = DeQue(&ready_que);  //pick a different proc
   }
   pcb[run_pid].time_count = 0;     //reset runtime count
   pcb[run_pid].state = RUN;
}

void Kernel(tf_t *tf_p) // kernel runs
{       
   pcb[run_pid].tf_p = tf_p; //copy tf_p to the trapframe ptr (in PCB) of the process in run

   switch(tf_p->event) {
   case TIMER_EVENT:
      TimerSR();         // handle tiemr event
      break;
   case SYSCALL_EVENT:
      SyscallSR();       // all syscalls go here 1st
      break;
   default:
      cons_printf("Kernel Panic: no such event!\n");
      breakpoint();
   }

   if(cons_kbhit())           //Read the key being pressed into ch. If 'b' key on target PC is pressed, goto the GDB prompt.
   {
      ch = cons_getchar();
      if(ch == 'b')	         //If 'b' is pressed, goto the GDB prompt.
      {
         cons_printf("You Pressed &c. Entering the breakpoint for GDB.\n", ch); //Message for user.
         breakpoint();        //breakpoint() is the function used to enter the GDB prompt.
      }
   }
   
   Scheduler();               //call Scheduler() to change run_pid if needed
   Loader(pcb[run_pid].tf_p); //call Loader() to load the trapframe of the selected process
}

