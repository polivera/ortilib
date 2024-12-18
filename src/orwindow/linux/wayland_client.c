//
// Created by pablo on 11/29/24.
//

#include <fcntl.h> // shm_open flags
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // shm_open
#include <unistd.h>   // ftruncate

#include "orwindow/orwindow.h"
#include "orwindow/orwindow_errors.h"
#include "orwindow_internal.h"
#include "wayland_client.h"
#include "wayland_decorator.h"
#include "wayland_keyboard.h"
#include "wayland_pointer.h"

#define WAYLAND_ENV_VAR "WAYLAND_DISPLAY"
#define FILE_DESCRIPTOR_NAME "/or-shared-mem"
#define SEAT_VERSION 3

// Registers devices from the Wayland registry by binding interfaces.
// This will only run at window startup.
static void register_device(void *data, struct wl_registry *registry,
                            uint32_t name, const char *interface,
                            uint32_t version) {
    struct InterWaylandClient *client = data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        client->wayland->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        client->wayland->shared_memory =
            wl_registry_bind(registry, name, &wl_shm_interface, version);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        client->wayland->seat =
            wl_registry_bind(registry, name, &wl_seat_interface, SEAT_VERSION);
        if (client->listeners) {
            if (client->listeners->keyboard_listeners) {
                setup_keyboard(client->listeners->window_listeners,
                               client->listeners->keyboard_listeners,
                               client->wayland);
            }
            if (client->listeners->pointer_listeners) {
                setup_pointer(client->listeners->pointer_listeners,
                              client->wayland);
            }
        }
    }
}

// Do something when devices are unplugged?
static void unregister_device(void *data, struct wl_registry *registry,
                              uint32_t name) {
    printf("Something has been unplugged?\n");
}

// Listener for Wayland registry events
struct wl_registry_listener registry_listener = {
    .global = register_device,
    .global_remove = unregister_device,
};

void inter_draw(const struct ORBitmap *bitmap,
                const struct ORWindowListeners *window_listeners) {
    if (window_listeners->draw) {
        window_listeners->draw(bitmap);
        return;
    }
    memset(bitmap->mem, 250, bitmap->mem_size);
}

// Renders a frame on the Wayland client
void inter_frame_render(const struct InterWaylandClient *wlclient) {
    inter_draw(wlclient->bitmap, wlclient->listeners->window_listeners);
    wl_surface_attach(wlclient->wayland->surface, wlclient->wayland->buffer, 0,
                      0);
    wl_surface_damage_buffer(wlclient->wayland->surface, 0, 0,
                             wlclient->bitmap->width, wlclient->bitmap->height);
    wl_surface_commit(wlclient->wayland->surface);
}

// Listener for Wayland frame callbacks
// Called when a new frame is ready to be drawn
struct wl_callback_listener callback_listener;

void wl_frame_new(void *data, struct wl_callback *cb, uint32_t cb_data) {
    struct InterWaylandClient *wlclient = data;
    wl_callback_destroy(cb);
    cb = wl_surface_frame(wlclient->wayland->surface);
    wl_callback_add_listener(cb, &callback_listener, wlclient);
    inter_frame_render(wlclient);
}

struct wl_callback_listener callback_listener = {.done = wl_frame_new};

// Initialize wayland related variables
static enum ORWindowError init_wayland(struct InterWaylandClient *wlclient) {
    wlclient->wayland->buffer = NULL;
    // Attempts to connect to the Wayland display using an environment variable
    // or fallback to default server.
    wlclient->wayland->display = wl_display_connect(getenv(WAYLAND_ENV_VAR));
    if (!wlclient->wayland->display) {
        wlclient->wayland->display = wl_display_connect(NULL);
        if (!wlclient->wayland->display) {
            fprintf(stderr, "Failed to connect to wayland display server\n");
            return OR_DISPLAY_INIT_ERROR;
        }
    }
    // Retrieves the registry and adds the listener
    wlclient->wayland->registry =
        wl_display_get_registry(wlclient->wayland->display);
    if (!wlclient->wayland->registry) {
        fprintf(stderr, "Cannot initialize display registry\n");
        return OR_WAYLAND_SURFACE_INIT_ERROR;
    }
    wl_registry_add_listener(wlclient->wayland->registry, &registry_listener,
                             wlclient);
    // This trigger the registration process.
    wl_display_roundtrip(wlclient->wayland->display);

