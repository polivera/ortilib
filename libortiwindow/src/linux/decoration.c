#include "decoration.h"
#include "../window.h"
#include <libdecor-0/libdecor.h>
#include <stdio.h>
#include <sys/mman.h>

extern void resize_window(struct orti_bitmap *bitmap);
extern void make_frame(struct orti_bitmap bitmap);
extern void update_bitmap_size_data(struct orti_bitmap *bitmap,
                                    int16_t new_width, int16_t new_height);

void commit_frame(struct libdecor_frame *frame,
                  struct libdecor_configuration *configuration,
                  struct orti_bitmap *data) {
  // NOTE: This code actually create the border
  struct libdecor_state *state = libdecor_state_new(data->width, data->height);
  libdecor_frame_commit(frame, state, configuration);
  libdecor_state_free(state);
}

void decoration_config(struct libdecor_frame *frame,
                       struct libdecor_configuration *configuration,
                       void *data) {
  struct orti_bitmap *bitmap = (struct orti_bitmap *)data;
  int width, height;
  if (libdecor_configuration_get_content_size(configuration, frame, &width,
                                              &height)) {
    munmap(bitmap->mem, bitmap->mem_size);
    update_bitmap_size_data(bitmap, width, height);
    resize_window(bitmap);
  }
  if (NULL == bitmap->mem) {
    resize_window(bitmap);
    make_frame(*bitmap);
  }
  commit_frame(frame, configuration, data);
}

void decoration_commit(struct libdecor_frame *frame, void *data) {
  printf("Decoration commit\n");
}

void decoration_close(struct libdecor_frame *frame, void *data) {
  struct orti_bitmap *bitmap = (struct orti_bitmap *)data;
  bitmap->is_window_open = 0;
}

struct libdecor_frame_interface frame_interface = {
    .configure = decoration_config,
    .commit = decoration_commit,
    .close = decoration_close,
};

void decoration_setup(struct orti_decoration *decoration,
                      struct orti_bitmap *bitmap,
                      struct wl_display *wayland_display,
                      struct wl_surface *wayland_surface,
                      const char *window_name) {
  decoration->base = libdecor_new(wayland_display, NULL);
  if (decoration->base == NULL) {
    printf("cannot create decoration->context");
    return;
  }
  decoration->frame = libdecor_decorate(decoration->base, wayland_surface,
                                        &frame_interface, bitmap);
  if (decoration->frame == NULL) {
    printf("cannot decorate window");
    return;
  }

  libdecor_frame_set_title(decoration->frame, window_name);
  libdecor_frame_map(decoration->frame);
}

void decoration_clean(struct orti_decoration decoration) {
  libdecor_frame_unref(decoration.frame);
  libdecor_unref(decoration.base);
}
