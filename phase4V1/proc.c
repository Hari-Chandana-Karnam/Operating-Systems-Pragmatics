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
            }else if (flagForDot == 0)
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
   	int my_pid, column, rand, forked_pid;
   	char pid_str[PROC_MAX]; //PROC_MAX == QUE_MAX == 20
	
   	forked_pid = sys_fork();
   	if(forked_pid == NONE)
    	sys_write("sys_fork() failed!\n");
   	forked_pid = sys_fork();
   	if(forked_pid == NONE)
       	sys_write("sys_fork() failed!\n");
   
   	my_pid = sys_get_pid();               // what's my PID
	Number2Str(my_pid, pid_str);          // convert # to str

	while(1) 
	{
		column=0;
		while (column!=70)
		{
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, column);
			sys_write(pid_str);
			sys_unlock_mutex(VIDEO_MUTEX);			
			rand =  1 + ((sys_get_rand()/my_pid) % 4);
			sys_sleep(rand);		
			column++;
		}

		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, 0);
		while (column--)
		{
			sys_write(" ");
		}
		sys_unlock_mutex(VIDEO_MUTEX);
        
		sys_sleep(30);		
	}
}        	

