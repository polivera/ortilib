#include "./xdg-shell-protocol.h"
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <wayland-client-protocol.h>

extern void resize_window();
extern void make_frame();

extern uint16_t window_width, window_height;
extern uint8_t *bitmap, close_window;

void handle_surface_configuration(void *data, struct xdg_surface *surface,
                                  uint32_t serial) {
  xdg_surface_ack_configure(surface, serial);
  if (NULL == bitmap) {
    resize_window();
  }
  make_frame();
}

void handle_toplevel_config(void *data, struct xdg_toplevel *toplevel,
                            int32_t new_width, int32_t new_height,
                            struct wl_array *states) {
  if (!new_width || !new_height) {
    return;
  }

  if (new_width != window_width || new_height != window_height) {
    munmap(bitmap, new_width * new_height);
    window_width = new_width;
    window_height = new_height;
    resize_window();
  }
}

void handle_toplevel_close(void *data, struct xdg_toplevel *top_level) {
  close_window = 1;
}

void handle_shell_ping(void *data, struct xdg_wm_base *sh, uint32_t serial) {
  xdg_wm_base_pong(sh, serial);
}

struct xdg_surface_listener surface_listener = {
    .configure = handle_surface_configuration};
struct xdg_toplevel_listener toplevel_listener = {
    .configure = handle_toplevel_config,
    .close = handle_toplevel_close,
};
struct xdg_wm_base_listener xdg_shell_listener = {.ping = handle_shell_ping};
struct xdg_wm_base *xdg_base;
struct xdg_toplevel *xdg_toplevel;
struct xdg_surface *xdg_surface;

void xdg_setup(struct wl_surface *wayland_surface, const char *window_name) {
  xdg_surface = xdg_wm_base_get_xdg_surface(xdg_base, wayland_surface);
  xdg_surface_add_listener(xdg_surface, &surface_listener, 0);

  xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
  xdg_toplevel_add_listener(xdg_toplevel, &toplevel_listener, 0);
  xdg_toplevel_set_title(xdg_toplevel, window_name);
}

void xdg_destroy() {
  xdg_toplevel_destroy(xdg_toplevel);
  xdg_surface_destroy(xdg_surface);
}
