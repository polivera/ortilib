//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include "valloc_internal.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OR_DEBUG_MEMORY
#define OR_LOG_ARENA_ALLOC(arena, size, aligned)                               \
    log_arena_alloc(arena, size, aligned)
#define OR_LOG_ARENA_RESET(arena) log_arena_reset(arena)
#define OR_LOG_ARENA_CREATE(arena, size) log_arena_create(arena, size)
#define OR_LOG_ARENA_DESTROY(arena) log_arena_destroy(arena)
#define OR_LOG_ARENA_POOL_CREATE(arena, chunk_size, chunk_count, alignment)    \
    log_arena_pool_create(arena, chunk_size, chunk_count, alignment)
#define OR_LOG_ARENA_POOL_ALLOC_FREE(arena) log_arena_pool_alloc_free(arena)
#else
#define OR_LOG_ARENA_ALLOC(arena, size, aligned)
#define OR_LOG_ARENA_RESET(arena)
#define OR_LOG_ARENA_CREATE(arena, size)
#define OR_LOG_ARENA_DESTROY(arena)
#define OR_LOG_ARENA_POOL_CREATE(arena, chunk_size, chunk_count, alignment)
#define OR_LOG_ARENA_POOL_FREE(arena)
#endif

static void
log_arena_alloc(const struct ORArena *arena, const size_t size,
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
log_arena_reset(const struct ORArena *arena) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Reset (previous used: %zu)\n", (const void *)arena,
               arena->used_size);
    } else {
        printf("Sub [ORArena %p] Reset (previous used: %zu)\n",
               (const void *)arena, arena->used_size);
    }
}

static void
log_arena_create(const struct ORArena *arena, const size_t size) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Created with size %zu\n", (const void *)arena,
               size);
    } else {
        printf("Sub [ORArena %p] Created with size %zu\n", (const void *)arena,
               size);
    }
}

static void
log_arena_destroy(const struct ORArena *arena) {
    if (arena->parent == NULL) {
        printf("[ORArena %p] Destroyed (final used: %zu/%zu)\n",
               (const void *)arena, arena->used_size, arena->total_size);
    } else {
        printf("Sub [ORArena %p] Destroyed (final used: %zu/%zu)\n",
               (const void *)arena, arena->used_size, arena->total_size);
    }
}

static void
log_arena_pool_create(const struct ORArenaPool *arena, const size_t chunk_size,
                      const uint32_t chunk_count, const size_t alignment) {
    printf(
        "[ORArenaPool %p] Created %d chunks of size %zu (with alignment %zu)\n",
        arena, chunk_count, chunk_size, alignment);
}

static void
log_arena_pool_alloc_free(const struct ORArenaPool *arena) {
    printf("[ORArenaPool %p] Changed [total: %d, allocated: %d, in pool: %d]",
           arena, arena->chunks_total, arena->chunks_allocated,
           arena->chunks_in_pool);
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
    OR_LOG_ARENA_CREATE(arena, initial_size);
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
    OR_LOG_ARENA_DESTROY(arena);
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
    OR_LOG_ARENA_ALLOC(arena, size, alignment);
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
    OR_LOG_ARENA_RESET(arena);
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
    OR_LOG_ARENA_CREATE(sub_arena, initial_size);
    return sub_arena;
}

struct ORArenaPool *
sub_arena_pool_create_aligned(struct ORArena *parent_arena,
                              const size_t chunk_size,
                              const uint32_t chunk_count,
                              const size_t alignment) {
    if (alignment % 2 != 0) {
        fprintf(stderr, "Alignment must be a multiple of 2\n");
        return NULL;
    }
    if (chunk_count == 0) {
        fprintf(stderr, "Chunk count must be greater than 0\n");
        return NULL;
    }
    if (chunk_size < sizeof(void *)) {
        fprintf(stderr, "Chunk size must be greater than %lu\n",
                sizeof(void *));
        return NULL;
    }
    const size_t free_list_size = sizeof(void *) * chunk_count;
    const size_t chunks_pool_size = chunk_size * chunk_count;
    const size_t total_size =
        sizeof(struct ORArenaPool) + free_list_size + chunks_pool_size;
    struct ORArenaPool *arena_pool =
        arena_alloc_aligned(parent_arena, total_size, alignment);
    if (arena_pool == NULL) {
        return NULL;
    }

    arena_pool->chunks_total = chunk_count;
    arena_pool->chunk_size = chunk_size;
    arena_pool->chunks_allocated = 0;
    arena_pool->chunks_in_pool = 0;
    arena_pool->memory =
        (void *)((char *)arena_pool + sizeof(struct ORArenaPool));
    OR_LOG_ARENA_POOL_CREATE(arena_pool, chunk_size, chunk_count, alignment);
    return arena_pool;
}

struct ORArenaPool *
sub_arena_pool_create(struct ORArena *parent_arena, const size_t chunk_size,
                      const uint32_t chunk_count) {
    return sub_arena_pool_create_aligned(parent_arena, chunk_size, chunk_count,
                                         DEFAULT_ALIGNMENT);
}

void *
sub_arena_pool_alloc(struct ORArenaPool *pool_arena) {
    if (pool_arena == NULL ||
        pool_arena->chunks_in_pool == 0 &&
            pool_arena->chunks_allocated >= pool_arena->chunks_total) {
        return NULL;
    }

    if (pool_arena->chunks_in_pool > 0) {
        void **free_list = pool_arena->memory;
        pool_arena->chunks_in_pool--;
        return free_list[pool_arena->chunks_in_pool];
    }

    const size_t aligned_allocation_size =
        sizeof(void *) * pool_arena->chunks_total +
        pool_arena->chunk_size * pool_arena->chunks_allocated;
    void *result_ptr = pool_arena->memory +
                       align_up(aligned_allocation_size, pool_arena->alignment);
    pool_arena->chunks_allocated++;
    OR_LOG_ARENA_POOL_ALLOC_FREE(pool_arena);
    return result_ptr;
}

void
sub_arena_pool_free(struct ORArenaPool *pool_arena, void *ptr_to_free) {
    if (pool_arena == NULL || ptr_to_free == NULL) {
        return;
    }
    void **free_list = pool_arena->memory;

    memset(ptr_to_free, 0, pool_arena->chunk_size);
    free_list[pool_arena->chunks_in_pool] = ptr_to_free;
    pool_arena->chunks_in_pool++;
}

void
sub_arena_destroy(const struct ORArena *sub_arena) {
    OR_LOG_ARENA_DESTROY(sub_arena);
    memset((void *)sub_arena, 0,
           sub_arena->total_size + sizeof(struct ORArena));
}
