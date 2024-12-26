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

static struct GamepadState gamepads[OR_MAX_GAMEPADS] = {0};
static enum ORGamepadButton button_map[11] = {0};
static enum ORGamepadStick stick_map[16] = {0};
static pthread_t gamepad_thread;

static void initialize_gamepad_ids() {}

static void initialize_stick_map() {}

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
}

static void handle_button_event(const int gamepad_id,
                                const struct js_event *event,
                                const struct ORGamepadListeners *listeners) {
    if (event->value > 0) {
        if (listeners->button_press) {
            listeners->button_press(gamepad_id, button_map[event->number],
                                    event->number, time(NULL));
        }
        return;
    }
    if (listeners->button_release) {
        listeners->button_release(gamepad_id, button_map[event->number],
                                  event->number, time(NULL));
    }
}

static void handle_axis_event(const int gamepad_id,
                              const struct js_event *event) {
    // TODO: too many messages from axis
    // TODO: RT and LT are axis and not buttons? Axis 2 and 3
    if (event->type & JS_EVENT_AXIS && event->number == 1) {
        // TODO: Limit messages for those over 2000
        // TODO: Change the method parameters to send the following:
        // - Control ID
        // - Stick ID (enum)
        // - Direction (enum)
        // - Value int
        if (event->value > 2000 || event->value < -2000) {
            printf("motion axis: %i - motion value: %i\n", event->number,
                   event->value);
        }
    }
}

static void handle_gamepad_event(const int gamepad_id,
                                 const struct js_event *event,
                                 const struct ORGamepadListeners *listeners) {
    switch (event->type) {
    case JS_EVENT_BUTTON:
        handle_button_event(gamepad_id, event, listeners);
        break;
    case JS_EVENT_AXIS:
        // handle_axis_event(gamepad_id, event);
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