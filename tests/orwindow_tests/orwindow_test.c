//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"

uint16_t x_offset = 0;
uint16_t y_offset = 0;

void
draw(const struct ORBitmap *bitmap) {
    uint8_t *row = bitmap->mem;
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
}

struct ORWindowListeners window_listeners = {
    .draw = draw
};

void key_press(enum ORKeys code, uint8_t os_code, uint16_t time, uint8_t mod) {
    printf("pressing key %d, at %d\n", code, time);
}

struct ORKeyboardListeners keyboard_listeners = {
    .key_press = key_press,
};

int
main() {
    printf("Starting window test from the main test file\n");
    or_create_window(1024, 768, "Window Name", "com.my.window.app");
    or_surface_setup(&window_listeners, &keyboard_listeners, NULL);
    or_start_main_loop();
    or_destroy_window();
    return 0;
}
