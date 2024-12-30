//
// Created by pablo on 12/20/24.
//

#ifndef WAYLAND_GAMEPAD_H
#define WAYLAND_GAMEPAD_H

#include "orwindow/orwindow_gamepad.h"
#include "wayland_client.h"
#include <linux/input.h>
#include <stdbool.h>

#define GAMEPAD_PATH_FORMAT "/dev/input/js%d"
#define GAMEPAD_EVENT_PATH_FORMAT "/dev/input/event%d"

// Gamepad state structure
// TODO: Is gamepad state still a good name for this?
struct GamepadState {
    int fd;
    // Device ID
    dev_t device_id;
    bool is_connected;
    // Dpad state
    bool dpad_down_pressed;
    bool dpad_right_pressed;
    // Force Feedback;
    int ff_fd;
    struct ff_effect *ff_effect;
};

void
setup_gamepad(const struct InterWaylandClient *client);

bool
gamepad_set_rumble(enum ORGamepadID gamepad_id, float strong_magnitude,
                   float weak_magnitude);

void
cleanup_gamepads(void);

#endif // WAYLAND_GAMEPAD_H
