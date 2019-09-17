// ksr.c, 159

void SpawnSR(func_p_t p) {     // arg: where process code starts

// copy code to DRAM, both code & stack are separated among processes, phase2
   MemCpy((char *)DRAM_START + ...?

// point tf_p to stack & fill TF out
   pcb[pid].tf_p = (tf_t *)(DRAM_START + ...?
   ...
   ...
   ...
   pcb[pid].tf_p->eip = DRAM_START + ...?
}

// count run time and switch if hitting time limit
void TimerSR(void) {
   ...
   ...
   ...
   original code to here:  ...total_time++;

   Use a loop to look for any processes that need to be waken up!

   also add here that:
   if run_pid is IDLE, just simply return;    // Idle exempt from below, phase2

   if(pcb[run_pid].time_count == TIME_MAX) {  // if runs long enough
   ...
   ...
   ...
}

void SyscallSR(void) {
   switch by the eax in the trapframe pointed to by pcb[run_pid].tf_p
      if it's  SYS_GET_PID,
         copy run_pid to ebx in the trapframe of the running process
         
      if it's SYS_GET_TIME,
         copy the system time count to ebx in the trapframe of the running process

      if it's SYS_SLEEP,
         call SysSleep()

      if it's SYS_WRITE
         call SysWrite()

      default:
         cons_printf("Kernel Panic: no such syscall!\n");
         breakpoint();
   }
}

void SysSleep(void) {
   int sleep_sec = ... from a register value wtihin the trapframe
   calculate the wake time of the running process using the current system
   time count plus the sleep_sec times 100
   alter the state of the running process to SLEEP
   alter the value of run_pid to NONE
}

void SysWrite(void) {
   char *str =  ... passed over by a register value wtihin the trapframe
   show the str one char at a time (use a loop)
      onto the console (at the system cursor position)
      (while doing so, the cursor may wrap back to the top-left corner if needed)
}

