//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"
#include "orwindow_internal.h"

struct ORBitmap bitmap;
struct InterListeners listeners;

struct ORBitmap
or_bitmap_create(const uint16_t width, const uint16_t height) {
    struct ORBitmap bmp = {
        .width = width,
        .height = height,
    };
    or_bitmap_reset(&bmp, width, height);
    return bmp;
}

void
or_bitmap_reset(struct ORBitmap *bmp, const uint16_t width, const uint16_t height) {
    bmp->mem = NULL;
    bmp->width = width;
    bmp->height = height;
    bmp->mem_size = width * height * 4;
    bmp->stride = width * 4;
}

enum ORWindowError
or_create_window(const uint16_t width, const uint16_t height, const char *window_name, const char *process_name) {
    bitmap = or_bitmap_create(width, height);
    return inter_create_window(window_name, process_name);
}

enum ORWindowError
or_surface_setup(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners) {
    listeners.window_listeners = window_listeners;
    listeners.keyboard_listeners = keyboard_listeners;
    listeners.pointer_listeners = pointer_listeners;
    return inter_surface_setup(&bitmap, &listeners);
}

enum ORWindowError
or_start_main_loop() {
    return inter_start_drawing();
}

void
or_destroy_window() {
    return inter_remove_window();
}
