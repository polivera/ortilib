#include "./valloc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: This should be rewrite this stuff, keep in mind MAP_SHARED memory and
// go the functional route to return the "modified" arena in all cases

#define ORTI_BUCKET_SIZE_MB 2048
#define ORTI_BYTES_IN_MB 1048576

typedef struct {
  size_t total_size;
  size_t used_size;
  void *bucket;
} Arena;

// TODO Is there a way to move chunk of bytes back inside the bucket

/**
 * Create a new arena of the specified size and return it.
 *
 * @param size_t size
 * @return *Arena
 */
Arena *new_arena() {
  Arena *arena = virtual_alloc((size_t)ORTI_BUCKET_SIZE_MB * ORTI_BYTES_IN_MB);
  if (NULL == arena) {
    perror("cannot allocate arena");
    return NULL;
  }
  arena->total_size = (size_t)ORTI_BUCKET_SIZE_MB * ORTI_BYTES_IN_MB;
  arena->used_size = 0;
  arena->bucket = (void *)((char *)arena + sizeof(Arena));

  return arena;
}

/**
 * Free the memory of the arena.
 *
 * @param Arena **arena
 */
void arena_release(Arena **arena) {
  virtual_free(&arena, (*arena)->total_size);
  *arena = NULL;
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
    size_t arena_previous_size = arena->total_size;
    while (arena->total_size < (arena->used_size + size)) {
      arena->total_size += ORTI_BUCKET_SIZE_MB;
    }
    arena->bucket = realloc(arena->bucket, arena->total_size);
    if (NULL == arena->bucket) {
      perror("Can't reallocate arena size");
      arena->total_size -= arena_previous_size;
      return NULL;
    }
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
