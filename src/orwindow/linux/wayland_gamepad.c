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
#include <string.h>
#include <unistd.h>

#define LEFT_TRIGGER_ID 2
#define RIGHT_TRIGGER_ID 5
// If a value greater than this two is added, fix handle_axis_event
#define LEFT_RIGHT_DPAD_AXIS 6
#define UP_DOWN_DPAD_AXIS 7

struct GamepadThread {
    pthread_t thread;
    const bool *is_running;
    int gamepad_id;
    const struct ORGamepadListeners *listeners;
};

static struct GamepadThread gamepad_threads[OR_MAX_GAMEPADS] = {0};
static struct GamepadState gamepads[OR_MAX_GAMEPADS] = {0};
static enum ORGamepadButton button_map[15] = {0};
static enum ORGamepadStick sticks_and_triggers_map[6] = {0};
static enum ORGamepadStickAxis sticks_axis[5] = {0};
static uint16_t stick_deadzone = 2000;

static void
initialize_sticks_axis() {
    sticks_axis[0] = OR_AXIS_X;
    sticks_axis[1] = OR_AXIS_Y;
    sticks_axis[3] = OR_AXIS_X;
    sticks_axis[4] = OR_AXIS_Y;
}

static void
initialize_sticks_and_triggers_map() {
    sticks_and_triggers_map[0] = OR_GAMEPAD_LEFT_STICK;
    sticks_and_triggers_map[1] = OR_GAMEPAD_LEFT_STICK;
    sticks_and_triggers_map[2] = OR_GAMEPAD_LEFT_TRIGGER;
    sticks_and_triggers_map[3] = OR_GAMEPAD_RIGHT_STICK;
    sticks_and_triggers_map[4] = OR_GAMEPAD_RIGHT_STICK;
    sticks_and_triggers_map[5] = OR_GAMEPAD_RIGHT_TRIGGER;
}

static void
initialize_button_map() {
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
}

static void
button_action(const int gamepad_id, const struct ORGamepadListeners *listeners,
              const uint8_t button_number, const uint8_t os_button_number,
              const bool press) {
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

static void
handle_button_event(const int gamepad_id, const struct js_event *event,
                    const struct ORGamepadListeners *listeners) {
    if (event->number < 0 || event->number > 10) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_UNKNOWN, event->number,
                      event->value > 0);
        return;
    }
    button_action(gamepad_id, listeners, button_map[event->number],
                  event->number, event->value > 0);
}

static void
handle_dpad_event(const int gamepad_id, const struct js_event *event,
                  const struct ORGamepadListeners *listeners) {
    if (event->number == UP_DOWN_DPAD_AXIS) {
        if (event->value > 0) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_DOWN, -1,
                          true);
            gamepads[gamepad_id].dpad_down_pressed = true;
            return;
        }

        if (event->value < 0) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_UP, -1, true);
            return;
        }

        if (gamepads[gamepad_id].dpad_down_pressed) {
            button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_DOWN, -1,
                          false);
            gamepads[gamepad_id].dpad_down_pressed = false;
            return;
        }

        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_UP, -1, false);
        return;
    }

    if (event->value > 0) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_RIGHT, -1, true);
        gamepads[gamepad_id].dpad_right_pressed = true;
        return;
    }

    if (event->value < 0) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_LEFT, -1, true);
        return;
    }

    if (gamepads[gamepad_id].dpad_right_pressed) {
        button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_RIGHT, -1, false);
        gamepads[gamepad_id].dpad_right_pressed = false;
        return;
    }

    button_action(gamepad_id, listeners, OR_GAMEPAD_DPAD_LEFT, -1, false);
}

static void
handle_trigger_event(const int gamepad_id, const struct js_event *event,
                     const struct ORGamepadListeners *listeners) {
    if (!listeners->trigger_motion)
        return;

    const float trigger_value = (float)(event->value + INT16_MAX) / 65534.0f;
    listeners->trigger_motion(gamepad_id,
                              sticks_and_triggers_map[event->number],
                              trigger_value, time(NULL));
}

static void
handle_stick_event(const int gamepad_id, const struct js_event *event,
                   const struct ORGamepadListeners *listeners) {
    if (!listeners->stick_motion)
        return;

    float stick_value = (float)event->value / 32767.0f;
    if (event->value < stick_deadzone && event->value > stick_deadzone * -1)
        stick_value = 0.0f;

    listeners->stick_motion(gamepad_id, sticks_and_triggers_map[event->number],
                            sticks_axis[event->number], stick_value,
                            time(NULL));
}

