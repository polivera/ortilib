//
// Created by pablo on 11/28/24.
//

#include <stdio.h>
#include "../orwindow_internal.h"
#include "wayland_client.h"
#include "wayland_decorator.h"

struct InterWaylandClient wlclient;

static void
inter_create_process(const char *process_name) {
    printf("create process for %s\n", process_name);
}

// TODO: Here it will split into wayland and X11 (will not be implemented for the moment) and the
// decoration will be handle inside wayland since they are tightly coupled.
// The process stuff if needed will be handle here as well
enum ORWindowError
inter_create_window(const char *window_name, const char *process_name) {
    inter_create_process(process_name);
    // TODO: Check here if wayland or x11
    wlclient = inter_get_wayland_client(window_name);

    return OR_NO_ERROR;
}

enum ORWindowError
inter_surface_setup(struct ORBitmap *bmp, struct InterListeners *listeners) {
    // TODO: Check here if wayland or X11
    if (!wlclient.wayland || !wlclient.libdecor) {
        fprintf(stderr, "Failed to create wayland client\n");
        return OR_WAYLAND_CLIENT_INIT_FAILED;
    }
    wlclient.listeners = listeners;
    return inter_wl_window_setup(bmp, &wlclient);
}

enum ORWindowError
inter_start_drawing() {
    // TODO: Check here if wayland or X11
    return inter_wl_start_drawing(&wlclient);
}

void
inter_remove_window() {
    // TODO: Check if wayland or X11
    inter_wl_destroy_libdecor(&wlclient);
    inter_wl_free_window(&wlclient);
}

