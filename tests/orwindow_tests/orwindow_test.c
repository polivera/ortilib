//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"

uint16_t x_offset = 0;
uint16_t y_offset = 0;
uint8_t is_key_pressed[255] = {0};
uint8_t is_mouse_button_pressed[OR_ECLICK_LEN] = {0};

void drawLittleFrame(const struct ORBitmap* bitmap)
{
    uint8_t* row = bitmap->mem;
    for (uint16_t y = 0; y < bitmap->height; y++)
    {
        uint8_t* pixel = row;
        for (uint16_t x = 0; x < bitmap->width; x++)
        {
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
    if (is_key_pressed[KEY_LSHIFT] == 1 || is_key_pressed[KEY_RSHIFT] == 1)
    {
        speed = 8;
    }

    if (is_key_pressed[KEY_Q] == 1)
    {
        x_offset += speed;
        y_offset += speed;
        return;
    }
    if (is_key_pressed[KEY_E] == 1)
    {
        x_offset -= speed;
        y_offset += speed;
        return;
    }
    if (is_key_pressed[KEY_A] == 1)
    {
        x_offset += speed;
    }
    if (is_key_pressed[KEY_W] == 1)
    {
        y_offset += speed;
    }
    if (is_key_pressed[KEY_D] == 1)
    {
        x_offset -= speed;
    }
    if (is_key_pressed[KEY_S] == 1)
    {
        y_offset -= speed;
    }
}

void key_press(enum ORKeys code, uint8_t os_code, time_t time, uint8_t mod)
{
    is_key_pressed[code] = 1;
    if (code == KEY_F11)
    {
        or_toggle_fullscreen();
    }
}

void key_release(enum ORKeys code, uint8_t os_code, time_t time, uint8_t mod)
{
    is_key_pressed[code] = 0;
}

void mouse_move(uint16_t point_x, uint16_t point_y)
{
    if (is_mouse_button_pressed[OR_CLICK_LEFT] == 1)
    {
        x_offset = -point_x;
        y_offset = -point_y;
    }
}

void mouse_press(const enum ORPointerButton button, time_t time)
{
    is_mouse_button_pressed[button] = 1;
}

void mouse_release(const enum ORPointerButton button, time_t time)
{
    is_mouse_button_pressed[button] = 0;
}

void window_leave() { printf("Window left\n"); }

void window_fullscreen()
{
    printf("Window is now fullscreen\n");
}

void window_close()
{
    printf("Window is now closed\n");
}

struct ORKeyboardListeners keyboard_listeners = {.key_press = key_press,
                                                 .key_release = key_release};
struct ORPointerListeners pointer_listeners = {
    .move = mouse_move,
    .button_press = mouse_press,
    .button_release = mouse_release,
};

struct ORWindowListeners window_listeners = {
    .draw = drawLittleFrame,
    .leave = window_leave,
    .fullscreen = window_fullscreen,
    .close = window_close,
};

int
main()
{
    printf("Starting window test from the main test file\n");
    or_create_window(1024, 768, "Window Name", "com.my.window.app");
    or_surface_setup(&window_listeners, &keyboard_listeners, &pointer_listeners);
    or_start_main_loop();
    or_destroy_window();
    return 0;
}
