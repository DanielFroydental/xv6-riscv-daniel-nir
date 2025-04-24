#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16)  // 2^16 = 65536
#define NUM_CHILDREN 4
#define NPROC 64  // Maximum number of processes, defined in kernel/param.h

int main() {
  int i;
  static int array[ARRAY_SIZE];  // Make the array static so it's not on the stack
  int pids[NUM_CHILDREN];
  int child_num;
  int start_idx, end_idx;
  int sum = 0;
  int statuses[NPROC];  // Max NPROC processes
  int n;
  
  // Initialize array with consecutive numbers
  for(i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }
  
  // Create NUM_CHILDREN child processes
  int ret = forkn(NUM_CHILDREN, pids);
  if(ret < 0) {
    exit(1, "forkn failed");
  } else if(ret > 0) {
    // Child process, ret is the child number (1-based)
    child_num = ret - 1;  // Convert to 0-based index
    
    // Calculate the range this child should process
    start_idx = (ARRAY_SIZE / NUM_CHILDREN) * child_num;
    end_idx = (child_num == NUM_CHILDREN - 1) ? 
              ARRAY_SIZE : (ARRAY_SIZE / NUM_CHILDREN) * (child_num + 1);
    
    // Calculate sum for this portion
    sum = 0;
    for(i = start_idx; i < end_idx; i++) {
      sum += array[i];
    }
    
    // Exit with the sum as the status
    exit(sum, "Child process completed");
  }
  
  // Parent waits for all children to complete
  if(waitall(&n, statuses) < 0) {
    exit(1, "waitall failed");
  }
  
  // Verify we got the expected number of children
  if(n != NUM_CHILDREN) {
    exit(1, "wrong number of children");
  }
  
  // Calculate total sum from child results
  sum = 0;
  for(i = 0; i < n; i++) {
    sum += statuses[i];
  }
  
  // The theoretical sum should be 0+1+2+...+(2^16-1) = (2^16-1)*(2^16)/2 = 2147450880
  printf("Total sum of all elements: %d\n", sum);
  if(sum == 2147450880) {
    printf("Success: Calculation completed successfully\n");
    exit(0, "Calculation completed successfully");
  } else {
    printf("Error: Calculation produced incorrect result\n");
    exit(1, "Calculation produced incorrect result");
  }
}
