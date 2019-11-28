Sac State, Computer Engineering/Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

Phase A TeleTYpe, Interrupt Driven Input

Goals
   In this phase we complete the I/O operation device drivers for
   the two-way communication device, the terminal (TTY). The OS
   will extend its SYS_READ service to accmmodate the input from
   a terminal keyboard.

   As a process requests a character string to be filled out from
   the keyboard input of a terminal, it issues a service call and
   in term the OS service routine suspends the process until a
   whole string has arrived. This is the upper half of the device
   driver that deals with a terminal keyboard.

   The lower half of the device driver is when the hardware interrupt
   from the terminal keyboard invokes it to read in an input character
   to be added to the input string until the return key at which time
   the string is null-delimited and the process is released.

   In order to echo back what is being typed, the lower half will
   also add the input character to an 'echo' buffer so they can be
   displayed back to the terminal display. This means the lower half
   of the terminal output (display) device driver will need to display
   what is in the echo buffer if it's not empty, and normally the
   echo buffer has a higher priority than the original output string
   requested by a process.  

   There will be two types of interrupt events from the terminal port:
   besides the display event from the previous phase, the keyboard event
   can also occur.
   Hence, to join the two lower halves, the original TTYSR will need to
   poll the port to differ the I/O, i.e., whether it's for output/display:
   call the original code (new name TTYdspSR, with the echo buffer included)
   or for keyboard input (TTYkbSR).

The Upper Half
   sys_read() is updated: there's an asm() macro like others in syscall.c.

   SysRead:
      1. if the STDIN of the PCB for the running process is CONSOLE:
            (Not required. See Question 1 on hints how it can be done.)
      2. else if the STDIN of the PCB for the running process is TTY:
            a. the string address (from the process) is copied to 'tty' (TTY data)
            b. the calling process is suspended in the keyboard wait queue of 'tty,'
            c. the calling process state is changed to IO_WAIT, and
            d. the running process becomes none.
      3. else (not either above):
            a. show panic msg: no such device, and
            b. call breakpoint().

The Lower Half
   TTYSR
      1. send to PIC a TTY_SERVED_VAL (similar to what TimerSR does)
      2. read the status of the port (Interrupt Indicator Register)
      3. if status is about terminal display:
            call TTYdspSR.
         else if status is about terminal keyboard:
            call TTYkbSR, then
            call TTYdspSR (to echo).
         (else:
            do nothing.)

   TTYdspSR
      if the echo buffer of TTY is NOT empty:
         a. dequeue the 1st character from the buffer, and
         b. send it to be displayed on the terminal, and
         c. return.
      (The rest is the same as in the previous phase.)

   TTYkbSR
      1. read a character from the TTY port
      2. if the wait queue (for keyboard) in TTY is empty, just return
      3. enqueue the character to the echo buffer of TTY
      4. switch to the virtual space of the 1st process in the TTY keyboard wait queue
      5. if the character read is NOT '\r:'
            a. add it to where the TTY string pointer points to, and
            b. advance the pointer.
         else:
            a. enqueue '\n' to the TTY echo buffer,
            b. add '\0' to where the TTY keyboard string points to, and
            c. release the 1st waiting process from the TTY keyboard wait queue. (3 steps.)

Deliverables
   Source files as usual, other restrictions and requirements of not conducting
   plagiarism as usual (see the deiverable description in previous phases).


Questions
1. Similar to how to get input from a terminal, how can we change the console
   keyboard input so sys_read has only an asm() macro?

2. To show "Can't touch that!" upon the ctrl-C key from the terminal, where
   is the proper place to call AlterStack? Suppose this is part of a signal
   handling project that we are to work on.
