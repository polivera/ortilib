#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t total_size;
  size_t used_size;
  void *bucket;
} Arena;

/**
 * Create a new arena of the specified size and return it.
 *
 * @param size_t size
 * @return *Arena
 */
Arena *new_arena(size_t size) {
  Arena *arena = malloc(sizeof(Arena));
  arena->bucket = malloc(size);
  if (NULL == arena->bucket) {
    perror("cannot allocate arena");
    return NULL;
  }
  arena->total_size = size;
  arena->used_size = 0;
  return arena;
}

/**
 * Free the memory of the arena.
 *
 * @param Arena **arena
 */
void arena_release(Arena *arena) {
  free(arena->bucket);
  free(arena);
}

/**
 * Reset the size of the arena back to zero
 *
 * @param Arena *arena
 */
void arena_clear(Arena *arena) { arena->used_size = 0; }

/**
 * Memory chunk.
 *
 * @param Arena *arena
 * @param size_t size
 * @return *void
 */
void *arena_push_no_zero(Arena *arena, size_t size) {
  if (arena->total_size < (arena->used_size + size)) {
    perror("Arena out of memory");
    return NULL;
  }
  void *result = arena->bucket + arena->used_size;
  arena->used_size += size;
  return result;
}

/**
 * Zero-fill memory chunk.
 *
 * @param Arena *arena
 * @param size_t size
 * @return *void
 */
void *arena_push(Arena *arena, size_t size) {
  void *result = arena_push_no_zero(arena, size);
  if (NULL != result) {
    memset(result, 0, size);
  }
  return result;
}

/**
 * Free memory up to position.
 * Position cannot be greater than the used size.
 *
 * @param Arena *arena
 * @param size_t position
 */
void arena_pop_to(Arena *arena, size_t position) {
  if (position <= arena->used_size) {
    arena->used_size = position;
    return;
  }
  perror("Trying to pop out of range");
}

/**
 * Release given size of memory.
 * Position cannot be greater than the used size.
 *
 * @param Arena *arena
 * @param size_t position
 */
void arena_pop(Arena *arena, size_t size) {
  if ((arena->used_size - size) > 0) {
    arena->used_size -= size;
    return;
  }
  perror("Trying to pop more memory than is used");
}
