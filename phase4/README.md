Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

OS Kernel Programming Phase 4
IPC: Mutex

The Goal

To be able to provide a mutex service for processes to avoid
the racing condition over the video usage. (See the Question
part in the writeup of the previous phase.)

In the previous phase a process is not guaranteed a continuous
right to use when accessing the video device (between setting
the cursor and writing). A racing condition occurs as a process
is switched between two related services. This will eventually
occur as the runtime slicing will eventually take place.

If we penalize a process for making a 'syscall' by re-queue it
to the tail of the ready-to-run process queue, the video racing
condition is almost always guaranteed. This is also the basis
of testing the mutex service for this phase.

Associated with each device is either a queue or mutex to guard
for coherent access by a process without conflicts or interruption.
Our OS will have a video mutex and unlocked initially. Services
to lock and unlock will be provided for processes to use them so
they can synchronize the video usage.

As before, Init will fork twice and altogether the 4 processes 
will loop to show their own runtime display on their own display
rows without interfering others 

A mutex is used by a group of processes to allow an exclusive
access of some guarded data by one process at a time. Each
process should first request to lock it before accessing the
guarded data, and request to unlock it afterwards. In our case
the guarded data is the video display device.

A mutex has a lock indicator and a process suspension queue.
When the mutex is unlocked, the 1st process requesting to lock
it may change it to be locked and continue to run. Any subsequent
processes requesting to lock it will be suspended in its suspension
queue until the 1st process requests to unlock it and at that time
the 1st suspended process will get to continue.

The OS will also provide a random number service so each of the
4 processes can use it to have a random period of sleep while
showing its own PID across their individual display rows (column
by column). Hence, the effect may seem like a 'race' across the
screen.

What Are to Be Added (p-code):

After fork calls and ..., Init has this infinite loop instead:
   start column with 0
   add a subloop (to loop until column reaches 70):
      lock video mutex
      set video cursor
      write my PID
      unlock video mutex
      get a number ranging from 1 to 4 inclusive randomly
      call sleep with that number as sleep period
      increment column by 1
   end subloop
   erase my entire row (use mutex & loop, of course)
   sleep for 30 (3 seconds)
(end of infinite loop)

To easily cause racing conditions, at the end of SyscallSR (after
the switching ends), add:
   if run_pid is not NONE, we penalize it by
      a. downgrade its state to READY
      b. moving it to the back of the ready-to-run process queue
      c. reset run_pid (is now NONE)
   
SleepSR
   can opt to shorten the sleep period by a factor of 10 (for
   more interesting viewing)

For 3 new syscalls, we have these event numbers for them:
   SYS_GET_RAND 135
   SYS_LOCK_MUTEX 136
   SYS_UNLOCK_MUTEX 137
the ID of video mutex is VIDEO_MUTEX 0
the state of a mutex is either LOCKED 1 or UNLOCKED 0

For mutexes, a new type mutex_t is needed and it has:
   an integer 'lock' that indicates the state of the mutex
   a queue 'suspend_que' to hold PID's of suspended processes

kernel.c and ext-data.h should declare and announce the new
   unsigned sys_rand_count
   mutex_t video_mutex
and BootStrap should initialize them for usages by processes.
(When a process calls to lock/unlock the mutex, put VIDEO_MUTEX
in the argument as the mutex ID.)

New syscalls for Init to call are:
   void sys_lock_mutex(int mutex_id)
   void sys_set_cursor(int mutex_id)
   unsigned sys_get_rand(void)
and they should be coded (prototyped) in syscall.c (.h).

Subsequently, SyscallSR sees new events and redirect to:
   void SysLockMutex(void)
   void SysUnockMutex(void)
(for SYS_GET_RAND, just copy sys_rand_count to the ebx in
the trapframe of the running process like SYS_GET_PID and
SYS_GET_TIME)

Since Idle has the uncandid nature to loop, we can use it
to count up sys_rand_count in its infinite loop. Approximately,
it will increment it by 400 per 1/100 of a second:
4M CPU inst / 100 timer intervals / 100 inst per loop = 400.
Or, sys_rand_count increents at a rate of 40,000 a second.
Hence, as it is fetched to return, its value easily vary.

Deliverables

Again, submit program files only. Do not submit any files generated
by the compiler or SPEDE software; i.e., submit only those that have
names ended in .c, .h, and .S.
Penalty will be applied if not following the instructions. This includes
incorrect submission and accidental cloberring of others in the dropbox
system. Please do it correctly.

Questions:

How can competing processes made to start at the very same time, and
how can a process notify the parent once it ends?
