// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct mem_ref {
  struct spinlock lock;
  int ref;
};

struct mem_ref mem_refs[PHYSTOP/PGSIZE];

void
kinit()
{
  for(int i = 0; i<PHYSTOP/PGSIZE;i++){
    initlock(&(mem_refs[i].lock), "mem_refs");
  }
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    mem_refs[(uint64)p/PGSIZE].ref = 0;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  uint64 pi = (uint64)pa/PGSIZE;
  acquire(&(mem_refs[pi].lock));
  mem_refs[pi].ref--; 
  if(mem_refs[pi].ref > 0){
    release(&(mem_refs[pi].lock));
    return;
  }
  release(&(mem_refs[pi].lock));
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    uint64 pi = (uint64)r/PGSIZE;
    acquire(&(mem_refs[pi].lock));
    mem_refs[pi].ref = 1;
    release(&(mem_refs[pi].lock));
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

int
add_ref(uint64 pa){
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    return -1;
  uint64 pi = (uint64)pa/PGSIZE;
  acquire(&mem_refs[pi].lock);
  mem_refs[pi].ref++;
  release(&mem_refs[pi].lock);
  return 0;
}

int 
get_ref(uint64 pa){
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    return -1;
  return mem_refs[pa/PGSIZE].ref;
}
