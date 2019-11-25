// const-type.h, 159, needed constants & types

#ifndef _CONST_TYPE_             // to prevent name mangling recursion
#define _CONST_TYPE_             // to prevent name redefinition

#define TIMER_EVENT 32           // timer interrupt signal code
#define PIC_MASK_REG 0x21        // I/O loc # of PIC mask
#define PIC_MASK_VAL ~0x01       // mask code for PIC
#define PIC_CONT_REG 0x20        // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60    // control code sent to PIC
#define VGA_MASK_VAL 0x0f00      // bold face, white on black
#define PRESENT 0x01             // Page present flag
#define RW 0x02                  // Page is both read & writable
#define RO 0x00                 // Page is read only

#define TIME_MAX 310            // max timer count, then rotate process
#define PROC_MAX 20             // max number of processes
#define STACK_MAX 4096          // process stack in bytes
#define QUE_MAX 20              // capacity of a process queue
#define STR_MAX 20
#define PAGE_MAX 100            // OS has 100 DRAM pages to dispatch
#define PAGE_SIZE 4096          // Each page size in bytes

#define NONE -1                 // to indicate none
#define IDLE 0                  // Idle thread PID 0
#define VIDEO_MUTEX 0           // ID of VIDEO_MUTEX is 0. Phase 4.
#define UNLOCKED 0              // mutex unlocked state is 0
#define LOCKED 1                // mutex locked state is 1

// System Calls
#define SYSCALL_EVENT 128       // syscall event identifier code, phase2
#define SYS_GET_PID 129         // different types of syscalls
#define SYS_GET_TIME 130
#define SYS_SLEEP 131
#define SYS_WRITE 132
#define SYS_FORK 133            // phase 3
#define SYS_SET_CURSOR 134      // phase 3
#define SYS_GET_RAND 135        // phase 4
#define SYS_LOCK_MUTEX 136      // phase 4
#define SYS_UNLOCK_MUTEX 137    // phase 4
#define SYS_EXIT 138            // phase 5
#define SYS_WAIT 139            // phase 5
#define SYS_SIGNAL 140          // phase 6
#define SYS_KILL 141            // phase 6
#define SYS_READ 142            // phase 7
#define SYS_VFORK 143           // phase 8 - creation of a virtual-space running process

#define CONSOLE 100             // phase9, for STDIN of Idle
#define TTY 200                 // for STDIN of Shell and its children
#define TTY_EVENT 35            // TTY0/2, use 36 for TTY1
#define PIC_MASK_VAL ~0x09      // new mask: ~0..01001
#define TTY_SERVED_VAL 0x63     // also for COM4, 0x64 for COM3
#define TTY0 0x2f8              // TTY1 0x3e8, TTY2 0x2e8

// Signals
#define SIGCHLD 17              // phase 6
#define SIGCONT 18              // phase 6

#define VIDEO_START (unsigned short *)0xb8000
#define VIDEO_END ((unsigned short *)0xb8000 + 25 * 80)
#define DRAM_START 0xe00000     // 14 MB
#define G1 0x40000000           // phase 8 - Virtual space starts
#define G2 0x80000000           // phase 8 - Vitual space ends (1 less byte)

typedef void (*func_p_t)(void);

typedef enum {AVAIL, READY, RUN, SLEEP, SUSPEND, WAIT, ZOMBIE, IO_WAIT} state_t;

typedef struct {
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax, event, eip, cs, efl;
} tf_t;  // 'trapframe' type

typedef struct {
    state_t state;                  // State of the system.
    tf_t *tf_p;                     // Trapframe.
    unsigned int time_count;        // time_count is the timer for the processes 
    unsigned int total_time;        // total_time is the CPU Run Time.
    unsigned int wake_time;         // wake_time is when the CPU will wake up.
    unsigned int ppid;              // Process ID of parent.
    func_p_t signal_handler[32];    // 32 is the number of total signals possible in a 32-bit OS.
    unsigned dir;                   // This is where the address-translation directory is.
    int STDOUT;                     //which device (CONSOLE/TTY) for process output
} pcb_t;    //PCB type

typedef struct {
      int tail;
      int que[QUE_MAX]; //QUE_MAX = 20
} que_t;    //Queue type

typedef struct {
   int lock;
   que_t suspend_que;
} mutex_t;

typedef struct {
   que_t buffer;
   que_t wait_que;
} kb_t;

typedef struct { 
    int pid;//  the process that uses the page (initially NONE)
    union {                  // u is an alias of 3 meanings:
        unsigned addr;        // its real byte address
        char *content;        // as ptr to page content (bytes)
        unsigned *entry;      // as an array, used as ...u.entry[..]
    } u;
} page_t;

typedef struct {            //A new tty_t for a TTY object:
    char *str;               // addr of string to print
    que_t wait_que;          // requesting process
    int port;                // set to TTY0/1/2
} tty_t;

#endif      // to prevent name mangling
