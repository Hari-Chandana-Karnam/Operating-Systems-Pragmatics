Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

OS Kernel Programming Phase 7
Device Driver: Busy Polls (Console Keyboard)

The Goal

We have experienced a simple method for the console video output
(the STDOUT channel of processes) as the SYS_WRITE service.
To learn more about how device drivers work, we look into getting
input from the console keyboard (the STDIN channel of processes).

Since the kernel runs frequently in the background, it is a good
candidate to busy-poll the keyboard for input and put input
characters in a string buffers so processes can retrieve them
via a SYS_READ service.

A kernel keyboard service routine (KBSR) is performed when the
kernel runs (to perform services for events). The KBSR checks
if the keyboard has input, reads in the character, checks if
there is a waiting process for it. If yes, the process will be
released and receive the character. If no, the character is
queued in a string buffer for later retrievals.

A process calls sys_read() with an address of a string to be
filled out. The sys_read() performs a loop to issue an inline
assembly call 'int ?' to get a character returned at a time
(to put into the string) until a RETURN key, then the loop
ends and the string is NUL-delimited.

The sys_read() is served by the kernel SysRead() routine
where the keyboard buffer is dequeued to return for sys_read().
However, if the buffer is empty, the process is preempted:
enqueued to a keyboard wait queue, state changed to IO_WAIT,
and run_pid is no longer there.

The above description sounds faimilar. The wait-exit or mutex
operation work similarly.

The kernl will need a special keyboard data structure (kb_t kb) to
host for a string buffer and a wait queue (que_t buffer, wait_que).

What Are to Be Added:

1. We need a new service name, a new constant, and a new state:
      SYS_READ=142, STR_MAX=20, and IO_WAIT
   plus, a new structure kb_t that has two queues: buffer and wait_que.

2. We need a new kernel data 'kb' of the 'kb_t' type.
   It is 1st cleared during the OS bootstrap.

3. In main(), SpawnSR(Login) instead of Init.

4. In Kernel(), delete: if(cons_kbhit)... (all code about keyboard)
   and replace with a call to KBSR() where all keyboard things are
   taken care of.

5. a new Login() in proc.c:
   void Login(void) {
      char login_str[STR_MAX], passwd_str[STR_MAX];

      while(1) {
         prompt for login entry
         read keyboard into login_str
         prompt for password entry
         read keyboard into passwd_str

         call StrCmp() to compare the 2 strings 
         if same -> prompt: login successful!\r  // & break; in next phase
         else -> prompt: login failed!\r
      }
//      sys_vfork(Shell);  // virtual-fork Shell (virtual memeory) phase8
   }

6. add a new syscall sys_read():
   void sys_read(char *str) {  // the string
      ...
      Apply a loop for the following logic:
         Issue asm("... issue an 'int ?' to do SYS_READ);
         and get a character at a time.

         'Echo' back the character to the console video:
         build from it a small string and call sys_write().

         Add the character to the string unless it's the RETURN
         key ('\r') in which a null character ('\0') is added.

         The string should not exceed STR_MAX bytes: when adding
         the STR_MAX-1th character, append NUL and return.

7. add a new kernel service case and routine SysRead():
   if the buffer in the KB data structure is not empty:
      get the 1st character and give it to the running process
   else
      queue the running PID to the wait queue in the KB data
      change the process state to IO_WAIT
      running process is NONE

8. add a new kernel service routine KBSR():
   if keyboard is not pressed: return
   read the key
   if the key is '$,' breakpoint() // used to be 'b'

   if NO process awaits (KB wait queue is empty):
      enqueue the key to the KB buffer
   else
      release a waiting process from the wait queue
      queue it to the ready-to-run queue
      update its state
      give it the key
   
9. modify service routine SysWrite() so:
   if the character to be echoed is '\r,' instead of displaying it,
   advance sys_cursor to the 1st column of next row,
   when sys_cursor wraps back to VIDEO_START: erase the whole screen.

Deliverables

Again, submit program files only. Do not submit any files generated
by the compiler or SPEDE software; i.e., submit only those that have
names ended in .c, .h, and .S.

Question

Revisit Idle():
Is it possible to re-program it so the letter the blinking asterisk
covers can become the alternate symbol (instead of a space)? That is,
if "login: " was there and the blinking starts, the letter 'l' (not
a space) will be altering with '*.' It can be any other possible letter
that went under, not just 'l.' (See demo.dli runs. Difficulty: Very High.
Hint: reading from video memory is valid -- still a difficult problem.)
