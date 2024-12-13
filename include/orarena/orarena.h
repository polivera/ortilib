//
// Created by pablo on 12/12/24.
//

#ifndef ORARENA_H
#define ORARENA_H

#include <stddef.h>

struct ORArena
{
    size_t total_size;
    size_t used_size;
    size_t previous_size;
    void* memory;
};

struct ORArena* arena_create(size_t initial_size); // Allow the possibility of using MAP_SHARED or not.
void arena_destroy(struct ORArena* arena);

void* arena_alloc(struct ORArena* arena, size_t size);
void* arena_alloc_aligned(struct ORArena* arena, size_t size, size_t alignment);

void arena_pop(struct ORArena* arena);

void arena_reset(struct ORArena* arena);
enum ORArenaError arena_release(struct ORArena* arena);

#endif //ORARENA_H
