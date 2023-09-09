#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  int pid = fork();
  if(pid > 0 ){
    int c_pid = getpid();
    write(p[1], "p", 1);
    wait(0);
    close(p[1]);
    char resp[2];
    read(p[0], resp, 1);
    close(p[0]);
    printf("%d: received pong\n", c_pid);
  } else if(pid==0){
    int c_pid = getpid();
    char mess[2];
    read(p[0], mess, 1);
    close(p[0]);
    printf("%d: received ping\n", c_pid);
    write(p[1], "q", 1);
    close(p[1]);
  } else {
   fprintf(2, "fork error\n");
  }
  exit(0);
}
