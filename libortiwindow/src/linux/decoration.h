#ifndef __ORTI_WINDOW_DECORATION__
#define __ORTI_WINDOW_DECORATION__

#include <libdecor-0/libdecor.h>
#include <wayland-client-core.h>

struct wl_display;
struct wl_surface;
struct orti_bitmap;

struct orti_decoration {
  struct libdecor *base;
  struct libdecor_frame *frame;
};

void decoration_setup(struct orti_decoration *decoration,
                      struct orti_bitmap *bitmap,
                      struct wl_display *wayland_display,
                      struct wl_surface *wayland_surface,
                      const char *window_name);

void decoration_clean(struct orti_decoration decoration);

#endif
