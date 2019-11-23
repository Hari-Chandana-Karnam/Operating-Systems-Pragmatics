// kernel.c, 158, phase 4
//
// Team Name: "RuntimeErrors" (Members: Angad Pal Dhanoa, Dalton Caraway, & Hari Chandana Karnam)

#include "spede.h"
#include "const-type.h"
#include "entry.h"    //entries to kernel (TimerEntry, etc.)
#include "tools.h"    //small handy functions
#include "ksr.h"      //kernel service routines
#include "proc.h"     //all user process code here
#include "syscall.h"

/* declare kernel data */
int run_pid;                  //declare an integer: run_pid;  // current running PID; if -1, none selected
que_t avail_que, ready_que;   //declare 2 queues: avail_que and ready_que;  // avail PID and those created/ready to run
pcb_t pcb[PROC_MAX];          //declare an array of PCB type: pcb[PROC_MAX];  // Process Control Blocks
mutex_t video_mutex;
kb_t kb;
tty_t tty;  		//use tty_t to declare a 'tty' to interface a terminal port
page_t  page[PAGE_MAX];
unsigned int KDir;			  // Kernals's 'real address' - translation directory;
unsigned int sys_time_count;  //declare an unsigned integer: sys_time_count
unsigned short *sys_cursor;   //Add the new cursor position that OS keep
unsigned sys_rand_count;		
struct i386_gate *idt;        //interrupt descriptor table

void BootStrap(void) 
{
   	int i; 
   	sys_time_count = 0;                    		//set sys time count to zero  	
	sys_cursor = VIDEO_START;               	//have it set to VIDEO_START in BootStrap()
	sys_rand_count = 0;							//set sys rand count to zero 
	video_mutex.lock = UNLOCKED;				//set the lock of video_mutex to be UNLOCKED
   	KDir = get_cr3();
	
	Bzero((char *) &avail_que, sizeof(que_t));  				//call tool Bzero() to clear avail queue
   	Bzero((char *) &ready_que, sizeof(que_t));  				//call tool Bzero() to clear ready queue
	Bzero((char *) &video_mutex.suspend_que, sizeof(que_t));	//call tool Bzero() to clear video_mutex's suspend queue
	Bzero((char *) &kb, sizeof(kb_t));
	
   //enqueue all the available PID numbers to avail queue
   for(i = 0; i < QUE_MAX; i++)
   {
      EnQue(i, &avail_que);
   }
	
   /* For the page array:
    * each page is used by NONE, and its
    * page[i].u.addr = DRAM_START + i * PAGE_SIZE where i = 0..PAGE_MAX-1
	*/
   for(i = 0; i <= PAGE_SIZE -1; i++)
   {
	   page[i].pid = NONE;
	   page[i].u.addr = DRAM_START + i * PAGE_SIZE;
   }

   idt = get_idt_base();                                                      	  //get IDT location
   fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);	  //addr of TimerEntry is placed into proper IDT entry
   fill_gate(&idt[SYSCALL_EVENT], (int)SyscallEntry, get_cs(), ACC_INTR_GATE, 0); //Have to program this one properly.
   outportb(PIC_MASK_REG, PIC_MASK_VAL); //send PIC control register the mask value for timer handling
}

int main(void) 
{
   BootStrap();               	//do the boot strap things 1st
   TTYinit();		        //to initialize a terminal port	
   SpawnSR(&Idle);             	//create Idle thread
   SpawnSR(&Login);             	//create Init thread
   run_pid = IDLE;           	//set run_pid to IDLE
   set_cr3(pcb[run_pid].dir);
   Loader(pcb[run_pid].tf_p); 	//call Loader() to load the trapframe of Idle
   return 0; 					//never would actually reach here
}

void Scheduler(void) // choose a run_pid to run
{              
   if(run_pid > IDLE)    
      return;                       //a user PID is already picked
   
   if(QueEmpty(&ready_que)) 
      run_pid = IDLE;               //use the Idle thread
   else 
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

    switch(tf_p->event) 
    {
        case TIMER_EVENT:
            TimerSR();         // handle tiemr event
      	    break;
   		case SYSCALL_EVENT:
      	    SyscallSR();       // all syscalls go here 1st
      	    break;
		case TTY_EVENT:      // phase9
			 TTYSR();
			 break;
		default:
      	    cons_printf("Kernel Panic: no such event!\n");
			breakpoint();
	}
	
	KBSR();
   	Scheduler();               //call Scheduler() to change run_pid if needed
   	set_cr3(pcb[run_pid].dir);
	Loader(pcb[run_pid].tf_p); //call Loader() to load the trapframe of the selected process
}
void TTYinit(void) {                // phase9
      int i, j;
      
      Bzero((char *)&tty, sizeof(tty_t));
      tty.port = TTY0;

      outportb(tty.port+CFCR, CFCR_DLAB);             // CFCR_DLAB is 0x80
      outportb(tty.port+BAUDLO, LOBYTE(115200/9600)); // period of each of 9600 bauds
      outportb(tty.port+BAUDHI, HIBYTE(115200/9600));
      outportb(tty.port+CFCR, CFCR_PEVEN|CFCR_PENAB|CFCR_7BITS);
   
      outportb(tty.port+IER, 0);
      outportb(tty.port+MCR, MCR_DTR|MCR_RTS|MCR_IENABLE);

      for(i=0; i<166667; i++)asm("inb $0x80");       // wait .1 sec
      outportb(tty.port+IER, IER_ERXRDY|IER_ETXRDY); // enable TX & RX intr
      for(i=0; i<166667; i++)asm("inb $0x80");       // wait .1 sec

      for(j=0; j<3; j++) {                           // clear 3 lines
         outportb(tty.port, 'V');
         for(i=0; i<83333; i++)asm("inb $0x80");     // wait .5 sec should do
         outportb(tty.port, '\n');
         for(i=0; i<83333; i++)asm("inb $0x80");
         outportb(tty.port, '\r');
         for(i=0; i<83333; i++)asm("inb $0x80");
      }
      inportb(tty.port);                             // get 1st key PROCOMM logo
      for(i=0; i<83333; i++)asm("inb $0x80");        // wait .5 sec
   }
