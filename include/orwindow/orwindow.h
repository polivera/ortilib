//
// Created by pablo on 11/27/24.
//

#ifndef ORTILIB_ORWINDOW_H
#define ORTILIB_ORWINDOW_H

#include <stdint.h>
#include "orwindow_keyboard.h"
#include "orwindow_pointer.h"
#include "orwindow_errors.h"

struct ORBitmap {
  uint16_t width;
  uint16_t height;
  uint32_t mem_size;
  uint16_t stride;
  uint8_t *mem;
};

struct ORWindowListeners {
  void (*draw)(struct ORBitmap bitmap);
  void (*enter)();
  void (*leave)();
  void (*fullscreen)();
  void (*close)();
};

void
or_create_window(uint16_t width, uint16_t height, const char *window_name, const char *process_name);

void
or_add_listeners(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners);

void
or_start_main_loop();

void
or_toggle_fullscreen();

void
or_destroy_window();

#endif //ORTILIB_ORWINDOW_H
