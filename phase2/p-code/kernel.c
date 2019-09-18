// kernel.c, 159, phase 1

...

Add the new cursor position that OS keeps

unsigned short *sys_cursor;         // phase2

void BootStrap(void) {              // set up kernel!

   ...

   sys_cursor = ???  // have it set to VIDEO_START in BootStrap()

   ...

   use fill_gate() to set entry # SYSCALL_EVENT to SyscallEntry
}

int main(void) {               // kernel boots

   ... after creating Idle ...

   also create Init

   ...
}

...

void Kernel(tf_t *tf_p) {       // kernel runs
   ...
   ...

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

   if(cons_kbhit()) {           // if keyboard pressed
   ...
   ...
}

