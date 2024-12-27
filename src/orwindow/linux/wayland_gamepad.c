//
// Created by pablo on 12/20/24.
//

#include "wayland_gamepad.h"

#include "orwindow/orwindow_gamepad.h"
#include "orwindow_internal.h"
#include "wayland_decorator.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define LEFT_TRIGGER_ID 2
#define RIGHT_TRIGGER_ID 5
// If a value greater than this two is added, fix handle_axis_event
#define LEFT_RIGHT_DPAD_AXIS 6
#define UP_DOWN_DPAD_AXIS 7

static struct GamepadState gamepads[OR_MAX_GAMEPADS] = {0};
static enum ORGamepadButton button_map[15] = {0};
static enum ORGamepadStick axis_input_map[6] = {0};
// Index 0 is true if DOWN arrow was pressed before and index 1 is true if RIGHT
// arrow was pressed
static bool was_positive_dpad_axis_pressed[2] = {false, false};
static pthread_t gamepad_thread;

static void initialize_gamepad_ids() {}

static void initialize_axis_inputs_map() {
    axis_input_map[0] = OR_LEFT_STICK;
    axis_input_map[1] = OR_LEFT_STICK;
    axis_input_map[2] = OR_LEFT_TRIGGER;
    axis_input_map[3] = OR_RIGHT_STICK;
    axis_input_map[4] = OR_RIGHT_STICK;
    axis_input_map[5] = OR_RIGHT_TRIGGER;
}

static void initialize_button_map() {
    button_map[0] = OR_GAMEPAD_A;
    button_map[1] = OR_GAMEPAD_B;
    button_map[2] = OR_GAMEPAD_X;
    button_map[3] = OR_GAMEPAD_Y;
    button_map[4] = OR_GAMEPAD_LB;
    button_map[5] = OR_GAMEPAD_RB;
    button_map[6] = OR_GAMEPAD_SELECT;
    button_map[7] = OR_GAMEPAD_START;
    button_map[8] = OR_GAMEPAD_HOME;
    button_map[9] = OR_GAMEPAD_L3;
    button_map[10] = OR_GAMEPAD_R3;
    button_map[11] = OR_GAMEPAD_DPAD_UP;
    button_map[12] = OR_GAMEPAD_DPAD_DOWN;
    button_map[13] = OR_GAMEPAD_DPAD_LEFT;
    button_map[14] = OR_GAMEPAD_DPAD_RIGHT;
    // TODO: Add DPAD map
}

static void button_action(const int gamepad_id,
                          const struct ORGamepadListeners *listeners,
                          const uint8_t button_number,
                          const uint8_t os_button_number, const bool press) {
    if (press && listeners->button_press) {
        listeners->button_press(gamepad_id, button_number, os_button_number,
                                time(NULL));
        return;
    }
    if (listeners->button_release) {
        listeners->button_release(gamepad_id, button_number, os_button_number,
                                  time(NULL));
    }
}

static void handle_button_event(const int gamepad_id,
                                const struct js_event *event,
                                const struct ORGamepadListeners *listeners) {
    if (event->number < 0 || event->number > 10) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_UNKNOWN, event->number,
                      event->value > 0);
        return;
    }
    button_action(gamepad_id, listeners, button_map[event->number],
                  event->number, event->value > 0);
}

static void handle_stick_event(const int gamepad_id,
                               const struct js_event *event,
                               const struct ORGamepadListeners *listeners) {
    // printf("handle_stick_event: gamepad_id=%d\n", gamepad_id);
}

static void handle_dpad_event(const int gamepad_id,
                              const struct js_event *event,
                              const struct ORGamepadListeners *listeners) {
    if (event->number == UP_DOWN_DPAD_AXIS) {
        if (event->value > 0) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_DOWN, -1,
                          true);
            was_positive_dpad_axis_pressed[0] = true;
            return;
        }

        if (event->value < 0) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_UP, -1, true);
            return;
        }

        if (was_positive_dpad_axis_pressed[0]) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_DOWN, -1,
                          false);
            was_positive_dpad_axis_pressed[0] = false;
            return;
        }

        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_UP, -1, false);
        return;
    }

    if (event->value > 0) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_RIGHT, -1, true);
        was_positive_dpad_axis_pressed[1] = true;
        return;
    }

    if (event->value < 0) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_LEFT, -1, true);
        return;
    }

    if (was_positive_dpad_axis_pressed[1]) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_RIGHT, -1, false);
        was_positive_dpad_axis_pressed[1] = false;
        return;
    }

    button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_LEFT, -1, false);
}

