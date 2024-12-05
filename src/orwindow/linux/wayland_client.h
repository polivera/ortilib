//
// Created by pablo on 11/29/24.
//

#ifndef ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H
#define ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H

#include <wayland-client-protocol.h>
#include <libdecor-0/libdecor.h>
#include "wayland_client.h"

struct InterWayland {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    struct wl_shm *shared_memory;
    struct wl_seat *seat;
    struct wl_keyboard *keyboard;
    struct wl_pointer *pointer;
};

struct InterWaylandClient {
    struct ORBitmap *bitmap;
    struct InterWayland *wayland;
    struct InterDecoration *libdecor;
    struct InterListeners *listeners;
};

struct InterWaylandClient
inter_get_wayland_client();

void
inter_frame_render(struct InterWaylandClient *wlclient);

enum ORWindowError
inter_wl_window_resize(struct InterWaylandClient *wlclient);

enum ORWindowError
inter_wl_window_setup(struct InterWaylandClient *wlclient, struct ORBitmap *bitmap, const char *window_name);

enum ORWindowError
inter_wl_set_listeners(struct InterWaylandClient *wlclient, struct InterListeners *listeners);

void
inter_wl_free_window(struct InterWaylandClient *wlclient);

enum ORWindowError
inter_wl_start_drawing(struct InterWaylandClient *wlclient);

#endif //ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H
