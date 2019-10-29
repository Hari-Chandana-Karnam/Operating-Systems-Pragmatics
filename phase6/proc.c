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

//Init for phase 5
void Init(void) {
   	int my_pid, column, forked_pid, i, exit_pid, exit_code, sleep_period; 
	int total_sleep_period;
   	char pid_str[PROC_MAX], str[PROC_MAX];
	
	sys_signal();//parent calls sys_signal() to 'register' the above handler
       //(when SIGCHLD/child exiting occurs, the handler will run)
	
	for(i = 0; i < 5; i++) 
	{
		forked_pid = sys_fork(); // is forked_pid used elsewhere only get final fork's pid
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

	my_pid = sys_get_pid();               // call sys_get_pid() to get my PID
	Number2Str(my_pid, pid_str);          // convert my PID to a string

	if(forked_pid > 0)   //if I'm the pround parent { //going by the loop above. Child will have forked_pid == 0 
	{	
		sys_sleep(1);//sleep for a second (probably it's 10 by now)
          	sys_kill();//call sys_kill() to send SIGCONT to all child processes (0)

         	while(1){// parent runs infinite loop 
             		sys_lock_mutex(VIDEO_MUTEX);//lock video mutex
             		sys_set_cursor(my_pid, 0);//set video cursor to the start of my row
             		sys_write(pid_str);//write my PID string
             		sys_unlock_mutex(VIDEO_MUTEX);	//unlock video mutex

             		sys_sleep(1);//sleep for a second

             		sys_lock_mutex(VIDEO_MUTEX);//lock video mutex
             		sys_set_cursor(my_pid, 0);//set video cursor to the start of my row
             		sys_write("-");//write a dash symbol (add another if needed)
             		sys_unlock_mutex(VIDEO_MUTEX);//unlock video mutex

            		sys_sleep(1); //sleep for a second
          		}
      		 }
	else
	{
		sys_sleep(1000000);//child sleeps for 1000000     // child code continues here
		column = 0; //reset col & total sleep period
		total_sleep_period = 0;
		while(column < 70)//while col is less than 70 {
		{			
			sys_lock_mutex(VIDEO_MUTEX);//lock video mutex
			sys_set_cursor(my_pid, column);  //set video cursor to my row, col
			sys_write(pid_str);//write my PID string
			sys_unlock_mutex(VIDEO_MUTEX);//unlock video mutex		
		
			sleep_period =  1 + ((sys_get_rand()/my_pid) % 4);//call and get a random sleep period 1~4
			sys_sleep(sleep_period);// sleep for that period
			total_sleep_period += sleep_period;//add it to total sleep period
			sys_lock_mutex(VIDEO_MUTEX);//lock video mutex
			sys_set_cursor(my_pid, column); //set video cursor to my row, col
			sys_write(".");//write a dot symbol (add another if needed)
			sys_unlock_mutex(VIDEO_MUTEX);// unlock video mutex
			column++;//increment col by 1
		}
		sys_exit(total_sleep_period);//child exits with total sleep period		
	} 	
}

void MyChildExitHandler(void)
{
	int exitCode;
	int exit_cpid = sys_wait(&exitCode);//call sys_wait() to get exiting child PID and exit code
	int mpid = sys_get_pid(); //call sys_get_pid() to get my PID
	
      	Number2Str(exit_cpid, pid_str1); //convert exiting child pid to a string
      	Number2Str(exit_cpid, pid_str2); //convert exiting code to another string

     	sys_lock_mutex(VIDEO_MUTEX);//lock the video mutex
      	sys_set_cursor(exit_cpid, 72); //set the video cursor to row: exiting child pid, column: 72
      	sys_write(pid_str1);//write 1st string
     	sys_write(":");// write ":"
      	sys_write(pid_str2);//write 2nd string
      	sys_unlock_mutex(VIDEO_MUTEX);//unlock the video mutex
}
