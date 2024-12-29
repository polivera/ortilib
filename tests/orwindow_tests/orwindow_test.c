//
// Created by pablo on 11/27/24.
//

#include "orarena/orarena.h"
#include "orwindow/orwindow.h"
#include <stdio.h>

uint16_t x_offset = 0;
uint16_t y_offset = 0;
uint8_t x_stick_move = 0;
uint8_t y_stick_move = 0;
uint8_t is_key_pressed[255] = {0};
uint8_t is_mouse_button_pressed[OR_ECLICK_LEN] = {0};

void draw_little_frame(const struct ORBitmap *bitmap) {
    uint8_t *row = bitmap->mem;
    x_offset += x_stick_move;
    y_offset += y_stick_move;
    for (uint16_t y = 0; y < bitmap->height; y++) {
        uint8_t *pixel = row;
        for (uint16_t x = 0; x < bitmap->width; x++) {
            *pixel = x + x_offset;
            pixel++;

            *pixel = y + y_offset;
            pixel++;

            *pixel = 100;
            pixel++;

            *pixel = 255;
            pixel++;
        }
        row += bitmap->stride;
    }

    uint8_t speed = 4;
    if (is_key_pressed[KEY_LSHIFT] == 1 || is_key_pressed[KEY_RSHIFT] == 1) {
        speed = 8;
    }
    if (is_key_pressed[KEY_Q] == 1) {
        x_offset += speed;
        y_offset += speed;
        return;
    }
    if (is_key_pressed[KEY_E] == 1) {
        x_offset -= speed;
        y_offset += speed;
        return;
    }
    if (is_key_pressed[KEY_A] == 1) {
        x_offset += speed;
    }
    if (is_key_pressed[KEY_W] == 1) {
        y_offset += speed;
    }
    if (is_key_pressed[KEY_D] == 1) {
        x_offset -= speed;
    }
    if (is_key_pressed[KEY_S] == 1) {
        y_offset -= speed;
    }
}

// -- KEY EVENTS --------- //
void key_press(enum ORKeys code, uint8_t os_code, time_t time, uint8_t mod) {
    is_key_pressed[code] = 1;
    if (code == KEY_F11) {
        or_toggle_fullscreen();
    }
}
void key_release(enum ORKeys code, uint8_t os_code, time_t time, uint8_t mod) {
    is_key_pressed[code] = 0;
}
struct ORKeyboardListeners keyboard_listeners = {.key_press = key_press,
                                                 .key_release = key_release};

// -- MOUSE EVENTS --------- //
void mouse_move(uint16_t point_x, uint16_t point_y) {
    if (is_mouse_button_pressed[OR_CLICK_LEFT] == 1) {
        printf("mouse (%d, %d)\n", point_x, point_y);
        x_offset = -point_x;
        y_offset = -point_y;
    }
}
void mouse_press(const enum ORPointerButton button, time_t time) {
    is_mouse_button_pressed[button] = 1;
}
void mouse_release(const enum ORPointerButton button, time_t time) {
    is_mouse_button_pressed[button] = 0;
}
struct ORPointerListeners pointer_listeners = {
    .move = mouse_move,
    .button_press = mouse_press,
    .button_release = mouse_release,
};

// -- WINDOW EVENTS --------- //
void window_leave() { printf("Window left\n"); }
void window_enter() { printf("Window ENTER\n"); }
void window_fullscreen() { printf("Window is now fullscreen\n"); }
void window_close() { printf("Window is now closed\n"); }
void window_resize() { printf("Window is now resize\n"); }
struct ORWindowListeners window_listeners = {
    .draw = draw_little_frame,
    .leave = window_leave,
    .enter = window_enter,
    .resize = window_resize,
    .fullscreen = window_fullscreen,
    .close = window_close,
};

// -- GAMEPAD EVENTS --------- //
void button_press(uint8_t gamepad_id, enum ORGamepadButton button,
                  uint16_t os_button, time_t time) {
    printf("Gamepad ID [%i] has PRESSED button %i at %ld\n", gamepad_id, button,
           time);
}
void button_release(uint8_t gamepad_id, enum ORGamepadButton button,
                    uint16_t os_button, time_t time) {
    printf("Gamepad ID [%i] has RELEASE button %i at %ld\n", gamepad_id, button,
           time);
}

void gamepad_connect(uint8_t gamepad_id) {
    printf("Gamepad %i connected\n", gamepad_id);
}

void gamepad_disconnect(uint8_t gamepad_id) {
    printf("Gamepad %i disconnected\n", gamepad_id);
}

void gamepad_trigger_motion(uint8_t gamepad_id, enum ORGamepadTrigger trigger,
                            float value, time_t time) {
    printf("Gamepad %i trigger %i at %ld with force %f\n", gamepad_id, trigger,
           time, value);
}

void gamepad_stick_motion(uint8_t gamepad_id, enum ORGamepadStick stick,
                          enum ORGamepadStickAxis axis, float value,
                          time_t time) {
    if (stick == OR_GAMEPAD_LEFT_STICK && axis == OR_AXIS_X) {
        x_stick_move = value * -10;
    }
    if (stick == OR_GAMEPAD_LEFT_STICK && axis == OR_AXIS_Y) {
        y_stick_move = value * -10;
    }
}

struct ORGamepadListeners gamepad_listeners = {
    .button_press = button_press,
    .button_release = button_release,
    .trigger_motion = gamepad_trigger_motion,
    .stick_motion = gamepad_stick_motion,
    .connected = gamepad_connect,
    .disconnected = gamepad_disconnect,
};

int main() {
    printf("Starting window test from the main test file\n");
    struct ORArena *arena = arena_create_shared(500 * 1024 * 1024);

    or_create_window(1024, 768, "Window Name", "com.my.window.app",
                     sub_arena_create(arena, 50 * 1024 * 1024));
    or_surface_setup(&window_listeners, &keyboard_listeners, &pointer_listeners,
                     &gamepad_listeners);
    or_start_main_loop();
    or_destroy_window();
    return 0;
}
