#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int virtual_free(void *valloc, uint64_t size_bytes) {
  return munmap(valloc, size_bytes);
}

// TODO: Review MAP_HUGETLB for large amount of memories (linux 2.6.32)
void *virtual_alloc(uint64_t size_in_bytes) {
  void *addr = mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE,
                    MAP_NORESERVE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    perror("mmap failed");
    return NULL;
  }

  return addr;
}

/**

[TEST] - Testing method: new_arena
==3190== Warning: set address range perms: large range [0x59cbc000, 0xd9cbc000)
(defined) [TEST] - Testing method: arena_release
==3190== Warning: set address range perms: large range [0xd9cbc000, 0x159cbc000)
(defined) [TEST] - Testing method: arena_push
==3190== Warning: set address range perms: large range [0x159cbc000,
0x1d9cbc000) (defined) [TEST] - Testing method: arena_release
==3190== Warning: set address range perms: large range [0x1d9cbc000,
0x259cbc000) (defined) [TEST] - Testing method: arena_push_no_zero
==3190== Warning: set address range perms: large range [0x259cbc000,
0x2d9cbc000) (defined) [TEST] - Testing method: arena_pop
==3190== Warning: set address range perms: large range [0x2d9cbc000,
0x359cbc000) (defined) [TEST] - Testing method: arena_pop_to
==3190== Warning: set address range perms: large range [0x359cbc000,
0x3d9cbc000) (defined)

*/
