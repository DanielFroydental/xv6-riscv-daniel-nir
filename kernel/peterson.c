// Peterson locks for two-process mutual exclusion

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "peterson.h"

struct petersonlock peterson_locks[NPETERSON];
struct spinlock peterson_global_lock;

void
peterson_init(void)
{
  initlock(&peterson_global_lock, "peterson_global");
  for(int i = 0; i < NPETERSON; i++) {
    peterson_locks[i].valid = 0;
    peterson_locks[i].turn = 0;
    peterson_locks[i].flag[0] = 0;
    peterson_locks[i].flag[1] = 0;
  }
}

int
peterson_create(void)
{
  acquire(&peterson_global_lock);
  
  for(int i = 0; i < NPETERSON; i++) {
    if(!peterson_locks[i].valid) {
      peterson_locks[i].valid = 1;
      peterson_locks[i].turn = 0;
      peterson_locks[i].flag[0] = 0;
      peterson_locks[i].flag[1] = 0;
      release(&peterson_global_lock);
      return i;
    }
  }
  
  release(&peterson_global_lock);
  return -1; // No available locks
}

int
peterson_acquire(int lock_id, int role)
{
  if(lock_id < 0 || lock_id >= NPETERSON || role < 0 || role > 1)
    return -1;
    
  if(!peterson_locks[lock_id].valid)
    return -1;
    
  struct petersonlock *lock = &peterson_locks[lock_id];
  int other = 1 - role;
  
  // Peterson's algorithm with yield() instead of busy waiting
  __sync_lock_test_and_set(&lock->flag[role], 1);
  __sync_synchronize();
  
  __sync_lock_test_and_set(&lock->turn, other);
  __sync_synchronize();
  
  while(lock->flag[other] && lock->turn == other) {
    __sync_synchronize();
    yield(); // Give up CPU instead of spinning
  }
  
  __sync_synchronize();
  return 0;
}

int
peterson_release(int lock_id, int role)
{
  if(lock_id < 0 || lock_id >= NPETERSON || role < 0 || role > 1)
    return -1;
    
  if(!peterson_locks[lock_id].valid)
    return -1;
    
  struct petersonlock *lock = &peterson_locks[lock_id];
  
  __sync_synchronize();
  __sync_lock_release(&lock->flag[role]);
  
  return 0;
}

int
peterson_destroy(int lock_id)
{
  if(lock_id < 0 || lock_id >= NPETERSON)
    return -1;
    
  acquire(&peterson_global_lock);
  
  if(!peterson_locks[lock_id].valid) {
    release(&peterson_global_lock);
    return -1;
  }
  
  peterson_locks[lock_id].valid = 0;
  peterson_locks[lock_id].turn = 0;
  peterson_locks[lock_id].flag[0] = 0;
  peterson_locks[lock_id].flag[1] = 0;
  
  release(&peterson_global_lock);
  return 0;
}
