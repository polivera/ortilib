//
// Created by pablo on 12/1/24.
//

#include "wayland_client.h"
#include "wayland_decorator.h"
#include "orwindow/orwindow.h"
#include <libdecor-0/libdecor.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

/**
 * The code here is the one that actually add the border to the window
 */
static void commit_frame(struct libdecor_frame *frame,
                         struct libdecor_configuration *configuration,
                         const struct ORBitmap *bitmap) {
    struct libdecor_state *state = libdecor_state_new(bitmap->width, bitmap->height);
    if (state == NULL) {
        fprintf(stderr, "Failed to allocate libdecor_state\n");
        return;
    }
    libdecor_frame_commit(frame, state, configuration);
    libdecor_state_free(state);
}

static void decoration_config(struct libdecor_frame *frame,
                              struct libdecor_configuration *configuration,
                              void *data) {
    struct InterWaylandClient *client = data;
    int width = 0, height = 0;

    if (NULL == client->bitmap->mem) {
        inter_wl_window_resize(client);
        inter_frame_render(client);
    } else if (libdecor_configuration_get_content_size(configuration, frame, &width, &height)) {
        const int unmap_res = munmap(client->bitmap->mem, client->bitmap->mem_size);
        if (unmap_res == -1) {
            fprintf(stderr, "cannot unmap bitmap memory");
            return;
        }
        or_bitmap_reset(client->bitmap, width, height);
        inter_wl_window_resize(client);
    }
    commit_frame(frame, configuration, client->bitmap);
}

static void decoration_commit(struct libdecor_frame *frame, void *data) {
    printf("Decoration commit\n");
}

static void decoration_close(struct libdecor_frame *frame, void *data) {
    const struct InterWaylandClient *client = data;
    client->libdecor->is_open = false;
    printf("decoration close \n");
}

/**
 * Decoration frame events.
 */
static struct libdecor_frame_interface frame_events = {
    .configure = decoration_config,
    .commit = decoration_commit,
    .close = decoration_close,
};

// Initialize libdecor
enum ORWindowError init_libdecor(struct InterWaylandClient *wlclient) {
    wlclient->libdecor->is_fullscreen = false;
    wlclient->libdecor->is_open = false;
    wlclient->libdecor->base = libdecor_new(wlclient->wayland->display, NULL);
    if (!wlclient->libdecor->base) {
        fprintf(stderr, "Cannot initialize libdecor base\n");
        return OR_LIBDECOR_INIT_ERROR;
    }
    wlclient->libdecor->frame = libdecor_decorate(wlclient->libdecor->base,
                                                  wlclient->wayland->surface,
                                                  &frame_events,
                                                  wlclient);
    if (!wlclient->libdecor->frame) {
        fprintf(stderr, "Cannot initialize libdecor frame\n");
        return OR_LIBDECOR_FRAME_ERROR;
    }
    return OR_NO_ERROR;
}

/**
 * Start window decoration.
 * @param libdecor_client
 */
void inter_wl_start_decoration(struct InterDecoration *libdecor_client) {
    libdecor_frame_ref(libdecor_client->frame);
    libdecor_frame_set_title(libdecor_client->frame, libdecor_client->name);
    libdecor_frame_map(libdecor_client->frame);
    libdecor_client->is_open = true;
}

/**
 * Clean up libdecor decoration.
 * @param wlclient InterWaylandClient
 */
void inter_wl_destroy_libdecor(struct InterWaylandClient *wlclient) {
    if (wlclient) {
        if (wlclient->libdecor->frame) {
            libdecor_frame_unref(wlclient->libdecor->frame);
            wlclient->libdecor->frame = NULL;
        }
        if (wlclient->libdecor->base) {
            libdecor_unref(wlclient->libdecor->base);
            wlclient->libdecor->base = NULL;
        }
        // TODO: Arenas here.
        free(wlclient->libdecor);
        wlclient->libdecor = NULL;
    }
}
