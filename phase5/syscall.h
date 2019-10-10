// syscall.h, 159

#ifndef _SYSCALL_
#define _SYSCALL_

int sys_get_pid(void);
int sys_get_time(void);
void sys_sleep(int sleep_sec);
void sys_write(char* write_sec);
void sys_set_cursor(int row, int column); 
int sys_fork(void); 
unsigned int sys_get_rand(void);          // phase 4
void sys_lock_mutex(int mutex_id);        // phase 4
void sys_unlock_mutex(int mutex_id);      // phase 4
void sys_exit(int exit_code);             // phase 5
int sys_wait(int *exit_code);             // phase 5

#endif
