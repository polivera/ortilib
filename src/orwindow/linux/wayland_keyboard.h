//
// Created by pablo on 12/9/24.
//

#ifndef WAYLAND_KEYBOARD_H
#define WAYLAND_KEYBOARD_H

#include "orwindow/orwindow.h"
#include "wayland_client.h"

void setup_keyboard(
    const struct ORWindowListeners* win_listeners,
    const struct ORKeyboardListeners* key_listeners, struct InterWayland* wayland);

#endif //WAYLAND_KEYBOARD_H
