//
// Created by pablo on 12/12/24.
//

#ifndef VALLOC_INTERNAL_H
#define VALLOC_INTERNAL_H

#include <stddef.h>

// Allocate virtual memory
void* virtual_alloc(size_t size_in_bytes);

// Allocate shared virtual memory
void* virtual_shared_alloc(size_t size_in_bytes);

// Release virtual allocated memory
int virtual_free(void* valloc, size_t size_in_bytes);

#endif //VALLOC_INTERNAL_H
