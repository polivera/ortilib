//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include "valloc_internal.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef OR_DEBUG_MEMORY
#define OR_LOG_ALLOC(arena, size, aligned)                                     \
    int_or_log_alloc(arena, size, aligned)
#define OR_LOG_RESET(arena) int_or_log_reset(arena)
#define OR_LOG_CREATE(arena, size) int_or_log_create(arena, size)
#define OR_LOG_DESTROY(arena) int_or_log_destroy(arena)
#else
#define OR_LOG_ALLOC(arena, size, aligned)
#define OR_LOG_RESET(arena)
#define OR_LOG_CREATE(arena, size)
#define OR_LOG_DESTROY(arena)
#endif

static void
int_or_log_alloc(const struct ORArena *arena, const size_t size,
                 const size_t aligned) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Allocating %zu bytes (aligned: %zu). Used %zu\n",
               (const void *)arena, size, aligned, arena->used_size);
    } else {
        printf(
            "Sub [ORArena %p] Allocating %zu bytes (aligned: %zu). Used %zu\n",
            (const void *)arena, size, aligned, arena->used_size);
    }
}
static void
int_or_log_reset(const struct ORArena *arena) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Reset (previous used: %zu)\n", (const void *)arena,
               arena->used_size);
    } else {
        printf("Sub [ORArena %p] Reset (previous used: %zu)\n",
               (const void *)arena, arena->used_size);
    }
}
static void
int_or_log_create(const struct ORArena *arena, const size_t size) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Created with size %zu\n", (const void *)arena,
               size);
    } else {
        printf("Sub [ORArena %p] Created with size %zu\n", (const void *)arena,
               size);
    }
}
static void
int_or_log_destroy(const struct ORArena *arena) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Destroyed (final used: %zu/%zu)\n",
               (const void *)arena, arena->used_size, arena->total_size);
    } else {
        printf("Sub [ORArena %p] Destroyed (final used: %zu/%zu)\n",
               (const void *)arena, arena->used_size, arena->total_size);
    }
}

static size_t
align_up(const size_t size, const size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

static struct ORArena *
inter_arena_create(const size_t initial_size, const bool is_shared) {
    const size_t arena_struct_size = sizeof(struct ORArena);
    if (initial_size <= 0) {
        fprintf(stderr, "ORArena: Initial size is invalid.\n");
        return NULL;
    }

    const size_t total_size =
        align_up(initial_size + arena_struct_size, MAX_ALIGNMENT);

    struct ORArena *arena;
    if (is_shared) {
        arena = virtual_shared_alloc(total_size);
    } else {
        arena = virtual_alloc(total_size);
    }

    if (arena == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    arena->total_size = initial_size;
    arena->used_size = 0;
    arena->previous_size = 0;
    // Set the memory pointer at the start of the memory field.
    arena->memory =
        (void *)((char *)arena + align_up(arena_struct_size, MAX_ALIGNMENT));
    OR_LOG_CREATE(arena, initial_size);
    return arena;
}

struct ORArena *
arena_create(const size_t initial_size) {
    struct ORArena *arena = inter_arena_create(initial_size, false);
    return arena;
}

struct ORArena *
arena_create_shared(const size_t initial_size) {
    return inter_arena_create(initial_size, true);
}

void
arena_destroy(struct ORArena *arena) {
    if (arena->parent != NULL) {
        sub_arena_destroy(arena);
        return;
    }
    if (virtual_free(arena, arena->total_size) == 1) {
        fprintf(stderr, "Cannot release memory\n");
        return;
    }
    OR_LOG_DESTROY(arena);
}

void *
arena_alloc_aligned(struct ORArena *arena, const size_t size,
                    const size_t alignment) {
    if (alignment > MAX_ALIGNMENT) {
        fprintf(stderr, "Max alignment exceeded\n");
        return NULL;
    }
    const size_t aligned_start = align_up(arena->used_size, alignment);
    const size_t new_used_size = aligned_start + size;

    if (new_used_size > arena->total_size) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    arena->previous_size = arena->used_size;
    arena->used_size = new_used_size;
    OR_LOG_ALLOC(arena, size, alignment);
    return arena->memory + aligned_start;
}

void *
arena_alloc(struct ORArena *arena, const size_t size) {
    return arena_alloc_aligned(arena, size, DEFAULT_ALIGNMENT);
}

void
arena_pop(struct ORArena *arena) {
    arena->used_size = arena->previous_size;
}

void
arena_reset(struct ORArena *arena) {
    OR_LOG_RESET(arena);
    arena->used_size = 0;
}

struct ORArena *
sub_arena_create(struct ORArena *arena, const size_t initial_size) {
    if (initial_size <= 0) {
        fprintf(stderr, "ORSubArena: Initial size is invalid.\n");
        return NULL;
    }
    struct ORArena *sub_arena =
        arena_alloc(arena, sizeof(struct ORArena) + initial_size);
    if (sub_arena == NULL) {
        return NULL;
    }
    sub_arena->total_size = initial_size;
    sub_arena->used_size = 0;
    sub_arena->parent = arena;
    sub_arena->memory = (void *)((char *)sub_arena + sizeof(struct ORArena));
    // Don't allow arena to be reset
    arena->previous_size = arena->used_size;
    OR_LOG_CREATE(sub_arena, initial_size);
    return sub_arena;
}

void
sub_arena_destroy(const struct ORArena *sub_arena) {
    OR_LOG_DESTROY(sub_arena);
    memset((void *)sub_arena, 0,
           sub_arena->total_size + sizeof(struct ORArena));
}
