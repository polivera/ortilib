//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"
#include "orwindow_internal.h"

struct ORBitmap;

void
or_create_window(uint16_t width, uint16_t height, const char *window_name, const char *process_name) {
    struct ORBitmap bitmap = inter_create_bitmap(width, height);
    inter_create_window(&bitmap, "my great window", "such.a.bad.impl");
    // create client
    printf("bitmap created with size %u x %u\n", bitmap.width, bitmap.height);
}

void
or_add_listeners(struct ORWindowListeners *window_listeners,
                 struct ORKeyboardListeners *keyboard_listeners,
                 struct ORPointerListeners *pointer_listeners) {

}