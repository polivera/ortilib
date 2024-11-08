#include "../window.h"
#include "./xdg-shell-protocol.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <wayland-client-protocol.h>

extern void resize_window(struct orti_bitmap *bitmap);
extern void make_frame(struct orti_bitmap bitmap);
extern void update_bitmap_size_data(struct orti_bitmap *bitmap,
                                    int16_t new_width, int16_t new_height);

void handle_surface_configuration(void *data, struct xdg_surface *surface,
                                  uint32_t serial) {
  xdg_surface_ack_configure(surface, serial);
  struct orti_bitmap *bitmap = (struct orti_bitmap *)data;
  if (NULL == bitmap->mem) {
    resize_window(bitmap);
  }
  make_frame(*bitmap);
}

/**
 * This will handle the window resize
 */
void handle_toplevel_config(void *data, struct xdg_toplevel *toplevel,
                            int32_t new_width, int32_t new_height,
                            struct wl_array *states) {
  printf("handle toplevel config\n");
  if (!new_width || !new_height) {
    return;
  }
  struct orti_bitmap *bitmap = (struct orti_bitmap *)data;
  if (new_width != bitmap->width || new_height != bitmap->height) {
    munmap(bitmap->mem, bitmap->mem_size);
    update_bitmap_size_data(bitmap, new_width, new_height);
    resize_window(bitmap);
  }
}

void handle_toplevel_close(void *data, struct xdg_toplevel *top_level) {
  struct orti_bitmap *bitmap = (struct orti_bitmap *)data;
  bitmap->is_window_open = 0;
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

void xdg_setup(struct orti_bitmap *bitmap, struct wl_surface *wayland_surface,
               const char *window_name) {
  xdg_surface = xdg_wm_base_get_xdg_surface(xdg_base, wayland_surface);
  xdg_surface_add_listener(xdg_surface, &surface_listener, bitmap);

  xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
  xdg_toplevel_add_listener(xdg_toplevel, &toplevel_listener, bitmap);
  xdg_toplevel_set_title(xdg_toplevel, window_name);

  xdg_toplevel_set_app_id(xdg_toplevel, "some-id");
}

void xdg_destroy() {
  xdg_toplevel_destroy(xdg_toplevel);
  xdg_surface_destroy(xdg_surface);
}
