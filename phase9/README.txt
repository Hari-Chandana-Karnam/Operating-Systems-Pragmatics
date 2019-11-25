Sac State, Computer Engineering/Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

Phase 9 TeleTYpe, Interrupt Driven Output


Goals

In order for a process to use a two-way communication device such
as a terminal (TTY), the OS must incorporate a device driver for
the service.

As a process requests a character string to be printed to a terminal,
it issues a service call and in term the OS service routine initiates
the hardware to process the 1st character of the string, and suspend
the process. A moment passes and the terminal will feed back with
an event of completion (of printing the character), an event handling
service routine will continue to the next character, and so on, until
the whole string is served, and the process is no longer suspended.


System Service SysWrite -- the upper half of a device driver

The existing SysWrite needs expansion:
1. The original code handles the CONSOLE output, the expansion
   is to handle the TTY output.
2. Therefore, which device: CONSOLE or TTY is the STDOUT of a
   process uses must be indicated in the PCB.
3. The calling process is suspended if its STDOUT is TTY:
   a. the address of the string is placed in the TTY data structure,
   b. the process state is changed to IO_WAIT,
   c. suspended in the wait queue of a new TTY data structure, and
   d. call the TTYSR (see below).


Setup TTY Port

Similar to how the OS is configured with the timer device:
1. the port to which a TTY is connected will be added to the PIC
   configuration by an updated PIC mask to accept a TTY event.
2. The IDT will have an entry for the TTY events.
3. The entry.S will have a code module to handle this event.


Handling Port Interrupt (TTYSR) -- the lower half of a device driver

1. first send to PIC a TTY_SERVED_VAL (similar to what TimerSR does)
2. if the wait queue of the TTY data structure is empty, just return
3. get a character from the string address in the TTY data structure
4. if it's NOT NUL
      a. output the character to the port (see TTYinit)
      b. advance the string address
   else
      release the waiting process (3 steps)


Deliverables

Source files as usual, other restrictions and requirements of not conducting
plagiarism as usual (see the deiverable description in previous phases).


Questions

If the TTY service is based on a busy-wait method, how would it be
programmed? To minimize the wait effect for multi-tasking processes?

