#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"
#include "sysinfo.h"

uint64 
sys_sysinfo(void)
{
  struct sysinfo si;
  uint64 addr;
  struct proc *p = myproc();
  if(argaddr(0, &addr)<0){
    return -1;
  }
  uint64 pn = used_proc_num();
  uint64 fm = free_mem();
  si.freemem = fm;
  si.nproc = pn;
  if(copyout(p->pagetable, addr, (char*)&si, sizeof(si))<0){
    return -1;
  }
  return 0; 
}

