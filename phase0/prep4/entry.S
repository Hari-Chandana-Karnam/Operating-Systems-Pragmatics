//*******************************************************************
// FIRST NAME: Angad Pal
// LAST NAME:  Dhanoa
// entry.S
// phase 0, prep 4, Timer Event Handling
//*******************************************************************

#include <spede/machine/asmacros.h>

ENTRY(TimerEntry)            // push eflag, cs, eip (by circuit)
   pusha                     // push all: e-acdb-x, e-sb-p, e-sd-i
   cld                       // clear direction flag
   call CNAME(TimerService)  // call C function
   popa                      // pop all: e-ds-i, e-bs-p, e-bdca-x
   iret                      // pop eip, cs, eflags

