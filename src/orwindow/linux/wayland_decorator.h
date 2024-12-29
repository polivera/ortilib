//
// Created by pablo on 12/1/24.
//

#ifndef ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H
#define ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H

#include "orwindow/orwindow.h"
#include "wayland_client.h"
#include <libdecor-0/libdecor.h>
#include <stdbool.h>

struct InterDecoration {
    struct libdecor *base;
    struct libdecor_frame *frame;
    char *name;
    bool is_open;
    bool is_fullscreen;
};

void
inter_wl_toggle_fullscreen(struct InterDecoration *libdecor_client,
                           const struct ORWindowListeners *window_listeners);

enum ORWindowError
init_libdecor(struct InterWaylandClient *wlclient);

void
inter_wl_start_decoration(struct InterDecoration *libdecor_client);

void
inter_wl_destroy_libdecor(struct InterWaylandClient *wlclient);
#endif // ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H
