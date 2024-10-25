#ifndef __ORTI_ARENA__
#define __ORTI_ARENA__

#include <stdlib.h>

typedef struct {
  size_t total_size;
  size_t used_size;
  void *bucket;
} Arena;

/**
 * @brief Creates a new arena with the specified size.
 *
 * This function allocates memory for a new arena of a given size and
 * initializes it. The memory for the arena itself is dynamically allocated and
 * managed by the caller.
 *
 * @param size The size of the memory block to allocate for the arena (in
 * bytes).
 *
 * @return A pointer to the newly created Arena structure, or NULL if memory
 * allocation fails.
 *
 * @note The caller is responsible for releasing the memory of the arena using
 * `arena_release()`.
 */
Arena *new_arena(size_t size);

/**
 * @brief Releases the memory associated with an arena.
 *
 * This function frees the memory allocated for the arena's internal memory
 * block and
 * the arena structure itself.
 *
 * @param arena A pointer to the arena structure to be released.
 *
 * @note After calling this function, the arena pointer becomes null
 */
void arena_release(Arena *arena);

/**
 * @brief Clears the arena by resetting the used memory to 0.
 *
 * This function resets the arena's `used` pointer to 0, making all memory
 * previously allocated available for reuse. It does not free or modify the
 * actual memory block, so the arena can continue to allocate new memory from
 * the same block.
 *
 * @param arena A pointer to the arena structure to clear.
 *
 * @note This function only resets the used pointer and does not modify the
 * actual memory. Any data previously allocated remains in the memory, but is
 * now considered invalid by the arena.
 */
void arena_clear(Arena *arena);

/**
 * @brief Allocates memory from the arena without initializing it to zero.
 *
 * This function pushes a block of memory from the arena of the specified size.
 * The memory is not zeroed out, meaning it may contain arbitrary data.
 *
 * @param arena A pointer to the arena structure.
 * @param size The size of the memory block to allocate (in bytes).
 *
 * @return A pointer to the allocated memory, or NULL if there is not enough
 * space in the arena.
 *
 * @note The arena's used pointer is incremented by the size of the allocation.
 */
void *arena_push_no_zero(Arena *arena, size_t size);

/**
 * @brief Allocates zero-initialized memory from the arena.
 *
 * This function pushes a block of memory from the arena of the specified size,
 * initializing the memory to zero.
 *
 * @param arena A pointer to the arena structure.
 * @param size The size of the memory block to allocate (in bytes).
 *
 * @return A pointer to the allocated memory, or NULL if there is not enough
 * space in the arena.
 *
 * @note The arena's used pointer is incremented by the size of the allocation.
 * The allocated memory is zero-initialized.
 */
void *arena_push(Arena *arena, size_t size);

/**
 * @brief Resets the arena's used pointer to a specific position.
 *
 * This function allows the arena to "free" memory by moving its internal
 * used pointer back to a specified position. Any memory allocated after
 * this position is no longer considered used.
 *
 * @param arena A pointer to the arena structure.
 * @param pos The position to which the arena's used pointer should be moved.
 *            Must be less than or equal to the current used size.
 *
 * @note If `pos` is greater than the current used size, this function will
 * error out
 */
void arena_pop_to(Arena *arena, size_t position);

/**
 * @brief Reduces the arena's used memory by a specified size.
 *
 * This function pops (or frees) a certain amount of memory from the arena
 * by reducing the `used` pointer. The memory after the `used` pointer is no
 * longer considered valid.
 *
 * @param arena A pointer to the arena structure.
 * @param size The amount of memory to pop (in bytes). If `size` is larger than
 *             the current used size, the arena's used size is reset to 0.
 *
 * @note If `size` exceeds the current used memory, the function will error out
 */
void arena_pop(Arena *arena, size_t size);

#endif
