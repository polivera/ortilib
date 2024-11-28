//
// Created by pablo on 11/28/24.
//

#include <stdio.h>
#include "../orwindow_internal.h"

struct ORBitmap
inter_create_bitmap(uint16_t width, uint16_t height) {
    struct ORBitmap bitmap = {
        .width = width,
        .height = height,
    };
    printf("Creating a bitmap in here");

    return bitmap;
}

// TODO: Here it will split into wayland and X11 (will not be implemented for the moment) and the
// decoration will be handle inside wayland since they are tightly coupled.
// The process stuff if needed will be handle here as well
int8_t
inter_create_window(struct ORBitmap *bitmap, const char *window_name, const char *process_name) {
    return 0;
}