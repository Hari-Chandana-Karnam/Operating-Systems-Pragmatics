// proc.c, 159
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "spede.h"
#include "proc.h"

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



void ShellDir(void)
{
	
		
	sys_write("     Desktop\r\n");   
	sys_write("     Documents \r\n");   
	sys_write("     Downloads \r\n");
	sys_write("     Music\r\n");  
	sys_write("     Pictures\r\n"); 
	sys_write("     Public\r\n");  
	sys_write("     Templates\r\n");   
	sys_write("     Videos\r\n");   
	sys_exit(0);
}

void ShellCal(void)
{	
	
	
	sys_write("        December 2019     \r\n");
	sys_write("     Su Mo Tu We Th Fr Sa \r\n");
	sys_write("      1  2  3  4  5  6  7 \r\n");
	sys_write("      8  9 10 11 12 13 14 \r\n");
	sys_write("     15 16 17 18 19 20 21 \r\n");
	sys_write("     22 23 24 25 26 27 28 \r\n");
	sys_write("     29 30 31             \r\n");
	sys_exit(0);
}

void ShellRoll(void)
{
	int dice1, dice2;
	
		dice1 = (sys_get_rand() % 6) + 1;
	dice2 = (sys_get_rand() % 6) + 1;
	sys_exit(dice1 + dice2);
}

void Shell (void)
{  	
	int exit_code, exit_cpid;
	char pid_str[PROC_MAX], str[PROC_MAX];
	char input_str[STR_MAX];
	char dir_str[] = "dir";
	char cal_str[] = "cal";
	char roll_str[] = "roll";
    while(1)
	{	
		sys_write("Runtime Errors> ");        
		sys_read(input_str);

		if(StringCompare(dir_str, input_str) == 1)
		{		
			sys_vfork(&ShellDir);	
			exit_cpid = sys_wait(&exit_code);
			Number2Str(exit_cpid, pid_str); 	
		    Number2Str(exit_code, str);
			sys_write(" Exit PID: ");									
			sys_write(pid_str);
			sys_write("     Exit Code: ");
			sys_write(str);
			sys_write("\r\n");
		}
		else if(StringCompare(cal_str, input_str) == 1)
		{		
			sys_vfork(&ShellCal);	
			exit_cpid = sys_wait(&exit_code);
			Number2Str(exit_cpid, pid_str); 	
		    Number2Str(exit_code, str);
			sys_write(" Exit PID: ");									
			sys_write(pid_str);
			sys_write("     Exit Code: ");
			sys_write(str);
			sys_write("\r\n");
		}
		else if(StringCompare(roll_str, input_str) == 1)
		{		
			sys_vfork(&ShellRoll);	
			exit_cpid = sys_wait(&exit_code);
			Number2Str(exit_cpid, pid_str); 	
		    Number2Str(exit_code, str);
			sys_write(" Exit PID: ");									
			sys_write(pid_str);
			sys_write("     Exit Code: ");
			sys_write(str);
			sys_write("\r\n");
		}
		else
		{
			int row, column;
			if(sys_cursor >= VIDEO_START + 20*80 )
			{
				row = 0;
				sys_cursor = VIDEO_START;
				while(row < 25)
				{
					column = 0;
					while(column < 80)
					{
						*sys_cursor = ' ' + VGA_MASK_VAL;
						sys_cursor++;
						column++;
					}
					row++;
				}
				sys_cursor = VIDEO_START;
				sys_write(" Wrapped cursor last command was invalid\r\n");
			}				
			sys_write("     Valid commands are:\r\n");
			sys_write("          dir  -- display directory cotent.\r\n");
			sys_write("          cal  -- display calender.\r\n");
        	sys_write("          roll -- roll a pair of dice.\r\n"); 
		}				
	}
}

void Login(void) 
{
    char login_str[STR_MAX], password_str[STR_MAX];
    
	while(1) 
	{
    	sys_write("Login: ");         
	    sys_read(login_str);
		sys_write("\r\n");
		sys_write("Password: ");
		sys_read(password_str);
		sys_write("\r\n");
		
        if(StringCompare(login_str, password_str) == 1)
		{
			sys_write("Login Successful!\r\n");
			sys_vfork(Shell);
			sys_exit(0);
		}
		else
		{
			sys_write("Login Failed!\r\n");
		}
	}
}



