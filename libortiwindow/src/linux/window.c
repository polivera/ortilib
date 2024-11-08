#include "../window.h"
#include "decoration.h"
#include <fcntl.h>
#include <libdecor-0/libdecor.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

struct wl_compositor *compositor;
struct wl_surface *surface;
struct wl_buffer *buffer;
struct wl_shm *shm;

/**
 * INPUTS
 */
struct wl_seat *seat;
struct wl_keyboard *keyboard;

/**
 * Allocate shared memory space
 * TODO: Can I send an arena to this allocator since its goint to be called
 * on window resize?
 */
int32_t allocate_shared_memory(uint32_t size) {
  const char *name = "orti-shimp";
  int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL,
                        S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
  shm_unlink(name);
  ftruncate(fd, size);
  return fd;
}

/**
 * This should be call every time the window is resize
 */
void resize_window(struct orti_bitmap *bitmap) {
  int32_t fd = allocate_shared_memory(bitmap->mem_size);
  bitmap->mem =
      mmap(0, bitmap->mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (NULL == bitmap->mem) {
    printf("cannot allocate pixel");
    return;
  }
  struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, bitmap->mem_size);

  if (buffer != NULL) {
    wl_buffer_destroy(buffer);
    buffer = NULL;
  }
  buffer = wl_shm_pool_create_buffer(pool, 0, bitmap->width, bitmap->height,
                                     bitmap->stride, WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);
}

void draw_weird_shit(struct orti_bitmap bitmap, uint32_t x_offset,
                     uint32_t y_offset) {
  uint8_t *row = bitmap.mem;
  for (uint16_t y = 0; y < bitmap.height; y++) {
    uint8_t *pixel = row;
    for (uint16_t x = 0; x < bitmap.width; x++) {
      *pixel = (x + x_offset);
      pixel++;

      *pixel = (y + y_offset);
      pixel++;

      *pixel = 100;
      pixel++;

      *pixel = 255;
      pixel++;
    }
    row += bitmap.stride;
  }
}

uint16_t my_x_offset = 0;
void draw(struct orti_bitmap bitmap) {
  draw_weird_shit(bitmap, my_x_offset, my_x_offset * 2);
  ++my_x_offset;
}

void make_frame(struct orti_bitmap bitmap) {
  draw(bitmap);
  wl_surface_attach(surface, buffer, 0, 0);
  wl_surface_damage_buffer(surface, 0, 0, bitmap.width, bitmap.height);
  wl_surface_commit(surface);
}

/**
 * Callback weird stuff
 */
struct wl_callback_listener cb_listener;
void frame_new(void *data, struct wl_callback *cb, uint32_t cb_data) {
  struct orti_bitmap *bitmap = data;
  wl_callback_destroy(cb);
  cb = wl_surface_frame(surface);
  wl_callback_add_listener(cb, &cb_listener, bitmap);
  make_frame(*bitmap);
}
struct wl_callback_listener cb_listener = {.done = frame_new};

void seat_capabilities(void *data, struct wl_seat *wl_seat,
                       uint32_t capabilities) {

  // TODO: Add key listener wl_keyboard_listener.
}
void seat_name(void *data, struct wl_seat *wl_seat, const char *name) {}
struct wl_seat_listener seat_listener = {.capabilities = seat_capabilities,
                                         .name = seat_name};

/**
 * Global listener function.
 */
void register_global(void *data, struct wl_registry *registry, uint32_t name,
                     const char *interface, uint32_t version) {
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    compositor =
        wl_registry_bind(registry, name, &wl_compositor_interface, version);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    shm = wl_registry_bind(registry, name, &wl_shm_interface, version);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
    wl_seat_add_listener(seat, &seat_listener, 0);
  }
}

void register_global_remove(void *data, struct wl_registry *registry,
                            uint32_t name) {
  printf("global remove this stuff\n");
}

struct wl_registry_listener registry_listener = {
    .global = register_global,
    .global_remove = register_global_remove,
};

void update_bitmap_size_data(struct orti_bitmap *bitmap, int16_t new_width,
                             int16_t new_height) {
  bitmap->mem = NULL;
  bitmap->width = new_width;
  bitmap->height = new_height;
  bitmap->mem_size = new_width * new_height * 4;
  bitmap->stride = new_width * 4;
}

struct orti_bitmap new_orti_window(uint16_t width, uint16_t height) {
  struct orti_bitmap bitmap = {
      .is_window_open = 0,
  };
  update_bitmap_size_data(&bitmap, width, height);
  return bitmap;
}

void orti_open_window(struct orti_bitmap *bitmap) {
  struct wl_display *display = wl_display_connect(0);
  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, 0);
  wl_display_roundtrip(display);

  surface = wl_compositor_create_surface(compositor);
  struct wl_callback *callback = wl_surface_frame(surface);
  wl_callback_add_listener(callback, &cb_listener, bitmap);

  struct orti_decoration deco_context;
  decoration_setup(&deco_context, bitmap, display, surface,
                   "My application name");

  bitmap->is_window_open = 1;
  while (wl_display_dispatch((display))) {
    if (bitmap->is_window_open != 1)
      break;
  }

  wl_seat_release(seat);
  if (buffer) {
    wl_buffer_destroy(buffer);
  }

  decoration_clean(deco_context);
  wl_surface_destroy(surface);
  wl_display_disconnect(display);
}
