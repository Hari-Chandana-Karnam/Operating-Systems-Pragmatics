Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

OS Kernel Programming Phase 6
Signal IPC (User Runtime Redirection)

The Goal

After the previous phase that demonstrated multiple processes being
created to form a game-like race, one may raise 2 obvious questions:

1. How can the child processes start the 'race' at the same time
(as much the same as what a multitasking OS can)?

2. How can the parent process free itself from the blocking sys_wait()
calls and do something else in the foreground?

In Unix/Linux, the pause() call is the same as sleep(infinite seconds)
and is used by a process to suspend itself until receiving a software
'signal' from another process. To send a signal to a process or group
of processes, a process can issue a kill() call. To 'register' own
handling routine upon a certain signal (event), a process can issue
a signal() call (so it doesn't need to constantly 'poll' for the event).
These are some of the signal IPC services.

We have the sleep service for child processes to call (once they start
to run). The parent can leisurely send a wake/start signal to all of
them after the children have all been created and gone to sleep.
The parent process can issue a signal-handler register call first and
demonstrate being active in the foreground by showing its PID and erasing
it constantly.

Later whenever a child gets to exit (finishing the race), the exit
service of the OS will redirect parent's runtime to its own handler
(and it will resume from the original point in code). In order for
the OS to redirect a process instruction sequence, the runtime stack
has to be altered in such the return instruction address in the trapframe
of the parent process is altered to the handling function while the
original return instruction address is inserted between the trapframe
and the rest of the original runtime stack. (This also means the
trapframe is moved down by one-number notch in the runtime memory.)
This is as if we 'manually' insert a subroutine call to the process
runtime!

What Are to Be Added:

1. we need new service names:
      SYS_SIGNAL 140 and SYS_KILL 141
   and new signal names:
      SIGCHLD 17 and SIGCONT 18

2. in PCB, an array of function pointers is needed
      func_p_t signal_handler[32]
   in a 32-bit OS, there can be this many different signals

3. new syscalls
   void sys_signal(int signal_name, func_p_t p) is for a process
   to 'register' a function p as the handler for a certain signal
   and
   void sys_kill(int pid, int signal_name) is for a process to
   send a signal to a process (or all in the same process group)

4. SyscallSR would accommodate 2 new cases

5. changes to existing kernel service routine:
   in SysExit(), as process turns ZOMBIE due to missing parent,
   check if parent has 'registered' a handler for SIGCHLD event
   if so, altering parent's stack is needed

6. a new function in ksr.c
   AlterStack(pid, func_p_t p) is to alter the current stack
   of process 'pid' by:
      a. lowering trapframe by 4 bytes,
      b. replacing EIP in trapframe with 'p'
      c. insert the original EIP into the gap (between
         the lowered trapframe and what originally above)

7. void SysSignal(void)
      use the signal name (as the array index) and function ptr
      (as the value) passed from syscall to initialize the
      signal-handler array in run_pid's PCB

8. void SysKill(void)
      the pid and signal name are passed via syscall
      if the pid is zero and the signal is SIGCONT:
      wake up sleeping children of run_pid

9. the handler when a child process exits:
   void MyChildExitHandler(void) {
      call sys_wait() to get exiting child PID and exit code
      call sys_get_pid() to get my PID

      convert exiting child pid to a string
      convert exiting code to another string

      lock the video mutex
      set the video cursor to row: exiting child pid, column: 72
      write 1st string
      write ":"
      write 2nd string
      unlock the video mutex
   }

10. void Init(void) {    // 1 parent, 5 children
       ...
       ...
       ...
       parent calls sys_signal() to 'register' the above handler
       (when SIGCHLD/child exiting occurs, the handler will run)

       parent loops 5 times
          call sys_fork() and get its return
          if I'm a child, get out of  the loop!
          if it returns NONE {
             write "sys_fork() failed!\n"
             call sys_exit() with exit code NONE
          }
       }

       call sys_get_pid() to get my PID
       convert my PID to a string

       if I'm the pround parent {
          sleep for a second (probably it's 10 by now)
          call sys_kill() to send SIGCONT to all child processes (0)

          parent runs infinite loop {
             lock video mutex
             set video cursor to the start of my row
             write my PID string
             unlock video mutex

             sleep for a second

             lock video mutex
             set video cursor to the start of my row
             write a dash symbol (add another if needed)
             unlock video mutex

             sleep for a second
          }
       }

       child sleeps for 1000000     // child code continues here

       reset col & total sleep period
       while col is less than 70 {
          lock video mutex
          set video cursor to my row, col
          write my PID string
          unlock video mutex

          call and get a random sleep period 1~4
          sleep for that period
          add it to total sleep period

          lock video mutex
          set video cursor to my row, col
          write a dot symbol (add another if needed)
          unlock video mutex

          increment col by 1
       }
       child exits with total sleep period
    }

