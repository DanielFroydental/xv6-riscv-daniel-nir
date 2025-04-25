#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16)  // 65536
#define NUM_CHILDREN 4
#define NPROC 64  // Maximum number of processes
#define FINAL_SUM 2147450880  // Expected sum of the array

int main() {
  int i;
  static int array[ARRAY_SIZE];
  int pids[NUM_CHILDREN];
  int child_num;
  int start_idx, end_idx;
  int sum = 0;
  int statuses[NPROC];
  int n;
  
  for(i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }
  
  int ret = forkn(NUM_CHILDREN, pids);
  if(ret < 0) {
    exit(1, "forkn failed");
  } else if(ret > 0) {
    child_num = ret - 1;
    start_idx = (ARRAY_SIZE / NUM_CHILDREN) * child_num;
    end_idx = (child_num == NUM_CHILDREN - 1) ? 
              ARRAY_SIZE : (ARRAY_SIZE / NUM_CHILDREN) * (child_num + 1);
    
    sum = 0;
    for(i = start_idx; i < end_idx; i++) {
      sum += array[i];
    }
    printf("Child %d: PID: %d, Sum: %d\n", child_num, getpid(), sum);
    exit(sum, "");
  }

  if(waitall(&n, statuses) < 0) {
    exit(1, "waitall failed");
  }
  
  if(n != NUM_CHILDREN) {
    exit(1, "wrong number of children");
  }
  if (ret == 0) {
    printf("Child processes completed successfully:\n");
    for (i = 0; i < NUM_CHILDREN; i++) {
      printf("PID: %d, Sum: %d\n", pids[i], statuses[i]);
    }
  }
  
  sum = 0;
  for(i = 0; i < n; i++) {
    sum += statuses[i];
  }
  
  printf("Total sum of all children: %d\n", sum);
  if(sum == FINAL_SUM) {
    printf("Success: Calculation completed successfully\n");
    exit(0, "");
  } else {
    printf("Error: Calculation produced incorrect result\n");
    exit(1, "");
  }
}
