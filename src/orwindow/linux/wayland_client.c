//
// Created by pablo on 11/29/24.
//

#include <stdlib.h>
#include <stdio.h>
#include "orwindow/orwindow_errors.h"
#include "wayland_client.h"
#include "wayland_decorator.h"

#define WAYLAND_ENV_VAR "WAYLAND_DISPLAY"

static void
register_device(void *data, struct wl_registry *registry,
                uint32_t name, const char *interface,
                uint32_t version) {
//    printf("register something good like %s\n", interface);
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
    wl_registry_add_listener(wlclient->wayland->registry, &registry_listener, wlclient);
    return OR_NO_ERROR;
}





enum ORWindowError
inter_wl_window_setup(struct InterWaylandClient *wlclient, struct ORBitmap *bitmap, const char *window_name) {
    enum ORWindowError error;
    wlclient->bitmap = bitmap;
    // TODO: Arena here
    wlclient->wayland = malloc(sizeof (struct InterWayland));
    error = init_wayland(wlclient);
    if (error != OR_NO_ERROR) {
        return error;
    }
    // TODO: Arena here
    wlclient->libdecor = malloc(sizeof(struct InterDecoration));
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