//*******************************************************************
// FIRST NAME: Angad Pal 
// LAST NAME:  Dhanoa
// kernel.c
// Phase 0, Prep 4, Timer Event Handling
//*******************************************************************

#include <spede/flames.h>            // some SPEDE stuff that helps...
#include <spede/machine/asmacros.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/stdio.h>

#define TIMER_EVENT 32         // timer interrupt event code
#define PIC_MASK_REG 0x21      // I/O loc # of PIC mask
#define PIC_MASK_VAL ~0x01     // mask code for PIC
#define PIC_CONT_REG 0x20      // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60  // control code sent to PIC
#define VGA_MASK_VAL 0x0f00    // bold face, white on black

// prototype a void-return void-argument TimerEntry function:
void TimerEntry(void);

unsigned int time_count = 0;
unsigned short *center_pos = (unsigned short *)0xb8000 + 12 * 80 + 40;
struct i386_gate *idt;         // interrupt descriptor table

//Program a C function 'void TimerService(void)'
void TimerService(void)
{
	//first, notify PIC the event is served: outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
   	
	//upcount time_count by 1
	time_count+=1;

   	/*as each second arrives: print/erase initials of your name on the center of the
   	 *screen, so it gives the effect of flashing out the initials at the rate of 2Hz
   	 *(i.e., 1 second on, 1 second off, see demo run), use time_count, center_pos and
   	 *VGA_MASK_VAL to achieve this
	 */
	if(time_count == 100)	//When the time_count == 100 we will show the message.
	{
		//I have AP as my first name's initials. 
		*(center_pos-1) = 'A' + VGA_MASK_VAL;
		*(center_pos)   = 'P' + VGA_MASK_VAL;	//This makes P in the center, and A and D on the outside.
		*(center_pos+1) = 'D' + VGA_MASK_VAL;
	}
        else if(time_count == 200)
        {
                *(center_pos-1) = ' ' + VGA_MASK_VAL;
                *(center_pos)   = ' ' + VGA_MASK_VAL;
                *(center_pos+1) = ' ' + VGA_MASK_VAL;
		time_count = 0;
        }

   	return;
}

//Program a C function 'int main(void)'
int main(void)
{
   	//declare local char 'ch'
	char ch;

	//set 'idt' to the return of 'get_idt_base()' call
	idt = get_idt_base();

   	//set an entry in idt to handle future events of timer interrupts:
      	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);

   	//send PIC_MASK_VAL to PIC_MASK_REG using outportb() call like above
	outportb(PIC_MASK_REG, PIC_MASK_VAL);
   
	//enable the CPU to handle interrupts using inline assembly macro: 'asm("sti");' 
	asm("sti");

	//perform an infinite loop that repeats:
        while(1)
	{
		//check the keyboard of the target PC, if it has just been pressed:
           	if(cons_kbhit())	//Read the key being pressed into ch.
		{
			ch = cons_getchar();
			cons_printf( "\nYou Pressed %c.\n", ch );	//Show the message onto target PC about the key being pressed.
			if(ch == 'x' || ch == 'X')	//If 'X' or 'x' are pressed, terminate the program.
			{	
				cons_printf( "Goodbye." );
				break;
			}
			else
			{
				cons_printf( "%c does not Terminate me.\n", ch);
			}
		}
	}
   	return 0;
}
