//
// Created by pablo on 12/20/24.
//

#include "wayland_gamepad.h"
#include "orwindow/orwindow_gamepad.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static struct GamepadState gamepads[MAX_GAMEPADS] = {0};
static struct ORGamepadListeners gamepad_listeners = {0};
static pthread_t gamepad_thread;
static bool thread_running = false;

// Maps raw button codes to our enum
static uint16_t button_map[16] = {0};

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
                                const struct js_event *event) {
    if (event->type & JS_EVENT_BUTTON) {
        if (gamepad_listeners.button_press && event->value) {
            gamepad_listeners.button_press(
                gamepad_id, button_map[event->number], event->time);
        } else if (gamepad_listeners.button_release && !event->value) {
            gamepad_listeners.button_release(
                gamepad_id, button_map[event->number], event->time);
        }
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
    if (event->type & JS_EVENT_AXIS && gamepad_listeners.axis_motion) {
        printf("motion axis: %i - motion value: %i\n", event->number,
               event->value);
        gamepad_listeners.axis_motion(gamepad_id, event->number, event->value,
                                      event->time);
    }
}

static void *gamepad_poll_thread(void *arg) {
    struct js_event event;

    // TODO: This has to match the window close
    // TODO: Use is_open (or similar) from wayland client.
    while (thread_running) {
        for (int i = 0; i < MAX_GAMEPADS; i++) {
            // TODO: This could go on a static function so I can use it on entry
            // point. If no controllers are connected I should not call threads
            if (!gamepads[i].is_connected) {
                char device_path[20];
                snprintf(device_path, sizeof(device_path), GAMEPAD_PATH_FORMAT,
                         i);

                gamepads[i].fd = open(device_path, O_RDONLY | O_NONBLOCK);
                if (gamepads[i].fd != -1) {
                    gamepads[i].is_connected = true;
                    if (gamepad_listeners.connected) {
                        gamepad_listeners.connected(i);
                    }
                }
            }

            if (gamepads[i].is_connected) {
                while (read(gamepads[i].fd, &event, sizeof(event)) > 0) {
                    handle_button_event(i, &event);
                    handle_axis_event(i, &event);
                }

                // Check if device was disconnected
                if (errno == ENODEV) {
                    close(gamepads[i].fd);
                    gamepads[i].is_connected = false;
                    if (gamepad_listeners.disconnected) {
                        gamepad_listeners.disconnected(i);
                    }
                }
            }
        }
        usleep(1000); // Sleep for 1ms to prevent busy waiting
    }
    return NULL;
}

void setup_gamepad(const struct ORGamepadListeners *gp_listeners,
                   struct InterWayland *wayland) {
    // TODO: Remove this intermediate variable, if no event were given the
    // thread should not work
    if (gp_listeners) {
        gamepad_listeners = *gp_listeners;
    }

    initialize_button_map();

    // TODO: Should I open thread if there are no controllers connected?
    // TODO: Pass wayland client.
    // Start the gamepad polling thread
    thread_running = true;
    if (pthread_create(&gamepad_thread, NULL, gamepad_poll_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create gamepad polling thread\n");
        return;
    }
}

void cleanup_gamepad(void) {
    thread_running = false;
    pthread_join(gamepad_thread, NULL);

    for (int i = 0; i < MAX_GAMEPADS; i++) {
        if (gamepads[i].is_connected) {
            close(gamepads[i].fd);
            gamepads[i].is_connected = false;
        }
    }
}