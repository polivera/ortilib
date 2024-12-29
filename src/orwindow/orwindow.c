//
// Created by pablo on 11/27/24.
//

#include "orwindow/orwindow.h"
#include "orarena/orarena.h"
#include "orwindow_internal.h"

#include <stdio.h>

struct ORArena *arena;
struct ORBitmap *bitmap;
struct InterListeners listeners;

void
or_bitmap_reset(struct ORBitmap *bmp, const uint16_t width,
                const uint16_t height) {
    bmp->mem = NULL;
    bmp->width = width;
    bmp->height = height;
    bmp->mem_size = width * height * 4;
    bmp->stride = width * 4;
}

enum ORWindowError
or_create_window(const uint16_t width, const uint16_t height,
                 const char *window_name, const char *process_name,
                 struct ORArena *extern_arena) {
    arena = extern_arena;
    if (arena == NULL) {
        arena = arena_create_shared(100 * 1024 * 1024);
    }
    bitmap = arena_alloc(arena, sizeof(struct ORBitmap));
    if (bitmap == NULL) {
        fprintf(stderr, "Failed to allocate bitmap\n");
        return OR_BITMAP_CREATE_ERROR;
    }
    or_bitmap_reset(bitmap, width, height);
    return inter_create_window(window_name, process_name, arena);
}

enum ORWindowError
or_surface_setup(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners,
                 struct ORGamepadListeners *gamepad_listeners) {
    listeners.window_listeners = window_listeners;
    listeners.keyboard_listeners = keyboard_listeners;
    listeners.pointer_listeners = pointer_listeners;
    listeners.gamepad_listeners = gamepad_listeners;
    return inter_surface_setup(bitmap, &listeners);
}

enum ORWindowError
or_start_main_loop() {
    return inter_start_drawing();
}

enum ORWindowError
or_toggle_fullscreen() {
    return inter_toggle_fullscreen();
}

void
or_destroy_window() {
    return inter_remove_window(arena);
}
