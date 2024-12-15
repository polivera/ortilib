//
// Created by pablo on 12/12/24.
//

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

// Allocate virtual memory
void* virtual_alloc(const size_t size_in_bytes)
{
    return mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE,
                MAP_NORESERVE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

// Allocate virtual shared memory
void* virtual_shared_alloc(const size_t size_in_bytes)
{
    return mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE,
                MAP_NORESERVE | MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

// Release virtual allocated memory
int virtual_free(void* valloc, const size_t size_in_bytes)
{
    return munmap(valloc, size_in_bytes);
}
