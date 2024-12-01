//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"
#include "orwindow_internal.h"

struct ORBitmap bitmap;

enum ORWindowError
or_create_window(uint16_t width, uint16_t height, const char *window_name, const char *process_name) {
    bitmap = inter_create_bitmap(width, height);
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