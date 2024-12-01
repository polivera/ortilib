//
// Created by pablo on 11/28/24.
//

#include <stdio.h>
#include "../orwindow_internal.h"
#include "wayland_client.h"

struct InterWaylandClient wl_client;

internal void
inter_create_process(const char *process_name) {
    printf("create process for %s\n", process_name);
}

struct ORBitmap
inter_create_bitmap(uint16_t width, uint16_t height) {
    struct ORBitmap bitmap = {
        .width = width,
        .height = height,
    };
    return bitmap;
}

// TODO: Here it will split into wayland and X11 (will not be implemented for the moment) and the
// decoration will be handle inside wayland since they are tightly coupled.
// The process stuff if needed will be handle here as well
enum ORWindowError
inter_create_window(struct ORBitmap *bitmap, const char *window_name, const char *process_name) {
    inter_create_process(process_name);
    // TODO: Check here if wayland or x11
    inter_wl_window_setup(&wl_client, bitmap, window_name);

    return 0;
}

enum ORWindowError
inter_add_listeners(struct InterListeners *listeners) {
    // TODO: Check here if wayland or X11
    return inter_wl_set_listeners(&wl_client, listeners);
}

