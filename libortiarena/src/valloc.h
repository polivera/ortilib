#ifndef __ORTI_ARENA_VALLOC__
#define __ORTI_ARENA_VALLOC__
#include <stddef.h>
#include <stdint.h>

// Allocate virtual memory
void *virtual_alloc(uint64_t alloc_in_mb);

// Release virtual allocated memory
int virtual_free(void *valloc, uint64_t size_in_mb);

#endif
