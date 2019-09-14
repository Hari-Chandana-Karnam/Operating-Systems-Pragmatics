// ext-data.h, 159
// kernel data are all declared in kernel.c during bootstrap
// .c code, if needed these, include this

#ifndef _EXT_DATA_
#define _EXT_DATA_

#include <const-type.h>
#include <tools.h>

extern int run_pid;                 // PID of current selected running process
/*and other extern ...*/
extern pcb_t pcb[PROC_MAX];
extern que_t ...;


#endif
