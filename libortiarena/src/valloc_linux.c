#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void *internal_alloc(size_t size_in_bytes) {
  return mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE,
              MAP_NORESERVE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

int virtual_free(void *valloc, size_t size_bytes) {
  return munmap(valloc, size_bytes);
}

// TODO: Review MAP_HUGETLB for large amount of memories (linux 2.6.32)
void *virtual_alloc(size_t size_in_bytes) {
  void *addr = internal_alloc(size_in_bytes);
  if (addr == MAP_FAILED) {
    perror("mmap failed");
    return NULL;
  }

  return addr;
}

void *virtual_realloc(size_t new_size, void *old_addr, size_t old_size) {
  void *new_addr = internal_alloc(new_size);
  if (new_addr == MAP_FAILED) {
    perror("reallocation failed");
    return NULL;
  }
  if (old_addr && old_size > 0) {
    memcpy(new_addr, old_addr, (old_size < new_size) ? old_size : new_size);
    virtual_free(old_addr, old_size);
  }
  return new_addr;
}
