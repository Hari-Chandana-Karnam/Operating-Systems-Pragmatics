...
...
...

void Init(void) {  // Init, PID 1, asks/tests various OS services
   int my_pid, os_time;
   char pid_str[20], time_str[20];

   int forked_pid;

   forked_pid = sys_fork();
   if(forked_pid == NONE)sys_write("sys_fork() failed!\n");

   do the above two statements again...

   my_pid = sys_get_pid();               // what's my PID
   Number2Str(my_pid, pid_str);          // convert # to str

   while(1) {
      sleep for a second,
      set cursor position to my row (equal to my PID), column 0,
      call sys_write a few times to show my PID as before,

      get time, and convert it,

      sleep for a second,
      set cursor position back again,
      call sys_write a few times to show sys time as before.
   }
}