static void handle_trigger_event(const int gamepad_id,
                                 const struct js_event *event,
                                 const struct ORGamepadListeners *listeners) {
    printf("handle_trigger_event: gamepad_id=%d\n", gamepad_id);
}

static void handle_axis_event(const int gamepad_id,
                              const struct js_event *event,
                              const struct ORGamepadListeners *listeners) {

    if (event->number == LEFT_TRIGGER_ID || event->number == RIGHT_TRIGGER_ID) {
        handle_trigger_event(gamepad_id, event, listeners);
        return;
    }
    // This takes both left right and up down because they are the latest two.
    if (event->number >= LEFT_RIGHT_DPAD_AXIS) {
        handle_dpad_event(gamepad_id, event, listeners);
        return;
    }
    handle_stick_event(gamepad_id, event, listeners);
}

static void handle_gamepad_event(const int gamepad_id,
                                 const struct js_event *event,
                                 const struct ORGamepadListeners *listeners) {
    switch (event->type) {
    case JS_EVENT_BUTTON:
        handle_button_event(gamepad_id, event, listeners);
        break;
    case JS_EVENT_AXIS:
        handle_axis_event(gamepad_id, event, listeners);
        break;
    case JS_EVENT_INIT:
        printf("initializing gamepad id: %i\n", gamepad_id);
        break;
    default:
        // printf("unknown joystick event %i \n", event->type);
        break;
    }
}

static void
handle_controller_connected(const int gamepad_id,
                            const struct ORGamepadListeners *listeners) {
    gamepads[gamepad_id].is_connected = true;
    if (listeners->connected) {
        listeners->connected(gamepad_id);
    }
}

static void
handle_controller_disconnected(const int gamepad_id,
                               const struct ORGamepadListeners *listeners) {
    gamepads[gamepad_id].is_connected = false;
    if (listeners->disconnected) {
        listeners->disconnected(gamepad_id);
    }
}

static void *gamepad_poll_thread(void *arg) {
    struct js_event event;
    const struct InterWaylandClient *client = arg;

    while (client->is_running) {
        for (int i = 0; i < OR_MAX_GAMEPADS; i++) {
            // TODO: This could go on a static function so I can use it on entry
            // point. If no controllers are connected I should not call threads
            if (!gamepads[i].is_connected) {
                char device_path[20];
                snprintf(device_path, sizeof(device_path), GAMEPAD_PATH_FORMAT,
                         i);

                gamepads[i].fd = open(device_path, O_RDONLY | O_NONBLOCK);
                if (gamepads[i].fd != -1) {
                    handle_controller_connected(
                        i, client->listeners->gamepad_listeners);
                }
            }

            if (gamepads[i].is_connected) {
                // Should each controller be its own thread? what happen if
                // if the controller keep sending events?
                while (read(gamepads[i].fd, &event, sizeof(event)) > 0) {
                    handle_gamepad_event(i, &event,
                                         client->listeners->gamepad_listeners);
                }
                // Check if device was disconnected
                if (errno == ENODEV) {
                    close(gamepads[i].fd);
                    handle_controller_disconnected(
                        i, client->listeners->gamepad_listeners);
                }
            }
        }
        usleep(1000); // Sleep for 1ms to prevent busy waiting
    }
    return NULL;
}

void setup_gamepad(struct InterWaylandClient *client) {
    initialize_button_map();
    initialize_axis_inputs_map();

    // TODO: Shouldn't I use a thread for each control?
    // Start the gamepad polling thread
    if (pthread_create(&gamepad_thread, NULL, gamepad_poll_thread, client) !=
        0) {
        fprintf(stderr, "Failed to create gamepad polling thread\n");
    }
}

void cleanup_gamepad(void) {
    pthread_join(gamepad_thread, NULL);

    for (int i = 0; i < OR_MAX_GAMEPADS; i++) {
        if (gamepads[i].is_connected) {
            close(gamepads[i].fd);
            gamepads[i].is_connected = false;
        }
    }
}