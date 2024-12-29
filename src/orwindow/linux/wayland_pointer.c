//
// Created by pablo on 12/9/24.
//

#include "orwindow/orwindow.h"
#include "wayland_client.h"
#include <stdio.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

#define MIN_BUTTON_ID 271

// struct ORPointerListeners pointer_listeners;

static enum ORPointerButton button_map[20] = {OR_CLICK_UNKNOWN};

static void initialize_button_map() {
    button_map[1] = OR_CLICK_LEFT;
    button_map[2] = OR_CLICK_RIGHT;
    button_map[3] = OR_CLICK_MIDDLE;
    button_map[4] = OR_CLICK_CUSTOM_A;
    button_map[5] = OR_CLICK_CUSTOM_B;
    button_map[6] = OR_CLICK_CUSTOM_C;
}

static void pointer_enter(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t sx, wl_fixed_t sy) {
    const struct ORPointerListeners *listeners = data;
    if (listeners->enter) {
        listeners->enter(wl_fixed_to_int(sx), wl_fixed_to_int(sy));
    }
}

static void pointer_motion(void *data, struct wl_pointer *pointer,
                           uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {
    const struct ORPointerListeners *listeners = data;
    if (listeners->move) {
        listeners->move(wl_fixed_to_int(sx), wl_fixed_to_int(sy));
    }
}

static void pointer_leave(void *data, struct wl_pointer *wl_pointer,
                          uint32_t serial, struct wl_surface *surface) {
    const struct ORPointerListeners *listeners = data;
    if (listeners->leave) {
        listeners->leave();
    }
}

static void pointer_button(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state) {
    const struct ORPointerListeners *listeners = data;
    button -= MIN_BUTTON_ID;
    if (state == WL_POINTER_BUTTON_STATE_PRESSED && listeners->button_press) {
        listeners->button_press(button_map[button], time);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED &&
               listeners->button_release) {
        listeners->button_release(button_map[button], time);
    }
}

static void pointer_axis(void *data, struct wl_pointer *wl_pointer,
                         uint32_t time, uint32_t axis, wl_fixed_t value) {
    const struct ORPointerListeners *listeners = data;
    if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL && listeners->scroll_x) {
        listeners->scroll_x(wl_fixed_to_int(value), time);
    } else if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL &&
               listeners->scroll_y) {
        listeners->scroll_y(wl_fixed_to_int(value), time);
    }
}

void setup_pointer(struct ORPointerListeners *pt_listeners,
                   struct InterWayland *wayland) {
    wayland->pointer = wl_seat_get_pointer(wayland->seat);
    if (!wayland->pointer) {
        fprintf(stderr, "Failed to load pointer from seat.\n");
        return;
    }
    initialize_button_map();

    static struct wl_pointer_listener wl_pointer = {
        .enter = pointer_enter,
        .leave = pointer_leave,
        .motion = pointer_motion,
        .button = pointer_button,
        .axis = pointer_axis,
    };

    wl_pointer_add_listener(wayland->pointer, &wl_pointer, pt_listeners);
}
