//
// Created by pablo on 11/27/24.
//

#ifndef ORTILIB_ORWINDOW_INTERNAL_H
#define ORTILIB_ORWINDOW_INTERNAL_H

#include "orwindow/orwindow.h"

#define internal static

struct InterListeners {
  struct ORWindowListeners *window_listeners;
  struct ORKeyboardListeners *keyboard_listeners;
  struct ORPointerListeners *pointer_listeners;
};

struct ORBitmap
inter_create_bitmap(uint16_t width, uint16_t height);

enum ORWindowError
inter_create_window(struct ORBitmap *bitmap, const char *window_name, const char *process_name);

enum ORWindowError
inter_add_listeners(struct InterListeners *listeners);

enum ORWindowError
inter_start_drawing(struct ORBitmap *bitmap);

enum ORWindowError
inter_toggle_fullscreen();

enum ORWindowError
inter_remove_window();

#endif //ORTILIB_ORWINDOW_INTERNAL_H
