// proc.c, 159
// processes are coded here

//need to include const-type.h and ext-data.h
#include <const-type.h>
#include <ext-data.h>
#include <proc.h>

unsigned int sys_time_count = 0; //To count the time for flickering purposes.
unsigned short *upper_left_pos = (unsigned short *)0xb8000; //declare a pointer and set it to the upper-left display corner
int flagForDot = 1; //declare and set a flag. When 1 we will show the dot. When 0 we will hide it. 

/*Code an Idle() function that doesn't have any input or return, but just
flickers the dot symbol at the upper-left corner of the target PC display.
It reads sys_time_count and at each second interval writes '.' or ' ' via
an unsigned short pointer to the VGA video memory location 0xb8000.
Apply the VGA_MASK_VAL when showing . or space so it will appear to be
boldface writing.*/
void Idle(void)
{
    //We will notify the PIC that the event is served.
    outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
    
    /*In an infinite loop:
       whenever the system time reaches a multiple of 100 (per 1 second):
          a. judging from the flag and show either the dot or space
          b. alternate the flag*/
    while(1)
    {
        //We will upcount the sys_time_count by 1 everytime the loop runs.
        sys_time_count += 1;
        
        if(sys_time_count%100 == 0) //Using mod to detmine the multiple of 100.
        {
            if(flagForDot = 1)
            {
                *upper_left_pos = '.' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                flagForDot = 0;
            }else
            {
                *upper_left_pos = ' ' + VGA_MASK_VAL; //VGA_MASK_VAL makes it bold and white on black.
                flagForDot = 1;
            }
        }
    }
    return;
}

