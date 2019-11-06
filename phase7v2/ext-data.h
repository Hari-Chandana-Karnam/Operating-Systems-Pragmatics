// ext-data.h, 159
// kernel data are all declared in kernel.c during bootstrap
// .c code, if needed these, include this

#ifndef _EXT_DATA_
#define _EXT_DATA_

#include "const-type.h"

extern int run_pid;                 
extern pcb_t pcb[PROC_MAX];
extern que_t avail_que; 
extern que_t ready_que;
extern unsigned int sys_time_count;
extern unsigned short *sys_cursor; 
extern unsigned sys_rand_count;
extern mutex_t video_mutex;
extern kb_t kb;

#endif
