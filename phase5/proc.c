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

//Init for phase 3
void Init(void) {  // Init, PID 1, asks/tests various OS services
   	int my_pid, column, rand, forked_pid,i, col, exit_pid, exit_code,sleep_period, total_sleep_period;
   	char pid_str[PROC_MAX],str[PROC_MAX];

 /*  	forked_pid = sys_fork();
   	if(forked_pid == NONE)
    	sys_write("sys_fork() failed!\n");
	forked_pid = sys_fork();
   	if(forked_pid == NONE)
     	sys_write("sys_fork() failed!\n"); */  
	
	for(i=0;i<5;i++) 
	{
		forked_pid = sys_fork();// is forked_pid used elsewhere only get final fork's pid
   		if(NONE == forked_pid )
		{
			sys_write("sys_fork() failed!\n");
			Sys_Exit(NONE);
		}
	}
	
	my_pid = sys_get_pid();               // what's my PID
	Number2Str(my_pid, pid_str);          // convert # to str
	/*get my_pid
    convert it to pid_str
    for the one parent process, it does */
	if ()
	{	
		for(i=0;i<5;i++)
		{
			sys_wait(exit_code);
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, column);
			sys_write("PID ");
			Number2Str(exit_pid, str[0]);
			sys_write(str);
			sys_write(": ");
			Number2Str(exit_code, str[0]);
			sys_write(str);
			sys_unlock_mutex(VIDEO_MUTEX);
		}
		sys_write("Init exits");	
		SYS_EXIT(0);
	}
	/*// child code below, similar to prev, race across screen
      col = total_sleep_period = 0;
      while col is less than 70 {
         lock video mutex
         set cursor to my row, column col
         write pid_str
         unlock video mutex

         get a random sleep_period = ... // get rand # 1~4
         sleep with that random period
         add random period to total_sleep_period
         col++;
      }
      call exit sesrvice with total_sleep_period as exit code
   }*/

	

	//while(1) 
	//{
		column = total_sleep_period = 0;
		while(column != 70)
		{			
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, column);
			sys_write(pid_str);
			sys_unlock_mutex(VIDEO_MUTEX);			
			rand =  1 + ((sys_get_rand()/my_pid) % 4);
			sys_sleep(rand);
			total_sleep_period = total_sleep_period + rand;
			column++;
		}
        exit_service(total_sleep_period);// not sure which service to call 
		//call exit sesrvice with total_sleep_period as exit code
		//sys_sleep(30);		
	}
}        	

