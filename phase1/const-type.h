// const-type.h, 159, needed constants & types

#ifndef _CONST_TYPE_           // to prevent name mangling recursion
#define _CONST_TYPE_           // to prevent name redefinition

#define TIMER_EVENT 32         // timer interrupt signal code
#define PIC_MASK_REG 0x21      // I/O loc # of PIC mask
#define PIC_MASK_VAL ~0x01     // mask code for PIC
#define PIC_CONT_REG 0x20      // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60  // control code sent to PIC
#define VGA_MASK_VAL 0x0f00    // bold face, white on black

#define TIME_MAX 310            // max timer count, then rotate process
#define PROC_MAX 20             // max number of processes
#define STACK_MAX 4096          // process stack in bytes
#define QUE_MAX 20              // capacity of a process queue

#define NONE -1                 // to indicate none
#define IDLE 0                  // Idle thread PID 0
#define DRAM_START 0xe00000     // 14 MB

typedef void (*func_p_t)(void); // void-return function pointer type

typedef enum {AVAIL, READY, RUN} state_t; //Because it is enum function, AVAIL = 0, READY = 1, RUN = 2

/*define a trapframe type (tf_t) that has these 'unsigned int'
   eax, ecx, edx, ebx, esp, ebp, esi, edi, eip, cs, efl*/
typedef struct {
      unsigned int eax, ecx, edx, ebx, esp, ebp, esi, edi, eip, cs, efl;
} tf_t;     //Trapframe type

/*define a PCB type (pcb_t) that has 
   state_t state
   tf_t *tf_p
   unsigned int time_count and total_time*/
typedef struct {
   state_t state;             //State of the system
   tf_t *tf_p;                //Trapframe
   unsigned int time_count;   //time_count is the timer for the processes 
   unsigned int total_time;   //total_time is the CPU Run Time.
} pcb_t;    //PCB type

/*define a queue type (que_t) that has an integer 'tail' and an integer
array 'que' in it, the dimension of 'que' is QUE_MAX*/
typedef struct {
      int tail;
      int que[QUE_MAX]; //QUE_MAX = 20
} que_t;    //Queue type

#endif      // to prevent name mangling
