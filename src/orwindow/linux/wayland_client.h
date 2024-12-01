//
// Created by pablo on 11/29/24.
//

#ifndef ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H
#define ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H

#include <wayland-client-protocol.h>

struct InterWaylandClient {
  struct ORBitmap *bitmap;
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_surface *surface;
  struct wl_buffer *buffer;
  struct wl_shm *shared_memory;
  struct wl_seat *seat;
  struct wl_keyboard *keyboard;
  struct wl_pointer *pointer;
  struct InterListeners *listeners;
};

enum ORWindowError
inter_window_setup(struct InterWaylandClient *wlclient, struct ORBitmap *bitmap);

int8_t
orwl_set_listeners(struct InterListeners *listeners);

#endif //ORTILIB_SRC_ORWINDOW_LINUX_WAYLAND_CLIENT_H
