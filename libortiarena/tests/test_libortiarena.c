#include "../include/libortiarena.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_new_arena() {
  printf("[TEST] - Testing method: new_arena\n");
  Arena *test_arena = new_arena(4096);
  assert(test_arena->total_size == 4096);
  assert(test_arena->used_size == 0);
  arena_release(test_arena);
}

void test_arena_release() {
  printf("[TEST] - Testing method: arena_release\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);
  arena_release(test_arena); // Release
}

void test_arena_push() {
  printf("[TEST] - Testing method: arena_push\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);

  char *test_str = arena_push(test_arena, sizeof(test_str) * 5);
  assert(test_arena->used_size == 40);
  assert(test_str[1] == 0);

  strncpy(test_str, "some", 5);
  char *test_bucket = (char *)test_arena->bucket;
  assert(strncmp(test_bucket, test_str, 5) == 0);

  arena_release(test_arena); 
}

void test_arena_clear() {
  printf("[TEST] - Testing method: arena_release\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);
  assert(test_arena->used_size == 0);

  char *test_str = arena_push(test_arena, sizeof(test_str) * 10);
  assert(test_arena->total_size == 1024);
  assert(test_arena->used_size == 80);

  arena_clear(test_arena);
  assert(test_arena->total_size == 1024);
  assert(test_arena->used_size == 0);

  arena_release(test_arena);
}

void test_arena_push_no_zero() {
  printf("[TEST] - Testing method: arena_push_no_zero\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);

  char *test_str = arena_push_no_zero(test_arena, sizeof(test_str) * 5);
  assert(test_arena->used_size == 40);

  strncpy(test_str, "some", 5);
  char *test_bucket = (char *)test_arena->bucket;
  assert(strncmp(test_bucket, test_str, 5) == 0);

  arena_release(test_arena);
}


void test_arena_pop() {
  printf("[TEST] - Testing method: arena_pop\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);

  char *test_str = arena_push_no_zero(test_arena, sizeof(test_str) * 5);
  assert(test_arena->used_size == 40);

  arena_pop(test_arena, 10);
  assert(test_arena->used_size == 30);

  arena_release(test_arena);
}


void test_arena_pop_to() {
  printf("[TEST] - Testing method: arena_pop_to\n");
  Arena *test_arena = new_arena(1024);
  assert(test_arena->total_size == 1024);

  char *test_str = arena_push_no_zero(test_arena, sizeof(test_str) * 5);
  assert(test_arena->used_size == 40);

  arena_pop_to(test_arena, 10);
  assert(test_arena->used_size == 10);

  arena_release(test_arena);
}

int main() {
  test_new_arena();
  test_arena_release();
  test_arena_push();
  test_arena_clear();
  test_arena_push_no_zero();
  test_arena_pop();
  test_arena_pop_to();
  return 0;
}
