#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
  char *arg[MAXARG];
  memset(arg, 0, sizeof(arg));
  int i = 1;
  for(;i<argc;i++){
    arg[i-1] = argv[i];
  }
  
  char buf[64];
  memset(buf, 0, sizeof(buf));
  char c;
  char *p;
  int n = 1;
  while(n==1){
    p = buf;
    while((n=read(0, &c, 1))&&c!='\n'){
      *p = c;
      p++;
    }
    if(p!=buf){
      *p = '\0';
      arg[i-1] = buf;
        if(fork() == 0 ) {
          exec(argv[1], arg);
          exit(1);
        } else{
          wait(0);
        }
     }
   }  
  exit(0);
}
