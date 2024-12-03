//
// Created by pablo on 11/29/24.
//

#include <stdlib.h>
#include <stdio.h>
#include "orwindow/orwindow_errors.h"
#include "wayland_client.h"

#include <string.h>

#include "wayland_decorator.h"

#define WAYLAND_ENV_VAR "WAYLAND_DISPLAY"

static void
register_device(void *data,
                struct wl_registry *registry,
                uint32_t name,
                const char *interface,
                uint32_t version) {
    struct InterWayland *wl = data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        wl->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, version);
    }
}

struct wl_registry_listener registry_listener = {
    .global = register_device
};

/**
 * Initialize wayland related variables
 * @param wlclient
 * @return ORWindowError
 */
static enum ORWindowError init_wayland(struct InterWaylandClient *wlclient) {
    wlclient->wayland->buffer = NULL;
    wlclient->wayland->display = wl_display_connect(getenv(WAYLAND_ENV_VAR));
    if (!wlclient->wayland->display) {
        wlclient->wayland->display = wl_display_connect(NULL);
        if (!wlclient->wayland->display) {
            fprintf(stderr, "Failed to connect to wayland display server\n");
            return OR_DISPLAY_INIT_ERROR;
        }
    }
    wlclient->wayland->registry = wl_display_get_registry(wlclient->wayland->display);
    if (!wlclient->wayland->registry) {
        fprintf(stderr, "Cannot initialize display registry\n");
        return OR_WAYLAND_SURFACE_INIT_ERROR;
    }
    wl_registry_add_listener(wlclient->wayland->registry, &registry_listener, wlclient->wayland);
    // This trigger the registration process.
    wl_display_roundtrip(wlclient->wayland->display);
    /*
     * NOTE: This should always be called after the wl_registry_add_listener because the
     * global listener is the one that register de compositor
     */
    wlclient->wayland->surface = wl_compositor_create_surface(wlclient->wayland->compositor);
    struct wl_callback *callback = wl_surface_frame(wlclient->wayland->surface);
    wl_callback_add_listener(callback, NULL, wlclient);
    return OR_NO_ERROR;
}

enum ORWindowError
inter_wl_window_setup(struct InterWaylandClient *wlclient, struct ORBitmap *bitmap, const char *window_name) {
    if (wlclient == NULL) {
        return OR_DISPLAY_INIT_ERROR;
    }
    wlclient->bitmap = bitmap;
    // TODO: Arena here
    wlclient->wayland = malloc(sizeof(struct InterWayland));
    if (wlclient->wayland == NULL) {
        inter_wl_free_window(wlclient);
        return OR_DISPLAY_CLIENT_INIT_FAILED;
    }
    enum ORWindowError error = init_wayland(wlclient);
    if (error != OR_NO_ERROR) {
        return error;
    }
    // TODO: Arena here
    wlclient->libdecor = malloc(sizeof(struct InterDecoration));
    if (wlclient->libdecor == NULL) {
        inter_wl_destroy_libdecor(wlclient);
        return OR_DISPLAY_CLIENT_INIT_FAILED;
    }
    return init_libdecor(wlclient, window_name);
}

enum ORWindowError
inter_wl_set_listeners(struct InterWaylandClient *wlclient, struct InterListeners *listeners) {
    if (!wlclient->wayland->display) {
        fprintf(stderr, "window client is not initialize");
        return OR_DISPLAY_INIT_ERROR;
    }
    wlclient->listeners = listeners;
    wl_display_roundtrip(wlclient->wayland->display);
    return OR_NO_ERROR;
}

enum ORWindowError
inter_wl_start_drawing(const struct InterWaylandClient *wlclient) {
    if (wlclient == NULL) {
        return OR_DISPLAY_INIT_ERROR;
    }

    inter_wl_start_decoration(wlclient);
    while (wlclient->libdecor->is_open) {
        if (wl_display_dispatch(wlclient->wayland->display) < 0) {
            perror("wl_display_dispatch error\n");
            break;
        }
    }

    return OR_NO_ERROR;
}

void
inter_wl_free_window(struct InterWaylandClient *wlclient) {
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


