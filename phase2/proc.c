// proc.c, 159
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "proc.h"
#include "spede.h"

unsigned short *upper_left_pos = (unsigned short *)0xb8000; //declare a pointer and set it to the upper-left display corner
int flagForDot = 1; //declare and set a flag. When 1 we will show the dot. When 0 we will hide it. 

/*Code an Idle() function that doesn't have any input or return, but just
flickers the dot symbol at the upper-left corner of the target PC display.
It reads sys_time_count and at each second interval writes '*' or ' ' via
an unsigned short pointer to the VGA video memory location 0xb8000.
Apply the VGA_MASK_VAL when showing . or space so it will appear to be
boldface writing.*/
void Idle(void)
{
    //We will notify the PIC that the event is served.
    outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
    asm("sti"); //To enable CPU to handle interrupts using inline assembly macro: 'asm("sti");'
    
    /*In an infinite loop:
       whenever the system time reaches a multiple of 100 (per 1 second):
          a. judging from the flag and show either the dot or space
          b. alternate the flag*/
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
    return;
}

void Init(void) {  // Init, PID 1, asks/tests various OS services
   declare two integers: my_pid & os_time
   declare two 20-char arrays: pid_str & time_str

   call sys_get_pid() to get my_pid
   call Number2Str() to convert it to pid_str

   forever loop {
      call sys_write() to show "my PID is "
      call sys_write() to show my pid_str
      call sys_write to show "... "
      call sys_sleep() to sleep for 1 second
      call sys_get_time() to get current os_time
      call Number2Str() to convert it to time_str
      call sys_write() to show "sys time is "
      call sys_write() to show time_str
      call sys_write to show "... "
      call sys_sleep() to sleep for 1 second
   }
}