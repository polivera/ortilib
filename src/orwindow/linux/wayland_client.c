//
// Created by pablo on 11/29/24.
//

#include <stdlib.h>
#include <stdio.h>
#include "orwindow/orwindow_errors.h"
#include "wayland_client.h"

#define WAYLAND_ENV_VAR "WAYLAND_DISPLAY"

static void
register_device(void *data, struct wl_registry *registry,
                uint32_t name, const char *interface,
                uint32_t version) {
    printf("register something good like %s\n", interface);
}

struct wl_registry_listener registry_listener = {
    .global = register_device
};

/*
 * TODO: Register listeners should be in here because the have to go before
 * wl_add_listeners.
 * It is a pointer so it can be done later but I have to see what is best
 */
enum ORWindowError
inter_window_setup(struct InterWaylandClient *wlclient, struct ORBitmap *bitmap) {
    wlclient->buffer = NULL;
    wlclient->bitmap = bitmap;

    wlclient->display = wl_display_connect(getenv(WAYLAND_ENV_VAR));
    if (!wlclient->display) {
        wlclient->display = wl_display_connect(NULL);
        if (!wlclient->display) {
            fprintf(stderr, "Failed to connect to wayland display server\n");
            return OR_DISPLAY_INIT_ERROR;
        }
    }

    wlclient->registry = wl_display_get_registry(wlclient->display);
    if (!wlclient->registry) {
        fprintf(stderr, "Cannot initialize display registry\n");
        return OR_WAYLAND_SURFACE_INIT_ERROR;
    }

    wl_registry_add_listener(wlclient->registry, &registry_listener, wlclient);
    wl_display_roundtrip(wlclient->display);

    return OR_NO_ERROR;
}

int8_t
orwl_set_listeners(struct InterListeners *listeners) {
    return 0;
}