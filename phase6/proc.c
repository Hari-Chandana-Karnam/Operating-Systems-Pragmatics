// proc.c, 159
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "spede.h"
unsigned short *upper_left_pos = (unsigned short *)0xb8000;
int flagForDot = 1; 

void Idle(void)
{
    int i;
    while(1)
    {     
		sys_rand_count++;        
		if((sys_time_count % 100) == 0) //Using mod to detmine the multiple of 100.
        {
            if (flagForDot == 1)
            {
                *upper_left_pos = '*' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                for(i = 0; i < 50; i++) //Added this loop to delay the rocess for half a second. This may stop te flickering
                {
                    asm("sti");
                }
                flagForDot = 0;
            }
			else if (flagForDot == 0)
            {
                *upper_left_pos = ' ' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                for(i = 0; i < 50; i++) //Added this loop to delay the rocess for half a second. This may stop te flickering
                {
                	asm("sti");
                }
                flagForDot = 1;
            }
        }
    }
}

void MyChildExitHandler(void)
{
	int exit_code, exit_cpid;
	char pid_str[PROC_MAX], str[PROC_MAX];
	
	exit_cpid = sys_wait(&exit_code);				
	
    Number2Str(exit_cpid, pid_str); 	
    Number2Str(exit_code, str); 	

    sys_lock_mutex(VIDEO_MUTEX);		
    sys_set_cursor(exit_cpid, 72); 		
    sys_write(pid_str);					
    sys_write(":");						
    sys_write(str);						
    sys_unlock_mutex(VIDEO_MUTEX);
}

void Init(void) {
   	int my_pid, column, forked_pid, i, sleep_period; 
	int total_sleep_period;
   	char pid_str[PROC_MAX];
	
   sys_signal(SIGCHLD, MyChildExitHandler);

	for(i = 0; i < 5; i++) 
	{
		forked_pid = sys_fork();
		if(forked_pid == 0)
		{
			break;
		}
   		else if(forked_pid == NONE)
		{
			sys_write("sys_fork() failed!\n");
			sys_exit(NONE);
		}
	}
	
	my_pid = sys_get_pid(); 
	Number2Str(my_pid, pid_str);

	if(forked_pid > 0) 
	{	
		sys_sleep(10);			
		sys_kill(0, SIGCONT);

		while(1)
		{
        	sys_lock_mutex(VIDEO_MUTEX);
            sys_set_cursor(my_pid, 0);
            sys_write(pid_str);
            sys_unlock_mutex(VIDEO_MUTEX);
            
			sys_sleep(10);
            
			sys_lock_mutex(VIDEO_MUTEX);
            sys_set_cursor(my_pid, 0);
            sys_write("-");
			if(my_pid > 9)
				sys_write("-");
            sys_unlock_mutex(VIDEO_MUTEX);
       		
			sys_sleep(10);
   		}
    }
	
	sys_sleep(1000000);
	column = 0;
	total_sleep_period = 0;
	while(column < 70)
	{			
		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, column);
		sys_write(pid_str);
		sys_unlock_mutex(VIDEO_MUTEX);	
		
		sleep_period =  1 + ((sys_get_rand()/my_pid) % 4);
		sys_sleep(sleep_period);
		total_sleep_period += sleep_period;
		
		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, column);
		sys_write(".");
		if(my_pid > 9)
		{
			sys_set_cursor(my_pid, column+1);
			sys_write(" ");
		}
		sys_unlock_mutex(VIDEO_MUTEX);
		
		column++;
	}
	sys_exit(total_sleep_period); 	
}


