<h1>Sacramento State, Computer Engineering / Computer Science CpE/CSc 159 Operating System Pragmatics (Fall 2019) Instructor: W. Chang</h1>
   
OS Kernel Programming Phase 5
Process Life Cycle

The Goal

To complete the process life cycle, at least two more services
are needed for a child process to exit and for a parent process
to wait. Two new syscalls will be needed of course and associated
with them are their event ID's and kernel service routines.

In this phase, Init will loop to fork 5 times first. (A new process
breaks the loop when knowing being a child by the fork return.)
After the loop, the parent Init will loop to call for the wait
service five times while each of the child processes races across
the screen and exits.

The exit service is to return an exit code to the parent process
of the service-calling process, and the resources of the process
are reclaimed by the OS.

The wait service is ususally called by a parent process (after
child-process creation). The service may block the calling
parent process into a WAIT state when not any child process
of it has yet called for the exit service. The suspension ends
as any one of its child processes calls for the exit service.

If during an exit service the parent process of the calling
process has not yet called for the wait service, the child
process will be suspended in a ZOMBIE state untile its parent
process call for the wait service. Only at that time the exit
code of the exiting child process can be given to the parent
and the resources of the child process can then be reclaimed.

Hence, the exit and wait services are somewhat similar to a
mutex operattion.


What Are to Be Added:

1. When running out of PID, SysFork service should place NONE
   to ebx in trapframe of process for sys_fork call to return.

2. new constants for the two new services are
      SYS_EXIT 138 and SYS_WAIT 139
   new process states are
      WAIT and ZOMBIE

3. new syscalls
      void sys_exit(int exit_code)
         the exit code is to pass to parent
      int sys_wait(int *exit_code)
         the return is the exiting child PID,
         addr of exit_code is for service to fill out

4. SyscallSR has 2 new cases to call new service routines

5. new service routines are:
   void SysExit(void), the service a process calls:
      is parent in WAIT state (called wait 1st)?
      no:
         running process cannot exit, it becomes a zombie
         no running process anymore
      yes:
         release parent:
            upgrade parent's state
            move parent to be ready to run again
         also:
            pass over exiting PID to parent
            pass over exit code to parent
         also:
            reclaim child resources (alter state, move it)
            no running process anymore

   void SysWait(void), the service a parent calls:
      search for any child that called to exit?
      if not any found:
         parent is blocked into WAIT state
         no running process anymore
      if found one:
         pass over its PID to parent
         pass over its exit code to parent
         reclaim child resources (alter state, move it)

6. Init looks like this:
   void Init(void) {
      char pid_str[20], str[20];
      int my_pid, forked_pid,
          i, col, exit_pid, exit_code,
          sleep_period, total_sleep_period;

      loop 5 times to fork 5 children:
         call sys_fork() and get its function return as forked_pid
         a child process now breaks the loop
         if forked_pid is NONE:
            write "sys_fork() failed!\n"
            call exit with code NONE

      get my_pid
      convert it to pid_str
      for the one parent process, it does {
         loop 5 times (i=0..4) {
            call sys_wait() with addr of exit_code as argument, 
            sys_wait returns an integer, received as exit_pid
   
            lock video mutex
            set cursor to my row, column i*14
            write "PID "
            convert exit_pid to str
            write str
            write ": "
            convert exit_code to str
            write str
            unlock video mutex
         }
         write "  Init exits."
         call exit with code 0;
      }
   // child code below, similar to prev, race across screen
      col = total_sleep_period = 0;
      while col is less than 70 {
         lock video mutex
         set cursor to my row, column col
         write pid_str
         unlock video mutex

         get a random sleep_period = ... // get rand # 1~4
         sleep with that random period
         add random period to total_sleep_period
         col++;
      }
      call exit sesrvice with total_sleep_period as exit code
   }


Deliverables

Again, submit program files only. Do not submit any files generated
by the compiler or SPEDE software; i.e., submit only those that have
names ended in .c, .h, and .S.


Question
If you press the space bar after the race ends, it would restart.
How can your program also do this (very easily)?

