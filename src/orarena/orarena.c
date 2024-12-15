//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include "valloc_internal.h"
#include <stdio.h>

#if defined(__x86_64__) || defined(_M_X64)
#define DEFAULT_ALIGNMENT 8
#elif defined(__i386__) || defined(_M_IX86)
    #define DEFAULT_ALIGNMENT 4
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define DEFAULT_ALIGNMENT 8
#elif defined(__arm__) || defined(_M_ARM)
    #define DEFAULT_ALIGNMENT 4
#else
    #define DEFAULT_ALIGNMENT 8
#endif

struct ORArena* arena_create(const size_t initial_size)
{
    const size_t arena_struct_size = sizeof(struct ORArena);
    if (initial_size <= 0)
    {
        fprintf(stderr, "ORArena: Initial size is invalid.\n");
        return NULL;
    }
    struct ORArena* arena = virtual_shared_alloc(initial_size + arena_struct_size);
    if (arena == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    arena->total_size = initial_size;
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
    return arena_alloc_aligned(arena, size, DEFAULT_ALIGNMENT);
}

void* arena_alloc_aligned(struct ORArena* arena, const size_t size, const size_t alignment)
{
    if (arena->used_size + size > arena->total_size)
    {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    arena->previous_size = arena->used_size;
    arena->used_size = (arena->used_size + size) + (alignment - 1) & ~(alignment - 1);
    return arena->memory + arena->used_size;
}

void arena_pop(struct ORArena* arena)
{
    arena->used_size = arena->previous_size;
}

void arena_reset(struct ORArena* arena)
{
    arena->used_size = 0;
}
