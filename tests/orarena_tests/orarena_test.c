//
// Created by pablo on 12/12/24.
//

#include "orarena/orarena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// Test cases
void
test_valid_creation() {
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

// Test cases
void
test_valid_shared_creation() {
    const size_t initial_size = 1024;
    struct ORArena *arena = arena_create_shared(initial_size);

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
    printf("✓ test_valid_shared_creation passed\n");
}

void
test_zero_size() {
    const struct ORArena *arena = arena_create(0);
    assert(arena == NULL);
    printf("✓ test_zero_size passed\n");
}

void
test_minimum_size() {
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

void
test_arena_basic_allocation() {
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

void
test_arena_aligned_allocation() {
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

void
test_arena_boundaries() {
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
    const void *ptr3 = arena_alloc(arena, 32);
    assert(ptr3 == NULL);

    arena_destroy(arena);
    printf("✓ Boundary tests passed!\n");
}

void
test_sub_arena_creation() {
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
    const struct ORArena *invalid_sub_arena = sub_arena_create(parent_arena, 0);
    assert(invalid_sub_arena == NULL);

    // Test sub-arena creation when parent doesn't have enough space
    const struct ORArena *too_large_sub_arena =
        sub_arena_create(parent_arena, 2048);
    assert(too_large_sub_arena == NULL);

    // Test sub-arena allocation
    const void *sub_ptr = arena_alloc(sub_arena, 64);
    assert(sub_ptr != NULL);
    assert(sub_arena->used_size >= 64);

    // Test sub-arena boundary
    const void *overflow_ptr = arena_alloc(sub_arena, 512);
    assert(overflow_ptr == NULL);

    // Cleanup - note that memory isn't returned to parent
    sub_arena_destroy(sub_arena);
    arena_destroy(parent_arena);
    printf("✓ Sub-arena creation tests passed!\n");
}

void
test_sub_arena_memory_isolation() {
    struct ORArena *parent_arena = arena_create(1024);
    assert(parent_arena != NULL);

    // Create multiple sub-arenas
    struct ORArena *sub_arena1 = sub_arena_create(parent_arena, 128);
    struct ORArena *sub_arena2 = sub_arena_create(parent_arena, 128);
    assert(sub_arena1 != NULL);
    assert(sub_arena2 != NULL);

    // Allocate in each sub-arena
    const void *ptr1 = arena_alloc(sub_arena1, 64);
    const void *ptr2 = arena_alloc(sub_arena2, 64);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);

    // Verify memory isolation
    assert(ptr2 > ptr1);
    assert((char *)ptr2 >=
           ((char *)ptr1 + 128)); // Should be in separate regions

    // Test that destroying sub_arena1 doesn't affect sub_arena2
    sub_arena_destroy(sub_arena1);
    const void *ptr3 = arena_alloc(sub_arena2, 32);
    assert(ptr3 != NULL);
    assert(ptr3 > ptr2);

    // Cleanup
    sub_arena_destroy(sub_arena2);
    arena_destroy(parent_arena);
    printf("✓ Sub-arena memory isolation tests passed!\n");
}

void
test_arena_pop() {
    struct ORArena *arena = arena_create(1024);
    assert(arena != NULL);

    // Allocate some memory and record the state
    const void *ptr1 = arena_alloc(arena, 100);
    assert(ptr1 != NULL);

    // Save the current size before second allocation
    arena->previous_size = arena->used_size;

    // Allocate more memory
    const void *ptr2 = arena_alloc(arena, 200);
    assert(ptr2 != NULL);
    const size_t size_before_pop = arena->used_size;

    // Pop back to first allocation
    arena_pop(arena);
    assert(arena->used_size == arena->previous_size);
    assert(arena->used_size < size_before_pop);

    // Verify we can allocate again in the popped space
    const void *ptr3 = arena_alloc(arena, 150);
    assert(ptr3 != NULL);
    assert(ptr3 >= ptr2); // Should reuse the previously popped space

    arena_destroy(arena);
    printf("✓ Arena pop tests passed!\n");
}

void
test_arena_reset() {
    struct ORArena *arena = arena_create(1024);
    assert(arena != NULL);

    // Make several allocations
    const void *ptr1 = arena_alloc(arena, 100);
    const void *ptr2 = arena_alloc(arena, 200);
    const void *ptr3 = arena_alloc(arena, 300);
    assert(ptr1 != NULL && ptr2 != NULL && ptr3 != NULL);

    const size_t used_before_reset = arena->used_size;
    assert(used_before_reset > 0);

    // Reset the arena
    arena_reset(arena);
    assert(arena->used_size == 0);
    assert(arena->total_size == 1024); // Total size should remain unchanged

    // Verify we can allocate again
    const void *ptr4 = arena_alloc(arena, used_before_reset);
    assert(ptr4 != NULL);
    assert(ptr4 == ptr1); // Should start from the beginning of the arena

    arena_destroy(arena);
    printf("✓ Arena reset tests passed!\n");
}

void
test_sub_arena_pop_protection() {
    struct ORArena *parent_arena = arena_create(1024);
    assert(parent_arena != NULL);
    // Record the previous size before sub-arena creation
    parent_arena->previous_size = parent_arena->used_size;
    // Create a sub-arena
    struct ORArena *sub_arena = sub_arena_create(parent_arena, 256);
    assert(sub_arena != NULL);
    // Try to pop parent arena
    arena_pop(parent_arena);
    // Verify that the sub-arena memory is still intact
    assert(parent_arena->used_size == parent_arena->previous_size);
    // Verify sub-arena is still usable
    void *ptr = arena_alloc(sub_arena, 64);
    assert(ptr != NULL);

    sub_arena_destroy(sub_arena);
    arena_destroy(parent_arena);
    printf("✓ Sub-arena pop protection tests passed!\n");
}

// Main test runner
int
main() {
    printf("Running arena creation tests...\n");
    test_valid_creation();
    test_valid_shared_creation();
    test_zero_size();
    test_minimum_size();
    test_arena_pop();
    test_arena_reset();
    printf("Running arena allocation tests...\n");
    test_arena_basic_allocation();
    test_arena_aligned_allocation();
    test_arena_boundaries();
    printf("Running sub-arena tests...\n");
    test_sub_arena_creation();
    test_sub_arena_memory_isolation();
    test_sub_arena_pop_protection();

    printf("All tests passed!\n");
    return 0;
}