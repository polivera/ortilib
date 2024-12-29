//
// Created by pablo on 11/27/24.
//

#ifndef ORTILIB_ORWINDOW_INTERNAL_H
#define ORTILIB_ORWINDOW_INTERNAL_H

#include "orarena/orarena.h"
#include "orwindow/orwindow.h"

struct InterListeners {
    struct ORWindowListeners *window_listeners;
    struct ORKeyboardListeners *keyboard_listeners;
    struct ORPointerListeners *pointer_listeners;
    struct ORGamepadListeners *gamepad_listeners;
};

enum ORWindowError
inter_create_window(const char *window_name, const char *process_name,
                    struct ORArena *arena);

enum ORWindowError
inter_surface_setup(struct ORBitmap *bitmap, struct InterListeners *listeners);

enum ORWindowError
inter_start_drawing();

enum ORWindowError
inter_toggle_fullscreen();

void
inter_remove_window(struct ORArena *arena);

#endif // ORTILIB_ORWINDOW_INTERNAL_H
