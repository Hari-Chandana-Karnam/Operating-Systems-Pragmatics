<h2>Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang</h2>

OS Kernel Programming Phase 3
Parent & Child Processes

The Goal

To be able to provide a system service for a parent process to
create multiple child processes.
A parent process has a PCB and 4KB DRAM for runtime. To create
a child process is to replicate them (with a few different places).

What to Be Added (p-code):

Process Init() will have a new definition in which it calls
sys_fork() twice. The runtime appears to have 4 processes
due to the fact the 1st child of Init continues to execute
the code to call sys_fork() again and create an extra process.
Afterward, the infinite loop (similar to the previous phase)
starts:
   sleep for a second,
   set cursor position to my row (equal to my PID), column 0,
   show my PID,
   get time and convert it,
   sleep for a second,
   set cursor position back again,
   show time.

The PCB type will need a parent PID information (unsigned ppid)
in order for the OS to know the relationship of processes.

Besides sys_fork() call, a handy sys_set_cursor() call to set
the cursor location is favored. Hence, the new constants are
   SYS_FORK 133
   SYS_SET_CURSOR 134
and new calls sys_fork() and sys_set_cursor() are to be added
to syscall.c (.h).

Subsequently as calls occur by the code in Init(), service
routines SysFork() and SysSetCursor() in ksr.c (.h) will run.

SysSetCursor() simply changes sys_cursor to the position of
the row and column numbers in the trapframe CPU registers
(as inserted when called by Init). Hint: the video memory
address for row 0, column 0 is the VIDEO_START.

To 'fork,' the SysFork() will need to do these things:
   1. allocate a new PID and add it to ready_que (similar to start of SpawnSR)
   2. copy PCB from parent process, but alter these:
         process state, the two time counts, and ppid
   3. copy the process image (the 4KB DRAM) from parent to child:
         figure out destination and source byte addresses
         use tool MemCpy() to do the copying
   4. calculate the byte distance between the two processes
         = (child PID - parent PID) * 4K
   5. apply the distance to the trapframe location in child's PCB
   6. use child's trapframe pointer to adjust these in the trapframe:
         eip (so it points o child's own instructions),
         ebp (so it points to child's local data),
         also, the value where ebp points to:
            treat ebp as an integer pointer and alter what it points to
   7. correctly set return values of sys_fork():
         ebx in the parent's trapframe gets the new child PID
         ebx in the child's trapframe gets ?

Deliverables

Again, submit program files only. Do not submit any files generated
by the compiler or SPEDE software; i.e., submit only those that have
names ended in .c, .h, and .S.
Penalty will be applied if not following the instructions. This includes
incorrect submission and accidental cloberring of others in the dropbox
system. Please do it correctly.

Questions:

Experiment the runtime of Init() after swapping the line calling
sleep and the next line. (There are two places to swap in the loop.)
What would happen? Why?