    // The registry callback is what sets the compositor variable
    wlclient->wayland->surface =
        wl_compositor_create_surface(wlclient->wayland->compositor);
    struct wl_callback *callback = wl_surface_frame(wlclient->wayland->surface);
    wl_callback_add_listener(callback, &callback_listener, wlclient);
    return OR_NO_ERROR;
}

// Create a wayland client struct
struct InterWaylandClient inter_get_wayland_client(const char *window_name) {
    // TODO: Maybe use arenas here?
    struct InterWaylandClient client = {0};
    client.wayland = malloc(sizeof(struct InterWayland));
    if (client.wayland == NULL) {
        return client;
    }
    client.libdecor = malloc(sizeof(struct InterDecoration));
    if (client.libdecor == NULL) {
        free(client.wayland);
        return client;
    }

    client.libdecor->name = malloc(sizeof(char) * strlen(window_name));
    if (client.libdecor->name == NULL) {
        client.libdecor->name = "some name";
    } else {
        strcpy(client.libdecor->name, window_name);
    }
    // client.libdecor->name = window_name;
    return client;
}

// This is called from the decorator upon window resize
enum ORWindowError
inter_wl_window_resize(const struct InterWaylandClient *wlclient) {
    const int32_t file_descriptor =
        shm_open(FILE_DESCRIPTOR_NAME, O_RDWR | O_CREAT | O_EXCL,
                 S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
    shm_unlink(FILE_DESCRIPTOR_NAME);
    ftruncate(file_descriptor, wlclient->bitmap->mem_size);
    wlclient->bitmap->mem =
        mmap(0, wlclient->bitmap->mem_size, PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_NORESERVE, file_descriptor, 0);
    if (wlclient->bitmap->mem == NULL) {
        fprintf(stderr, "Failed to mmap bitmap\n");
        return OR_WAYLAND_FAILED_WINDOW_SIZING;
    }
    struct wl_shm_pool *pool =
        wl_shm_create_pool(wlclient->wayland->shared_memory, file_descriptor,
                           wlclient->bitmap->mem_size);
    if (pool == NULL) {
        fprintf(stderr, "Failed to create shared memory pool\n");
        return OR_WAYLAND_FAILED_WINDOW_SIZING;
    }
    if (wlclient->wayland->buffer != NULL) {
        wl_buffer_destroy(wlclient->wayland->buffer);
        wlclient->wayland->buffer = NULL;
    }
    wlclient->wayland->buffer = wl_shm_pool_create_buffer(
        pool, 0, wlclient->bitmap->width, wlclient->bitmap->height,
        wlclient->bitmap->stride, WL_SHM_FORMAT_ARGB8888);
    if (wlclient->wayland->buffer == NULL) {
        fprintf(stderr, "Failed to create shared memory buffer\n");
        return OR_WAYLAND_FAILED_WINDOW_SIZING;
    }
    wl_shm_pool_destroy(pool);
    close(file_descriptor);
    return OR_NO_ERROR;
}

// Setup wayland window client
enum ORWindowError inter_wl_window_setup(struct ORBitmap *bmp,
                                         struct InterWaylandClient *wlclient) {
    if (wlclient == NULL) {
        return OR_DISPLAY_INIT_ERROR;
    }
    wlclient->bitmap = bmp;
    const enum ORWindowError error = init_wayland(wlclient);
    if (error != OR_NO_ERROR) {
        inter_wl_free_window(wlclient);
        return error;
    }
    return init_libdecor(wlclient);
}

enum ORWindowError inter_wl_start_drawing(struct InterWaylandClient *wlclient) {
    if (wlclient == NULL) {
        return OR_DISPLAY_INIT_ERROR;
    }
    inter_wl_start_decoration(wlclient->libdecor);
    while (wlclient->libdecor->is_open) {
        if (wl_display_dispatch(wlclient->wayland->display) < 0) {
            perror("wl_display_dispatch error\n");
            break;
        }
    }
    return OR_NO_ERROR;
}

void inter_wl_free_window(struct InterWaylandClient *wlclient) {
    if (wlclient != NULL && wlclient->wayland != NULL) {
        if (wlclient->wayland->buffer) {
            wl_buffer_destroy(wlclient->wayland->buffer);
            wlclient->wayland->buffer = NULL;
        }
        if (wlclient->wayland->surface) {
            wl_surface_destroy(wlclient->wayland->surface);
            wlclient->wayland->surface = NULL;
        }
        if (wlclient->wayland->display) {
            wl_display_disconnect(wlclient->wayland->display);
            wlclient->wayland->display = NULL;
        }
        wlclient->wayland = NULL;
    }
}
