//
// Created by pablo on 11/27/24.
//

#ifndef ORTILIB_ORWINDOW_H
#define ORTILIB_ORWINDOW_H

#include "orarena/orarena.h"
#include "orwindow_errors.h"
#include "orwindow_gamepad.h"
#include "orwindow_keyboard.h"
#include "orwindow_pointer.h"
#include <stdint.h>

struct ORBitmap {
    uint16_t width;
    uint16_t height;
    int32_t mem_size;
    uint16_t stride;
    uint8_t *mem;
};

struct ORWindowListeners {
    void (*draw)(const struct ORBitmap *bitmap);
    void (*enter)();
    void (*leave)();
    void (*resize)();
    void (*fullscreen)();
    void (*close)();
};

void
or_bitmap_reset(struct ORBitmap *bmp, uint16_t width, uint16_t height);

enum ORWindowError
or_create_window(uint16_t width, uint16_t height, const char *window_name,
                 const char *process_name, struct ORArena *extern_arena);

enum ORWindowError
or_surface_setup(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners,
                 struct ORGamepadListeners *gamepad_listeners);

enum ORWindowError
or_start_main_loop();

enum ORWindowError
or_toggle_fullscreen();

void
or_destroy_window();

#endif // ORTILIB_ORWINDOW_H
