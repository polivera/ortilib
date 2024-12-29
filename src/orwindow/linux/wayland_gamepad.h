//
// Created by pablo on 12/20/24.
//

#ifndef WAYLAND_GAMEPAD_H
#define WAYLAND_GAMEPAD_H

#include "orwindow/orwindow_gamepad.h"
#include "wayland_client.h"
#include <stdbool.h>
#include <stdint.h>

// Maximum number of gamepads that can be connected simultaneously
#define GAMEPAD_PATH_FORMAT "/dev/input/js%d"
#define GAMEPAD_EVENT_PATH_FORMAT "/dev/input/event%d"

// Gamepad state structure
struct GamepadState {
    int fd;                 // File descriptor for the gamepad
    bool is_connected;      // Whether the gamepad is currently connected
    short axes[8];          // Raw axes values
    uint16_t button_states; // Bitmap of current button states
    bool dpad_down_pressed;
    bool dpad_right_pressed;

    int ff_fd;       // Force feedback file descriptor
    short effect_id; // Effect ID for strong rumble
    bool has_rumble; // Whether the device supports rumble
};

void
setup_gamepad(const struct InterWaylandClient *client);

bool
gamepad_set_rumble(int gamepad_id, float strong_magnitude,
                   float weak_magnitude);

void
cleanup_gamepad(void);

#endif // WAYLAND_GAMEPAD_H
