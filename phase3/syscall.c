// syscall.c
// system service calls for kernel services

#include "const-type.h"     // for SYS_GET_PID, etc., below


int sys_get_pid(void) { 
   int pid;
   asm("movl %1, %%eax;     
        int $128;           
        movl %%ebx, %0"     
       : "=g" (pid)         // output from asm()
       : "g" (SYS_GET_PID)  // input to asm()
       : "eax", "ebx"       // clobbered registers
   );
   return pid;
}

int sys_get_time(void) { 
   int time;
   asm("movl %1, %%eax;       
        int $128;             
        movl %%ebx, %0"       
       : "=g" (time)          // output from asm()
       : "g" (SYS_GET_TIME)   // input to asm()
       : "eax", "ebx"         // clobbered registers
   );
   return time;
}

void sys_sleep(int sleep_sec) {
   asm("movl %0, %%eax;          
        movl %1, %%ebx;          
        int $128"                
       :                         	   // no output from asm()
       : "g" (SYS_SLEEP), "g" (sleep_sec)  // 2 inputs to asm()
       : "eax", "ebx"            	   // clobbered registers
   );
}

void sys_write(char *write_sec) {
   asm("movl %0, %%eax;                      
        movl %1, %%ebx;                      
        int $128"                            
       :                                     // no output from asm()
       : "g" (SYS_WRITE), "g" (write_sec)    // 2 inputs to asm()
       : "eax", "ebx"                        // clobbered registers
   );
}
