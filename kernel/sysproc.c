#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 userpage_va;
  uint64 buff_bitmask = 0;  // temp var
  uint64 useraddr_buff;     // dst user address

  int page_num;
  int MAX_PAGE_NUM = 32; // 限制最大页数

  pagetable_t pt = myproc()->pagetable;

  argaddr(0, &userpage_va);
  argint(1, &page_num);
  argaddr(2, &useraddr_buff);    // 最终要拷贝到的用户地址

  for(int i = 0; i < MAX_PAGE_NUM; i++)
  {
    pte_t* pte = walk(pt, userpage_va + PGSIZE * i, 0); // alloc 参数设为0,表示只查询不分配
    if(*pte & PTE_A) // 最近访问过
    {
      *pte = *pte & (~PTE_A);    // 将pte的PTE_A位设为0,其他不变
      buff_bitmask = buff_bitmask | (1 << i);
    }
  }

  if(copyout(pt, useraddr_buff, (char *)&buff_bitmask, 4) != 0)
  {
    panic("sys_pgaccess copyout error!");
    return -1;
  }
  
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