static void
handle_axis_event(const int gamepad_id, const struct js_event *event,
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

static void
handle_gamepad_event(const int gamepad_id, const struct js_event *event,
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
        printf("unknown joystick event type=%i value=%i number=%i \n",
               event->type, event->value, event->number);
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

bool
gamepad_set_rumble(const int gamepad_id, const float strong_magnitude,
                   const float weak_magnitude) {
    struct GamepadState *gamepad = &gamepads[gamepad_id];
    if (!gamepad->has_rumble)
        return false;

    struct ff_effect effect;
    struct input_event play;

    // Update strong rumble
    memset(&effect, 0, sizeof(effect));
    effect.type = FF_RUMBLE;
    effect.id = gamepad->effect_id;
    effect.u.rumble.strong_magnitude = (uint16_t)(strong_magnitude * 0xFFFF);
    effect.u.rumble.weak_magnitude = (uint16_t)(weak_magnitude * 0xFFFF);

    if (ioctl(gamepad->ff_fd, EVIOCSFF, &effect) < 0)
        return false;

    // Play strong effect
    play.type = EV_FF;
    play.code = effect.id;
    play.value = 1;
    if (write(gamepad->ff_fd, &play, sizeof(play)) < 0)
        return false;

    return true;
}

void
setup_gamepad_rumble(struct GamepadState *gamepad) {
    for (int i = 0; i < 32; i++) {
        char event_path[64];
        snprintf(event_path, sizeof(event_path), "/dev/input/event%d", i);
        const int fd = open(event_path, O_RDWR);
        if (fd < 0)
            continue;

        // Check if this is the right device
        unsigned long features[4];
        // TODO: wtf this line do?
        if (ioctl(fd, EVIOCGBIT(0, sizeof(features)), features) < 0) {
            close(fd);
            continue;
        }

        // Check if device supports force feedback
        if (features[0] & (1 << EV_FF)) {
            gamepad->ff_fd = fd;
            gamepad->has_rumble = true;

            // Upload effects
            struct ff_effect effect = {0};

            // Setup strong rumble effect
            effect.type = FF_RUMBLE;
            effect.id = -1;
            effect.u.rumble.strong_magnitude = 0;
            effect.u.rumble.weak_magnitude = 0;

            if (ioctl(fd, EVIOCSFF, &effect) < 0) {
                gamepad->has_rumble = false;
                close(fd);
                continue;
            }
            gamepad->effect_id = effect.id;
            break;
        }
        close(fd);
    }
}

static void *
single_gamepad_thread(void *arg) {
    const struct GamepadThread *thread_data = arg;
    struct js_event event;

    while (*thread_data->is_running) {
        if (!gamepads[thread_data->gamepad_id].is_connected) {
            char device_path[20];
            snprintf(device_path, sizeof(device_path), GAMEPAD_PATH_FORMAT,
                     thread_data->gamepad_id);

            gamepads[thread_data->gamepad_id].fd =
                open(device_path, O_RDONLY | O_NONBLOCK);
            if (gamepads[thread_data->gamepad_id].fd != -1) {
                setup_gamepad_rumble(&gamepads[thread_data->gamepad_id]);
                handle_controller_connected(thread_data->gamepad_id,
                                            thread_data->listeners);
            }
        }

        if (gamepads[thread_data->gamepad_id].is_connected) {
            while (read(gamepads[thread_data->gamepad_id].fd, &event,
                        sizeof(event)) > 0) {
                handle_gamepad_event(thread_data->gamepad_id, &event,
                                     thread_data->listeners);
            }

            if (errno == ENODEV) {
                close(gamepads[thread_data->gamepad_id].fd);
                handle_controller_disconnected(thread_data->gamepad_id,
                                               thread_data->listeners);
            }
        }
        usleep(1000); // Sleep for 1ms to prevent busy waiting
    }

    return NULL;
}

void
setup_gamepad(const struct InterWaylandClient *client) {
    initialize_button_map();
    initialize_sticks_and_triggers_map();
    initialize_sticks_axis();

    for (int i = 0; i < OR_MAX_GAMEPADS; i++) {
        gamepad_threads[i].gamepad_id = i;
        gamepad_threads[i].is_running = &client->is_running;
        gamepad_threads[i].listeners = client->listeners->gamepad_listeners;

        if (pthread_create(&gamepad_threads[i].thread, NULL,
                           single_gamepad_thread, &gamepad_threads[i]) != 0) {
            fprintf(stderr,
                    "Failed to create gamepad thread for controller %d\n", i);
        }
    }
}

void
cleanup_gamepad(void) {
    for (int i = 0; i < OR_MAX_GAMEPADS; i++) {
        pthread_join(gamepad_threads[i].thread, NULL);

        if (gamepads[i].is_connected) {
            close(gamepads[i].fd);
            gamepads[i].is_connected = false;
        }
    }
}