// const-type.h, 159, needed constants & types

...

#define SYSCALL_EVENT 128       // syscall event identifier code, phase2
#define SYS_GET_PID 129         // different types of syscalls
#define SYS_GET_TIME 130
#define SYS_SLEEP 131
#define SYS_WRITE 132
#define VIDEO_START (unsigned short *)0xb8000
#define VIDEO_END ((unsigned short *)0xb8000 + 25 * 80)

...

Add a new state SLEEP to the existing state_t
Add an unsigned int wake_time to the PCB type

Use the new trapframe sequence (entry.S requires alteration):
typedef struct {   // add an 'event' into this, phase2
   unsigned int
      edi, esi, ebp, esp, ebx, edx, ecx, eax, event, eip, cs, efl;
} tf_t;                      // 'trapframe' type

...

