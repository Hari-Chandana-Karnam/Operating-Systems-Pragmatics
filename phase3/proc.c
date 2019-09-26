// proc.c, 159
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()

unsigned short *upper_left_pos = (unsigned short *)0xb8000;
int flagForDot = 1; 

void Idle(void)
{
    while(1)
    {     
        if((sys_time_count % 100) == 0) //Using mod to detmine the multiple of 100.
        {
            if (flagForDot == 1)
            {
                *upper_left_pos = '*' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                flagForDot = 0;
            }else if (flagForDot == 0)
            {
                *upper_left_pos = ' ' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                flagForDot = 1;
            }
        }
    }
}

void Init(void) {  				// Init, PID 1, asks/tests various OS services
   int my_pid, os_time;  			//declare two integers: my_pid & os_time
   char pid_str[QUE_MAX], time_str[QUE_MAX]; 	//declare two 20-char arrays: pid_str & time_str
   my_pid = sys_get_pid();  			//call sys_get_pid() to get my_pid
   Number2Str(my_pid, pid_str);		    	//call Number2Str() to convert it to pid_str
   while(1)
   {
        sys_write("My PID is ");            //call sys_write() to show "my PID is "
        sys_write(pid_str);                 //call sys_write() to show my pid_str
        sys_write("... ");                  //call sys_write to show "... "
        sys_sleep(1);                       //call sys_sleep() to sleep for 1 second
        os_time = sys_get_time();           //call sys_get_time() to get current os_time
        Number2Str(os_time, time_str);      //call Number2Str() to convert it to time_str
        sys_write("Sys time is ");          //call sys_write() to show "sys time is "
        sys_write(time_str);                //call sys_write() to show time_str
        sys_write("... ");                  //call sys_write to show "... "
        sys_sleep(1);
   }
}
