//
// Created by pablo on 12/12/24.
//

#include <assert.h>
#include <stdio.h>
#include "orarena/orarena.h"

#include <stdint.h>

// Test cases
void test_valid_creation()
{
    const size_t initial_size = 1024;
    struct ORArena* arena = arena_create(initial_size);

    assert(arena != NULL);
    assert(arena->total_size == initial_size);
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

void test_minimum_size()
{
    const size_t min_size = 1;
    struct ORArena* arena = arena_create(min_size);

    assert(arena != NULL);
    assert(arena->total_size == 1); // Only 1 byte available for allocation
    assert(arena->used_size == 0);
    assert(arena->previous_size == 0);

    // Cleanup
    arena_destroy(arena);
    printf("✓ test_minimum_size passed\n");
}

void test_arena_basic_allocation()
{
    // Create an arena with 1024 bytes
    struct ORArena* arena = arena_create(1024);
    assert(arena != NULL);

    // Test basic allocation
    void* ptr1 = arena_alloc(arena, 100);
    assert(ptr1 != NULL);
    assert(arena->used_size >= 100);

    // Test multiple allocations
    void* ptr2 = arena_alloc(arena, 200);
    assert(ptr2 != NULL);
    assert(ptr2 > ptr1);
    assert(arena->used_size >= 300);

    // Test allocation failure (try to allocate more than remaining space)
    void* ptr3 = arena_alloc(arena, 2000);
    assert(ptr3 == NULL);

    arena_destroy(arena);
    printf("✓ Basic allocation tests passed!\n");
}

void test_arena_aligned_allocation()
{
    struct ORArena* arena = arena_create(1024);
    assert(arena != NULL);

    // Test aligned allocation with different alignments
    void* ptr1 = arena_alloc_aligned(arena, 50, 8);
    assert(ptr1 != NULL);
    assert(((uintptr_t)ptr1 & 7) == 0); // Check 8-byte alignment

    void* ptr2 = arena_alloc_aligned(arena, 50, 16);
    assert(ptr2 != NULL);
    assert(((uintptr_t)ptr2 & 15) == 0); // Check 16-byte alignment

    // Test that aligned allocation preserves spacing
    assert(ptr2 > ptr1);

    arena_destroy(arena);
    printf("✓ Aligned allocation tests passed!\n");
}

void test_arena_boundaries()
{
    // Test with a small arena to check boundary conditions
    struct ORArena* arena = arena_create(64);
    assert(arena != NULL);

    // Test allocations near the boundary
    const void* ptr1 = arena_alloc(arena, 32);
    assert(ptr1 != NULL);

    // This should still succeed
    const void* ptr2 = arena_alloc(arena, 16);
    assert(ptr2 != NULL);

    // This should fail (beyond capacity)
    void* ptr3 = arena_alloc(arena, 32);
    assert(ptr3 == NULL);

    arena_destroy(arena);
    printf("✓ Boundary tests passed!\n");
}

// Main test runner
int main()
{
    printf("Running arena creation tests...\n");

    test_valid_creation();
    test_zero_size();
    test_minimum_size();

    test_arena_basic_allocation();
    test_arena_aligned_allocation();
    test_arena_boundaries();

    printf("All tests passed!\n");
    return 0;
}
