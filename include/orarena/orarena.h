//
// Created by pablo on 12/12/24.
//

#ifndef ORARENA_H
#define ORARENA_H

#include <stddef.h>

#define MAX_ALIGNMENT 16

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

struct ORArena {
    size_t total_size;
    size_t used_size;
    size_t previous_size;
    struct ORArena *parent;
    void *memory;
};

/**
 * @brief Creates a new arena with the specified initial size
 *
 * @param initial_size Size in bytes for the arena's memory block
 * @return struct ORArena* Pointer to the new arena, or NULL if creation fails
 */
struct ORArena *arena_create(size_t initial_size);

/**
 * @brief Creates a new shared memory arena with the specified initial size
 *
 * @param initial_size Size in bytes for the shared memory block
 * @return struct ORArena* Pointer to the new shared arena, or NULL if creation
 * fails
 */
struct ORArena *arena_create_shared(size_t initial_size);

/**
 * @brief Destroys an arena and frees all associated memory
 *
 * @param arena Pointer to the arena to destroy
 */
void arena_destroy(struct ORArena *arena);

/**
 * @brief Allocates memory from the arena with default alignment
 *
 * @param arena Pointer to the arena
 * @param size Number of bytes to allocate
 * @return void* Pointer to the allocated memory, or NULL if allocation fails
 */
void *arena_alloc(struct ORArena *arena, size_t size);

/**
 * @brief Allocates memory from the arena with specified alignment
 *
 * @param arena Pointer to the arena
 * @param size Number of bytes to allocate
 * @param alignment Required alignment in bytes (must be power of 2)
 * @return void* Pointer to the aligned memory, or NULL if allocation fails
 */
void *arena_alloc_aligned(struct ORArena *arena, size_t size, size_t alignment);

/**
 * @brief Reverts the last allocation made in the arena
 *
 * @param arena Pointer to the arena
 */
void arena_pop(struct ORArena *arena);

/**
 * @brief Resets the arena to its initial empty state
 *
 * All allocations are invalidated, but the memory block is retained.
 *
 * @param arena Pointer to the arena
 */
void arena_reset(struct ORArena *arena);

/**
 * @brief Creates a sub-arena within the specified parent arena
 *
 * @param arena Pointer to the parent arena
 * @param initial_size Size in bytes for the sub-arena
 * @return struct ORArena* Pointer to the new sub-arena, or NULL if creation
 * fails
 */
struct ORArena *sub_arena_create(struct ORArena *arena, size_t initial_size);

/**
 * @brief Destroys a sub-arena
 *
 * Note: This does not automatically free the memory in the parent arena if
 * there are allocations stacked after this sub-arena.
 *
 * @param sub_arena Pointer to the sub-arena to destroy
 */
void sub_arena_destroy(const struct ORArena *sub_arena);

#endif // ORARENA_H
