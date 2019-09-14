// kernel.c, 159, phase 1
//
// Team Name: "HeWhoShallNotBeNamed" (Members: Angad Pal Dhanoa & Dalton Caraway)

#include "spede.h"
#include "const-type.h"
#include "entry.h"    // entries to kernel (TimerEntry, etc.)
#include "tools.h"    // small handy functions
#include "ksr.h"      // kernel service routines
#include "proc.h"     // all user process code here

/* declare kernel data */
int run_pid;                  //declare an integer: run_pid;  // current running PID; if -1, none selected
que_t avail_que, ready_que;   //declare 2 queues: avail_que and ready_que;  // avail PID and those created/ready to run
pcb_t pcb[PROC_MAX];          //declare an array of PCB type: pcb[PROC_MAX];  // Process Control Blocks

unsigned int sys_time_count;  //declare an unsigned integer: sys_time_count
struct i386_gate *idt;        // interrupt descriptor table

void BootStrap(void) // set up kernel!
{            
   sys_time_count = 0;                          //set sys time count to zero

   Bzero((char *) &avail_que, sizeof(que_t));   //call tool Bzero() to clear avail queue
   Bzero((char *) &ready_que, sizeof(que_t));   //call tool Bzero() to clear ready queue
   
   //enqueue all the available PID numbers to avail queue
   for(int i = 0; i < QUE_MAX; i++)
   {
      EnQue(i, &avail_que);
   }
   
   //The following follow the same structure as Prep 4 of Phase 0.
   idt = get_idt_base();                                                      //get IDT location
   fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0); //addr of TimerEntry is placed into proper IDT entry
   outportb(PIC_MASK_REG, PIC_MASK_VAL);                                      //send PIC control register the mask value for timer handling
}

int main(void) // OS starts
{               
   BootStrap();               //do the boot strap things 1st
   SpawnSR(Idle);             // create Idle thread
   run_pid = IDLE;            //set run_pid to IDLE
   Loader(pcb[run_pid].tf_p); //call Loader() to load the trapframe of Idle
   return 0;                  // never would actually reach here
}

void Scheduler(void) // choose a run_pid to run
{              
   if(run_pid > IDLE) 
   {   
      return;       // a user PID is already picked
   }
   
   if(QueEmpty(&ready_que)) 
   {
      run_pid = IDLE;               // use the Idle thread
   } else 
   {
      pcb[IDLE].state = READY;
      run_pid = DeQue(&ready_que);  // pick a different proc
   }

   pcb[run_pid].time_count = 0;     // reset runtime count
   pcb[run_pid].state = RUN;
}

void Kernel(tf_t *tf_p) // kernel runs 
{       
   pcb[run_pid].tf_p = tf_p;  //copy tf_p to the trapframe ptr (in PCB) of the process in run
   TimerSR();                 //call the timer service routine

   char ch;                   //to read the character from the keyboard.
   if(cons_kbhit())           //Read the key being pressed into ch. If 'b' key on target PC is pressed, goto the GDB prompt.
   {
      ch = cons_getchar();
      if(ch == 'b')	         //If 'b' is pressed, goto the GDB prompt.
      {
         breakpoint();        //breakpoint() is the function used to enter the GDB prompt;
      }
   }

   Scheduler();               //call Scheduler() to change run_pid if needed
   Loader(pcb[run_pid].tf_p); //call Loader() to load the trapframe of the selected process
}
