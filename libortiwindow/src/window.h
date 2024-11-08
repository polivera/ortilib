#ifndef __ORTI_WINDOW_WINDOW__
#define __ORTI_WINDOW_WINDOW__

#include <stdint.h>

struct orti_bitmap {
  uint16_t width;
  uint16_t height;
  uint32_t mem_size;
  uint16_t stride;
  uint8_t *mem;

  uint8_t is_window_open;
};

struct orti_bitmap new_orti_window(uint16_t width, uint16_t height);
void orti_open_window(struct orti_bitmap *bitmap);

#endif
