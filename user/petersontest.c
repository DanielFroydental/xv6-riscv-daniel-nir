#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int lock_id = peterson_create();
  if (lock_id < 0) {
    printf("Failed to create Peterson lock\n");
    exit(1);
  }
  
  printf("Created Peterson lock with ID: %d\n", lock_id);
  
  int fork_ret = fork();
  if (fork_ret < 0) {
    printf("Fork failed\n");
    exit(1);
  }
  
  int role = fork_ret > 0 ? 0 : 1;
  char *process_name = role == 0 ? "Parent" : "Child";
  
  for (int i = 0; i < 10; i++) {
    if (peterson_acquire(lock_id, role) < 0) {
      printf("%s: Failed to acquire lock\n", process_name);
      exit(1);
    }
    
    // Critical section
    printf("%s process in critical section (iteration %d)\n", process_name, i);
    sleep(1); // Simulate some work
    
    if (peterson_release(lock_id, role) < 0) {
      printf("%s: Failed to release lock\n", process_name);
      exit(1);
    }
    
    sleep(1); // Give other process a chance
  }
  
  if (fork_ret > 0) {
    // Parent waits for child and destroys lock
    wait(0);
    printf("Parent process destroying lock\n");
    if (peterson_destroy(lock_id) < 0) {
      printf("Failed to destroy lock\n");
      exit(1);
    }
    printf("Peterson lock test completed successfully!\n");
  }
  
  exit(0);
}
