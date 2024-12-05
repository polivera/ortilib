//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"
#include "orwindow_internal.h"

struct ORBitmap bitmap;

struct ORBitmap
or_bitmap_create(const uint16_t width, const uint16_t height) {
    struct ORBitmap bitmap = {
        .width = width,
        .height = height,
    };
    or_bitmap_reset(&bitmap, width, height);
    return bitmap;
}

void
or_bitmap_reset(struct ORBitmap *bitmap, const uint16_t width, const uint16_t height) {
    bitmap->mem = NULL;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->mem_size = width * height * 4;
    bitmap->stride = width * 4;
}

enum ORWindowError
or_create_window(const uint16_t width, const uint16_t height, const char *window_name, const char *process_name) {
    bitmap = or_bitmap_create(width, height);
    return inter_create_window(&bitmap, window_name, process_name);
}

enum ORWindowError
or_add_listeners(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners) {
    struct InterListeners listeners = {
        .window_listeners = window_listeners,
        .keyboard_listeners = keyboard_listeners,
        .pointer_listeners = pointer_listeners,
    };
    return inter_add_listeners(&listeners);
}

enum ORWindowError
or_start_main_loop() {
    return inter_start_drawing();
}

void
or_destroy_window() {
    return inter_remove_window();
}
