#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
get_message(int pipe)
{
  char mess[1];
  read(pipe, mess, sizeof mess);
  return (int)(mess[0]);
}

void 
send_message(int pipe, int mess){
  char m[1];
  m[0] = (char)mess;
  write(pipe, m, sizeof m);
}

void 
pipe_primes(int pipe_in, int prime, int end){
  if(prime == end) exit(0);
  int p[2];
  pipe(p);
  int pid = fork();
  if(pid > 0){
    close(p[0]);
    if(prime==1) prime = get_message(pipe_in);
    printf("prime %d\n",prime);
    int i = prime;
    while(i<end && i>1){
      i = get_message(pipe_in);
      if(i%prime!=0||i==end){
	send_message(p[1], i);
      }
    }
    close(pipe_in);
    close(p[1]);
    wait(0);
  }else if(pid==0){
    close(p[1]);
    int pi = get_message(p[0]);
    pipe_primes(p[0], pi, end);
  }else{
   fprintf(2, "fork error\n");
  }
  exit(0);  
}

int 
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  int pid = fork();
  if(pid > 0 ){
    close(p[0]);
    for(int i = 1;i<=35;++i){
      send_message(p[1], i);
    }
    close(p[1]);
    wait(0);
  } else if(pid==0){
    close(p[1]);
    int prime = get_message(p[0]);
    pipe_primes(p[0], prime, 35);
  } else {
   fprintf(2, "fork error\n");
  }
  exit(0);
}
