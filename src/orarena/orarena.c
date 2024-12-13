//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include "valloc_internal.h"
#include <stdio.h>

struct ORArena* arena_create(const size_t initial_size)
{
    const size_t arena_struct_size = sizeof(struct ORArena);
    if (initial_size <= 0 || arena_struct_size > initial_size)
    {
        fprintf(stderr, "ORArena: Initial size is invalid.\n");
        return NULL;
    }
    struct ORArena* arena = virtual_shared_alloc(initial_size);
    if (arena == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    arena->total_size = initial_size - arena_struct_size;
    arena->used_size = 0;
    arena->previous_size = 0;
    // Set the memory pointer at the start of the memory field.
    arena->memory = (void*)((char*)arena + arena_struct_size);
    return arena;
}

void arena_destroy(struct ORArena* arena)
{
    if (virtual_free(arena, arena->total_size) == 1)
    {
        fprintf(stderr, "Cannot release memory\n");
    }
}

void* arena_alloc(struct ORArena* arena, const size_t size)
{
    if (arena->used_size + size > arena->total_size)
    {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    // TODO: check alignment
    arena->previous_size = arena->used_size;
    arena->used_size += size;
    return arena->memory + arena->used_size;
}
