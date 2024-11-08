#ifndef __ORTI_WINDOW_XDG_SHELL__
#define __ORTI_WINDOW_XDG_SHELL__

#include "./xdg-shell-protocol.h"
#include <stdint.h>

void xdg_setup(struct orti_bitmap *bitmap, struct wl_surface *wayland_surface,
               const char *window_name);
void xdg_destroy();

// Shared global variables
extern struct xdg_wm_base *xdg_base;
extern struct xdg_surface *xdg_surface;
extern struct xdg_toplevel *xdg_toplevel;

// Listener declarations
extern struct xdg_surface_listener surface_listener;
extern struct xdg_toplevel_listener toplevel_listener;
extern struct xdg_wm_base_listener xdg_shell_listener;

#endif
