#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int processes = 16;  // Default to 16 processes
    
    if (argc > 1) {
        processes = atoi(argv[1]);
    }
    
    // Validate input
    if (processes <= 0 || processes > 16 || (processes & (processes - 1)) != 0) {
        printf("Error: Number of processes must be a power of 2 up to 16\n");
        exit(1);
    }
    
    printf("Creating tournament tree with %d processes\n", processes);
    
    int tournament_id = tournament_create(processes);
    if (tournament_id == -1) {
        printf("Failed to create tournament tree\n");
        exit(1);
    }
    
    // Each process acquires the lock, prints, and releases
    if (tournament_acquire() == 0) {
        printf("Process PID=%d, Tournament ID=%d acquired the root lock\n", getpid(), tournament_id);
        
        if (tournament_release() == -1) {
            printf("Process PID=%d failed to release lock\n", getpid());
        }
    } else {
        printf("Process PID=%d failed to acquire lock\n", getpid());
    }
    
    exit(0);
}
