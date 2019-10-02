...
...
...

void SysSetCursor(void) {
   changes sys_cursor to the position of the row and column numbers
   in the trapframe CPU registers (as inserted when called by Init).
   Hint: the video memory address for row 0, column 0 is the VIDEO_START.

void SysFork(void) {
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

