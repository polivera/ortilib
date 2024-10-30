#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int virtual_free(void *valloc, uint64_t size_bytes) {
  return munmap(valloc, size_bytes);
}

void *virtual_alloc(uint64_t size_in_bytes) {
  void *addr = mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE,
                    MAP_NORESERVE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    perror("mmap failed");
    return NULL;
  }

  return addr;
}
