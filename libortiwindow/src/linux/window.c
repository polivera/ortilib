#include "xdg-shell-protocol.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

#include <fcntl.h>
#include <wayland-util.h>

// The compositor lives in the server but we need to access it to create a
// surface.
struct wl_compositor *compositor;

// The surface its the object htat represent the actual pixel data.
struct wl_surface *surface;

// Every window has its own offscreen buffer somewhere in memory.
// Waylan read that buffer and composite it into an image.
// To create a buffer you have to allocate shared memory.
// Buffer represents the memory but is not the actual pointer to the memory.
// We need a pixel for that
struct wl_buffer *buffer;

// Shared memory for the buffer
struct wl_shm *shm;

struct xdg_wm_base *xdg_shell;
// This is what we consider the window
struct xdg_toplevel *xdg_top_level;

/**
 * INPUTS
 */
struct wl_seat *seat;
struct wl_keyboard *keyboard;

// RGBA pixel
uint8_t *pixel;

uint16_t window_width = 1024;
uint16_t window_heigh = 768;
uint8_t color;
uint8_t close_window;

/**
 * Allocate shared memory space
 * TODO: Can I send an arena to this allocator since its goint to be called
 * on window resize?
 */
int32_t allocate_shared_memory(uint64_t size) {
  const char *name = "orti-shimp";
  int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL,
                        S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
  shm_unlink(name);
  ftruncate(fd, size);
  return fd;
}

/**
 * This should be call every time the window is resize
 * TODO: Again we have a virtual allocation that might be replace with arena
 */
void resize_window() {
  printf("Resize the window! %d\n", color);
  int32_t fd = allocate_shared_memory(window_width * window_heigh * 4);
  pixel = mmap(0, window_width * window_heigh * 4, PROT_READ | PROT_WRITE,
               MAP_SHARED, fd, 0);
  if (NULL == pixel) {
    printf("cannot allocate pixel");
    return;
  }
  struct wl_shm_pool *pool =
      wl_shm_create_pool(shm, fd, window_width * window_heigh * 4);
  // NOTE: Here is the stride
  buffer = wl_shm_pool_create_buffer(pool, 0, window_width, window_heigh,
                                     window_width * 4, WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);
}

void draw() {
  color++;
  memset(pixel, color, window_width * window_heigh * 4);
}

void make_frame() {
  draw();
  wl_surface_attach(surface, buffer, 0, 0);
  wl_surface_damage_buffer(surface, 0, 0, window_width, window_heigh);
  wl_surface_commit(surface);
}

/**
 * Callback weird stuff
 */
struct wl_callback_listener cb_listener;
void frame_new(void *data, struct wl_callback *cb, uint32_t cb_data) {
  wl_callback_destroy(cb);
  cb = wl_surface_frame(surface);
  wl_callback_add_listener(cb, &cb_listener, 0);
  make_frame();
}
struct wl_callback_listener cb_listener = {.done = frame_new};

void x_surface_config(void *data, struct xdg_surface *x_surface,
                      uint32_t serial) {
  xdg_surface_ack_configure(x_surface, serial);
  printf("XDG configuration \n");
  if (!pixel) {
    resize_window();
  }
  make_frame();
}

struct xdg_surface_listener x_listener = {.configure = x_surface_config};

void top_config(void *data, struct xdg_toplevel *top_level, int32_t new_width,
                int32_t new_height, struct wl_array *states) {
  if (!new_width || !new_height) {
    return;
  }

  if (window_width != new_width || window_heigh != new_height) {
    munmap(pixel, new_width * new_height);
    window_width = new_width;
    window_heigh = new_height;
    resize_window();
  }
}

void top_close(void *data, struct xdg_toplevel *top_level) { close_window = 1; }

struct xdg_toplevel_listener toplevel_listener = {.configure = top_config,
                                                  .close = top_close};

void shell_ping(void *data, struct xdg_wm_base *sh, uint32_t serial) {
  xdg_wm_base_pong(sh, serial);
}

struct xdg_wm_base_listener xdg_shell_listener = {.ping = shell_ping};

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
    compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    xdg_shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(xdg_shell, &xdg_shell_listener, 0);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    wl_seat_add_listener(seat, &seat_listener, 0);
  }
}

/**
 * This is call on remove
 */
void register_global_remove(void *data, struct wl_registry *registry,
                            uint32_t name) {}

struct wl_registry_listener registry_listener = {
    .global = register_global,
    .global_remove = register_global_remove,
};

void orti_open_window() {
  /**
   * First we need a display and a registry.
   * The `wl_display_connect` will look for a server that can connect to
   * Then we will create the registry that will act as an "inbox" of every
   * event of the screen
   */
  struct wl_display *display = wl_display_connect(0);
  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, 0);
  // Sync client and server. It sends all pending request to the compositor
  wl_display_roundtrip(display);

  surface = wl_compositor_create_surface(compositor);
  struct wl_callback *callback = wl_surface_frame(surface);
  wl_callback_add_listener(callback, &cb_listener, 0);

  struct xdg_surface *x_surface =
      xdg_wm_base_get_xdg_surface(xdg_shell, surface);
  xdg_surface_add_listener(x_surface, &x_listener, 0);

  xdg_top_level = xdg_surface_get_toplevel(x_surface);
  xdg_toplevel_add_listener(xdg_top_level, &toplevel_listener, 0);
  xdg_toplevel_set_title(xdg_top_level, "some weird title");
  wl_surface_commit(surface);

  while (wl_display_dispatch((display))) {
    if (close_window)
      break;
  }

  wl_seat_release(seat);
  if (buffer) {
    wl_buffer_destroy(buffer);
  }
  xdg_toplevel_destroy(xdg_top_level);
  xdg_surface_destroy(x_surface);
  wl_surface_destroy(surface);
  wl_display_disconnect(display);
}
