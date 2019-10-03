// proc.c, 159
...
...
...
void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner
   ...

   while(1) {
      sys_rand_count++;                  // phase4
      ...
      ...
      ...
}

void Init(void) {    // illustrates a racing condition
   int col, my_pid, forked_pid, rand;
   char pid_str[20];

   forked_pid = sys_fork();
   if(forked_pid == NONE)sys_write("sys_fork() failed!\n");

   forked_pid = sys_fork();
   if(forked_pid == NONE)sys_write("sys_fork() failed!\n");

   my_pid = sys_get_pid();              // what's my PID
   Number2Str(my_pid, pid_str);         // convert # to str

   Init has this infinite loop instead:
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
}

