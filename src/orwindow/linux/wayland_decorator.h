//
// Created by pablo on 12/1/24.
//

#ifndef ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H
#define ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H

#include <libdecor-0/libdecor.h>
#include <stdbool.h>

struct InterDecoration {
    struct libdecor *base;
    struct libdecor_frame *frame;
    const char *name;
    bool is_open;
    bool is_fullscreen;
};

enum ORWindowError init_libdecor(struct InterWaylandClient *wlclient, const char *window_name);

void inter_wl_start_decoration(const struct InterWaylandClient *wlclient);

void inter_wl_destroy_libdecor(struct InterWaylandClient *wlclient);
#endif //ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_DECORATOR_H
