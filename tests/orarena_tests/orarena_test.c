//
// Created by pablo on 12/12/24.
//

#include <assert.h>
#include <stdio.h>
#include "orarena/orarena.h"

// Test cases
void test_valid_creation()
{
    const size_t initial_size = 1024;
    struct ORArena* arena = arena_create(initial_size);

    assert(arena != NULL);
    assert(arena->total_size == initial_size - sizeof(struct ORArena));
    assert(arena->used_size == 0);
    assert(arena->previous_size == 0);
    assert(arena->memory == (void*)((char*)arena + sizeof(struct ORArena)));

    // Verify memory is accessible and writable
    char* mem = arena->memory;
    mem[0] = 'a'; // Should not crash

    // Cleanup
    arena_destroy(arena);
    printf("✓ test_valid_creation passed\n");
}

void test_zero_size()
{
    const struct ORArena* arena = arena_create(0);
    assert(arena == NULL);
    printf("✓ test_zero_size passed\n");
}

void test_size_too_small()
{
    const struct ORArena* arena = arena_create(sizeof(struct ORArena) - 1);
    assert(arena == NULL);
    printf("✓ test_size_too_small passed\n");
}

void test_minimum_size()
{
    const size_t min_size = sizeof(struct ORArena) + 1;
    struct ORArena* arena = arena_create(min_size);

    assert(arena != NULL);
    assert(arena->total_size == 1); // Only 1 byte available for allocation
    assert(arena->used_size == 0);
    assert(arena->previous_size == 0);

    // Cleanup
    arena_destroy(arena);
    printf("✓ test_minimum_size passed\n");
}

// Main test runner
int main()
{
    printf("Running arena creation tests...\n");

    test_valid_creation();
    test_zero_size();
    test_size_too_small();
    test_minimum_size();

    printf("All tests passed!\n");
    return 0;
}
