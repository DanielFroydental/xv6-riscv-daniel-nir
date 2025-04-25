// memsize_test.c
// User-space test for memsize syscall in xv6-riscv

#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int before, after_alloc, after_free;
    void *p;

    // (a) Print memory usage before allocation
    before = memsize();
    printf("Memory usage before allocation: %d bytes\n", before);

    // (b) Allocate 20 KB more
    p = malloc(20 * 1024);

    // (c) Print memory usage after allocation
    after_alloc = memsize();
    printf("Memory usage after allocating 20 KB: %d bytes\n", after_alloc);

    // (d) Free the allocated block
    free(p);

    // (e) Print memory usage after free
    after_free = memsize();
    printf("Memory usage after free: %d bytes\n", after_free);

    exit(0, "");
}