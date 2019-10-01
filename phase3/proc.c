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
    int i;
    while(1)
    {     
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
   int my_pid, os_time;
   char pid_str[PROC_MAX], time_str[PROC_MAX]; //PROC_MAX == QUE_MAX == 20

   int forked_pid;
   sys_write("Forked pre");
   forked_pid = sys_fork();
   if(forked_pid == NONE)
   {
       sys_write("sys_fork() failed!\n");
   }
   sys_write("Forked 1");
   forked_pid = sys_fork();
   if(forked_pid == NONE)
   {
       sys_write("sys_fork() failed!\n");
   }
   sys_write("Forked 2");
    
   my_pid = sys_get_pid();               // what's my PID
   sys_write("After sys_get_pid");
   Number2Str(my_pid, pid_str);          // convert # to str

   while(1) {
      sys_sleep(1);                       //call sys_sleep() to sleep for 1 second
      sys_set_cursor(my_pid, 0);          //call sys_set_cursor() to set the cursor position.
      sys_write("My PID is ");            //call sys_write() to show "my PID is "
      sys_write(pid_str);                 //call sys_write() to show my pid_str
      sys_write("... ");                  //call sys_write to show "... "
      os_time = sys_get_time();           //call sys_get_time() to get current os_time
      Number2Str(os_time, time_str);      //call Number2Str() to convert it to time_str
      sys_sleep(1);                       //call sys_sleep() to sleep for 1 second
      sys_set_cursor(my_pid, 0);
      sys_write("Sys time is ");          //call sys_write() to show "sys time is "
      sys_write(time_str);                //call sys_write() to show time_str
      sys_write("... ");                  //call sys_write to show "... "
      sys_sleep(1);
   }
}
