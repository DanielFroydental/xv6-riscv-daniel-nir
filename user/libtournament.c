#include "kernel/types.h"
#include "user/user.h"

static int *locks = 0;          // Array of Peterson lock IDs
static int process_index = -1;  // Index of current process (0 to N-1)
static int num_processes = 0;   // Total number of processes
static int num_levels = 0;      // Number of levels in tree
static int total_locks = 0;     // Total number of locks

int tournament_create(int processes) {
    if (processes <= 0 || processes > 16 || (processes & (processes - 1)) != 0) {
        return -1;
    }
    
    num_processes = processes;
    num_levels = 0;
    int temp = processes;
    while (temp > 1) {
        num_levels++;
        temp /= 2;
    }
    total_locks = processes - 1;
    
    // Allocate memory for locks array
    locks = malloc(total_locks * sizeof(int));
    if (locks == 0) {
        return -1;
    }
    
    // Create all Peterson locks
    for (int i = 0; i < total_locks; i++) {
        locks[i] = peterson_create();
        if (locks[i] == -1) {
            return -1;
        }
    }
    
    // Fork processes and assign indices
    for (int i = 1; i < processes; i++) {
        int pid = fork();
        if (pid == 0) {
            process_index = i;
            return i;
        } else if (pid < 0) {
            return -1;
        }
    }
    process_index = 0;
    return 0;
}

int tournament_acquire(void) {
    if (process_index == -1 || locks == 0) {
        return -1;
    }
    for (int level = num_levels - 1; level >= 0; level--) {
        int role = (process_index & (1 << (num_levels - level - 1))) >> (num_levels - level - 1);
        int lock_at_level = process_index >> (num_levels - level);
        int array_index = lock_at_level + ((1 << level) - 1);
        if (peterson_acquire(locks[array_index], role) == -1) {
            return -1;
        }
    }
    
    return 0;
}

int tournament_release(void) {
    if (process_index == -1 || locks == 0) {
        return -1;
    }
    for (int level = 0; level < num_levels; level++) {
        int role = (process_index & (1 << (num_levels - level - 1))) >> (num_levels - level - 1);
        int lock_at_level = process_index >> (num_levels - level);
        int array_index = lock_at_level + ((1 << level) - 1);
        if (peterson_release(locks[array_index], role) == -1) {
            return -1;
        }
    }
    
    return 0;
}
