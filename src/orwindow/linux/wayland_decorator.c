//
// Created by pablo on 12/1/24.
//

#include "wayland_client.h"
#include "wayland_decorator.h"
#include "orwindow/orwindow.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * The code here is the one that actually add the border to the window
 */
static void commit_frame(struct libdecor_frame *frame,
                         struct libdecor_configuration *configuration, void *data) {
    printf("committing frame\n");
}

static void decoration_config(struct libdecor_frame *frame,
                              struct libdecor_configuration *configuration,
                              void *data) {
    printf("deco config\n");
}

static void decoration_commit(struct libdecor_frame *frame, void *data) {
    printf("Decoration commit\n");
}

static void decoration_close(struct libdecor_frame *frame, void *data) {
    printf("decoration close \n");
}

static struct libdecor_frame_interface frame_events = {
        .configure = decoration_config,
        .commit = decoration_commit,
        .close = decoration_close,
};


/**
 * Initialize libdecor
 * @param wlclient
 * @param window_name
 * @return ORWindowError
 */
enum ORWindowError init_libdecor(struct InterWaylandClient *wlclient, const char *window_name) {
    wlclient->libdecor = malloc(sizeof(struct InterDecoration));
    wlclient->libdecor->name = window_name;
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


    libdecor_frame_ref(wlclient->libdecor->frame);
    libdecor_frame_set_title(wlclient->libdecor->frame, wlclient->libdecor->name);
    libdecor_frame_map(wlclient->libdecor->frame); // TODO: This throws an error, why is not initializing?
    return OR_NO_ERROR;
}

enum ORWindowError clear_libdecor(struct InterWaylandClient *wlclient) {
    if (!wlclient || !wlclient->libdecor) {
        return OR_LIBDECOR_ERROR_CLEANING;
    }
    if (wlclient->libdecor->frame) {
        libdecor_frame_close(wlclient->libdecor->frame);
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
    return OR_NO_ERROR;
}
