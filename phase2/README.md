Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

OS Kernel Programming Phase 2
Process Init & OS Services


The Goal

A process is completely reliant on the OS to serve the functions
in its code. The process Init will be created after the OS Idle 
thread to test a set of basic OS services that we here learn to
construct for it to use.

The fundamental kernel system services are often termed "syscalls"
from which "library calls" are built, and hence all different classes
of applications and frameworks follow.

A service call is typically performed by an "interrupt" event that
is intentional in the software, hence, colloquially a "software"
interrupt. On a Unix/Linux system, the event number is 128 (0x80).
This means in the IDT entry 128 leads to the entry point of the
kernel where a specific service is performed. The calling process 
meanwhile is suspended until the service is done.

The simpliest services are providing system information to processes,
such as the process ID and system time. More involved services such
as "sleep" and "write" will also be practiced in this phase.

In order to provide software services, an "API" interface will be
accessible for processes. The kernel will need to have additional
code and data to perform the services.


What to Be Added

The file const-type.h will have more items in it. You see them
detailed in its p-code counterpart. Make sure you use the new
trapframe definition. Also, note the new 'event' in the trapframe.
This affects how assembly modules in entry.S must be coded to
accommodate it.

In ksr.c, TimerSR() do not check if Idle runs upto TIME_MAX,
i.e., insert "if run_pid == Idle, simply return;" right before
the comparison to TIME_MAX.

In main(), after creatng Idle, call SpawnSR again to create Init.
In SpawnSR, make sure the code and stack are being assigned to
a new 4KB region for Idle and Init: multiply STACK_MAX with the
'pid' dequeued from the 'avail_que' so a new process will occupy
a different 4KB of the DRAM.

Again, to copy pseudo code, issue a shell command:
   cp ~changw/html/159/2/p-code/*  .
and, avoid using copy-paste which many times incurs illegal
ctrl-M characters that cause compiler errors.


Descriptions of Files in 'p-code:'

The Init() for proc.c is all coded for you but your goal is to have
your OS serve the 4 syscalls correctly as it runs.

The syscall.c & its .h files are the API for Init() to call.
Some examples are code in it but you need to complete the rest.

Via each syscall, the CPU execution goes into Kernel() and then
Syscall() which is in ksr.c. Syscall() looks into the 'event'
of the trapframe and perform the service either directly in
the Syscall() function or call subroutines SysSleep() and SysWrite().

The tools.c and its .h files have a new function to convert a
given number (usually positive integer) to a char string (for
printing).


Deliverables

Again, submit program files only. Do not submit any files generated
by the compiler or SPEDE software; i.e., submit only those that have
names ended in .c, .h, and .S.

Penalty will be applied if not following the instructions. This includes
incorrect submission and accidental cloberring of others in the dropbox
system. Please do it correctly.
