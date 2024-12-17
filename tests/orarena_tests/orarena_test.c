//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// Test cases
void test_valid_creation() {
    const size_t initial_size = 1024;
    struct ORArena *arena = arena_create(initial_size);

    assert(arena != NULL);
    assert(arena->total_size == initial_size);
    assert(arena->used_size == 0);
    assert(arena->previous_size == 0);
    assert(arena->memory != NULL);

    // Verify memory is accessible and writable
    char *mem = arena->memory;
    mem[0] = 'a'; // Should not crash

    // Cleanup
    arena_destroy(arena);
    printf("✓ test_valid_creation passed\n");
}

void test_zero_size() {
    const struct ORArena *arena = arena_create(0);
    assert(arena == NULL);
    printf("✓ test_zero_size passed\n");
}

void test_minimum_size() {
    const size_t min_size = 1;
    struct ORArena *arena = arena_create(min_size);

    assert(arena != NULL);
    assert(arena->total_size == 1); // Only 1 byte available for allocation
    assert(arena->used_size == 0);
    assert(arena->previous_size == 0);

    // Cleanup
    arena_destroy(arena);
    printf("✓ test_minimum_size passed\n");
}

void test_arena_basic_allocation() {
    // Create an arena with 1024 bytes
    struct ORArena *arena = arena_create(1024);
    assert(arena != NULL);

    // Test basic allocation
    const void *ptr1 = arena_alloc(arena, 100);
    assert(ptr1 != NULL);
    assert(arena->used_size >= 100); // >= because of the alignment

    // Test multiple allocations
    const void *ptr2 = arena_alloc(arena, 200);
    assert(ptr2 != NULL);
    assert(ptr2 > ptr1);
    assert(arena->used_size >= 300); //  >= because of the alignment

    // Test allocation failure (try to allocate more than remaining space)
    const void *ptr3 = arena_alloc(arena, 2000);
    assert(ptr3 == NULL);

    arena_destroy(arena);
    printf("✓ Basic allocation tests passed!\n");
}

void test_arena_aligned_allocation() {
    struct ORArena *arena = arena_create(1024);
    assert(arena != NULL);

    // Test aligned allocation with different alignments
    void *ptr1 = arena_alloc_aligned(arena, 50, 8);
    assert(ptr1 != NULL);
    assert(((uintptr_t)ptr1 & 7) == 0); // Check 8-byte alignment

    void *ptr2 = arena_alloc_aligned(arena, 50, 16);
    assert(ptr2 != NULL);
    assert(((uintptr_t)ptr2 & 15) == 0); // Check 16-byte alignment

    // Test that aligned allocation preserves spacing
    assert(ptr2 > ptr1);

    arena_destroy(arena);
    printf("✓ Aligned allocation tests passed!\n");
}

void test_arena_boundaries() {
    // Test with a small arena to check boundary conditions
    struct ORArena *arena = arena_create(64);
    assert(arena != NULL);

    // Test allocations near the boundary
    const void *ptr1 = arena_alloc(arena, 32);
    assert(ptr1 != NULL);

    // This should still succeed
    const void *ptr2 = arena_alloc(arena, 16);
    assert(ptr2 != NULL);

    // This should fail (beyond capacity)
    void *ptr3 = arena_alloc(arena, 32);
    assert(ptr3 == NULL);

    arena_destroy(arena);
    printf("✓ Boundary tests passed!\n");
}

void test_sub_arena_creation() {
    // Create a parent arena with 1024 bytes
    struct ORArena *parent_arena = arena_create(1024);
    assert(parent_arena != NULL);

    // Test basic sub-arena creation
    struct ORArena *sub_arena = sub_arena_create(parent_arena, 256);
    assert(sub_arena != NULL);
    assert(sub_arena->total_size == 256);
    assert(sub_arena->used_size == 0);
    assert(sub_arena->parent == parent_arena);

    // Verify that parent arena's used size has increased
    assert(parent_arena->used_size >= (sizeof(struct ORArena) + 256));

    // Test zero size sub-arena creation (should fail)
    struct ORArena *invalid_sub_arena = sub_arena_create(parent_arena, 0);
    assert(invalid_sub_arena == NULL);

    // Test sub-arena creation when parent doesn't have enough space
    struct ORArena *too_large_sub_arena = sub_arena_create(parent_arena, 2048);
    assert(too_large_sub_arena == NULL);

    // Test sub-arena allocation
    void *sub_ptr = arena_alloc(sub_arena, 64);
    assert(sub_ptr != NULL);
    assert(sub_arena->used_size >= 64);

    // Test sub-arena boundary
    void *overflow_ptr = arena_alloc(sub_arena, 512);
    assert(overflow_ptr == NULL);

    // Cleanup - note that memory isn't returned to parent
    sub_arena_destroy(sub_arena);
    arena_destroy(parent_arena);
    printf("✓ Sub-arena creation tests passed!\n");
}

void test_sub_arena_memory_isolation() {
    struct ORArena *parent_arena = arena_create(1024);
    assert(parent_arena != NULL);

    // Create multiple sub-arenas
    struct ORArena *sub_arena1 = sub_arena_create(parent_arena, 128);
    struct ORArena *sub_arena2 = sub_arena_create(parent_arena, 128);
    assert(sub_arena1 != NULL);
    assert(sub_arena2 != NULL);

    // Allocate in each sub-arena
    void *ptr1 = arena_alloc(sub_arena1, 64);
    void *ptr2 = arena_alloc(sub_arena2, 64);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);

    // Verify memory isolation
    assert(ptr2 > ptr1);
    assert((char *)ptr2 >=
           ((char *)ptr1 + 128)); // Should be in separate regions

    // Test that destroying sub_arena1 doesn't affect sub_arena2
    sub_arena_destroy(sub_arena1);
    void *ptr3 = arena_alloc(sub_arena2, 32);
    assert(ptr3 != NULL);
    assert(ptr3 > ptr2);

    // Cleanup
    sub_arena_destroy(sub_arena2);
    arena_destroy(parent_arena);
    printf("✓ Sub-arena memory isolation tests passed!\n");
}

// Main test runner
int main() {
    printf("Running arena creation tests...\n");
    test_valid_creation();
    test_zero_size();
    test_minimum_size();
    printf("Running arena allocation tests...\n");
    test_arena_basic_allocation();
    test_arena_aligned_allocation();
    test_arena_boundaries();
    printf("Running sub-arena tests...\n");
    test_sub_arena_creation();
    test_sub_arena_memory_isolation();

    printf("All tests passed!\n");
    return 0;
}