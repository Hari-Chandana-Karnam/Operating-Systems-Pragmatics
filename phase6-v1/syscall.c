// syscall.c
// system service calls for kernel services

#include "const-type.h"     // for SYS_GET_PID, etc., below

int sys_get_pid(void) 
{ 
    int pid;
    asm("movl %1, %%eax;     
        int $128;           
        movl %%ebx, %0"     
       : "=g" (pid)         				// output from asm()
       : "g" (SYS_GET_PID)  				// input to asm()
       : "eax", "ebx"       				// clobbered registers
    );
    return pid;
}

int sys_get_time(void) 
{ 
    int time;
    asm("movl %1, %%eax;       
        int $128;             
        movl %%ebx, %0"       
       : "=g" (time)          				// output from asm()
       : "g" (SYS_GET_TIME)   				// input to asm()
       : "eax", "ebx"         				// clobbered registers
    );
    return time;
}

void sys_sleep(int sleep_sec) 
{
    asm("movl %0, %%eax;          
        movl %1, %%ebx;          
        int $128"                
       :                         	   		// no output from asm()
       : "g" (SYS_SLEEP), "g" (sleep_sec)  		// 2 inputs to asm()
       : "eax", "ebx"            	   		// clobbered registers
    );
}

void sys_write(char *write_str) 
{
    asm("movl %0, %%eax;                      
        movl %1, %%ebx;                      
        int $128"                            
       :                                     		// no output from asm()
       : "g" (SYS_WRITE), "g" (write_str)    		// 2 inputs to asm()
       : "eax", "ebx"                        		// clobbered registers
    );
}

void sys_set_cursor(int row, int column) 
{  
    int cursor_position;
    cursor_position = (row*80 + column);
    asm("movl %0, %%eax;		     
        movl %1, %%ebx; 
        int $128"                
       :
       : "g"(SYS_SET_CURSOR), "g" (cursor_position)
       : "eax", "ebx"
    );
}

int sys_fork(void) 
{
    int forked_pid;
    asm("movl %1, %%eax;
        int $128;
        movl %%ebx, %0"       
       : "=g" (forked_pid)  				// output from asm()
       : "g" (SYS_FORK)     				// input to asm()
       : "eax", "ebx"       				// clobbered registers
    );
    return forked_pid;
}

unsigned int sys_get_rand(void) 
{ 
    unsigned int rand;
    asm("movl %1, %%eax;     
        int $128;           
        movl %%ebx, %0"     
       : "=g" (rand)         				// output from asm()
       : "g" (SYS_GET_RAND)  				// input to asm()
       : "eax", "ebx"       				// clobbered registers
    );
    return rand;
}

void sys_lock_mutex(int mutex_id)
{
    asm("movl %0, %%eax;          
        movl %1, %%ebx;          
        int $128"                
       :                         	       			// no output from asm()
       : "g" (SYS_LOCK_MUTEX), "g" (mutex_id)		// 2 inputs to asm()
       : "eax", "ebx"            	   				// clobbered registers
    );
}

void sys_unlock_mutex(int mutex_id)
{
    asm("movl %0, %%eax;          
        movl %1, %%ebx;          
        int $128"                
       :                         	   				// no output from asm()
       : "g" (SYS_UNLOCK_MUTEX), "g" (mutex_id) 	// 2 inputs to asm()
       : "eax", "ebx"            	   				// clobbered registers
    );
}

void sys_exit(int exit_code) 	// phase 5
{
    asm("movl %0, %%eax;          
        movl %1, %%ebx;          
        int $128"                
       :                         	   		// no output from asm()
       : "g" (SYS_EXIT), "g" (exit_code) 	// 2 inputs to asm()
       : "eax", "ebx"            	   		// clobbered registers
    );
}

int sys_wait(int *exit_code)	// phase 5
{
    int cpid;
    asm("movl %1, %%eax;          
        movl %2, %%ebx;          
        int $128;
		movl %%ecx, %0"
       : "=g" (cpid)                    	   	// 1 output from asm()
       : "g" (SYS_WAIT), "g" (exit_code) 		// 2 inputs to asm()
       : "eax", "ebx", "ecx"            	   	// clobbered registers
    );
    return cpid;
}

void sys_signal(int signal_name, func_p_t p)	// phase 6
{
	// for a process to 'register' a function p as the handler for a certain signal
	asm("movl %0, %%eax;          
        movl %1, %%ebx; 
		movl %2, %%ecx;
        int $128"
       :                     	   						// no output from asm()
       : "g" (SYS_SIGNAL), "g" (signal_name), "g" (p) 	// 3 inputs to asm()
       : "eax", "ebx", "ecx"            	   			// clobbered registers
    );
}

void sys_kill(int pid, int signal_name)		// phase 6
{
	// for a process to send a signal to a process (or all in the same process group)
	asm("movl %0, %%eax;          
        movl %1, %%ebx; 
		movl %2, %%ecx;
        int $128"
       :                     	   						// no output from asm()
       : "g" (SYS_KILL), "g" (pid), "g" (signal_name) 	// 3 inputs to asm()
       : "eax", "ebx", "ecx"            	   			// clobbered registers
    );
}
