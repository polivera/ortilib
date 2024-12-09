//
// Created by pablo on 12/9/24.
//

#include <stdio.h>
#include <wayland-client-protocol.h>
#include "orwindow/orwindow.h"
#include "wayland_client.h"

struct ORKeyboardListeners keyboard_listeners;
struct ORWindowListeners window_listeners;

static uint8_t active_mods = 0;
static uint8_t key_map[255] = {KEY_UNKNOWN};

static void initialize_key_map()
{
    key_map[1] = KEY_ESC;
    key_map[2] = KEY_1;
    key_map[3] = KEY_2;
    key_map[4] = KEY_3;
    key_map[5] = KEY_4;
    key_map[6] = KEY_5;
    key_map[7] = KEY_6;
    key_map[8] = KEY_7;
    key_map[9] = KEY_8;
    key_map[10] = KEY_9;
    key_map[11] = KEY_0;
    key_map[12] = KEY_DASH;
    key_map[13] = KEY_PLUS;
    key_map[14] = KEY_BACKSPACE;
    key_map[15] = KEY_TAB;
    key_map[16] = KEY_Q;
    key_map[17] = KEY_W;
    key_map[18] = KEY_E;
    key_map[19] = KEY_R;
    key_map[20] = KEY_T;
    key_map[21] = KEY_Y;
    key_map[22] = KEY_U;
    key_map[23] = KEY_I;
    key_map[24] = KEY_O;
    key_map[25] = KEY_P;
    key_map[26] = KEY_OPEN_BRACKET;
    key_map[27] = KEY_CLOSE_BRACKET;
    key_map[28] = KEY_ENTER;
    key_map[29] = KEY_LCTRL;
    key_map[30] = KEY_A;
    key_map[31] = KEY_S;
    key_map[32] = KEY_D;
    key_map[33] = KEY_F;
    key_map[34] = KEY_G;
    key_map[35] = KEY_H;
    key_map[36] = KEY_J;
    key_map[37] = KEY_K;
    key_map[38] = KEY_L;
    key_map[39] = KEY_SEMICOLON;
    key_map[40] = KEY_QUOTE;
    key_map[41] = KEY_TILDA;
    key_map[42] = KEY_LSHIFT;
    key_map[43] = KEY_BACKSLASH;
    key_map[44] = KEY_Z;
    key_map[45] = KEY_X;
    key_map[46] = KEY_C;
    key_map[47] = KEY_V;
    key_map[48] = KEY_B;
    key_map[49] = KEY_N;
    key_map[50] = KEY_M;
    key_map[51] = KEY_COMMA;
    key_map[52] = KEY_DOT;
    key_map[53] = KEY_SLASH;
    key_map[54] = KEY_RSHIFT;
    key_map[56] = KEY_LMETA;
    key_map[57] = KEY_SPACEBAR;
    key_map[58] = KEY_CAPS_LOCK;
    key_map[59] = KEY_F1;
    key_map[60] = KEY_F2;
    key_map[61] = KEY_F3;
    key_map[62] = KEY_F4;
    key_map[63] = KEY_F5;
    key_map[64] = KEY_F6;
    key_map[65] = KEY_F7;
    key_map[66] = KEY_F8;
    key_map[67] = KEY_F9;
    key_map[68] = KEY_F10;
    key_map[87] = KEY_F11;
    key_map[88] = KEY_F12;
    key_map[97] = KEY_RCTRL;
    key_map[102] = KEY_HOME;
    key_map[103] = KEY_ARROW_UP;
    key_map[104] = KEY_PAGE_UP;
    key_map[105] = KEY_ARROW_LEFT;
    key_map[106] = KEY_ARROW_RIGHT;
    key_map[107] = KEY_END;
    key_map[108] = KEY_ARROW_DOWN;
    key_map[109] = KEY_PAGE_DOWN;
    key_map[110] = KEY_INSERT;
    key_map[111] = KEY_DELETE;
    // TODO: Finish with the rest of the keys
}

static void keyboard_keymap(void* data, struct wl_keyboard* keyboard,
                            uint32_t format, int fd, uint32_t size)
{
    if (keyboard_listeners.load)
    {
        keyboard_listeners.load();
    }
}

static void keyboard_enter(void* data, struct wl_keyboard* keyboard,
                           uint32_t serial, struct wl_surface* surface,
                           struct wl_array* keys)
{
    if (window_listeners.enter)
    {
        window_listeners.enter();
    }
}

static void keyboard_leave(void* data, struct wl_keyboard* keyboard,
                           uint32_t serial, struct wl_surface* surface)
{
    if (window_listeners.leave)
    {
        window_listeners.leave();
    }
}

static void keyboard_key(void* data, struct wl_keyboard* keyboard,
                         uint32_t serial, uint32_t time, uint32_t key,
                         uint32_t state)
{
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED && keyboard_listeners.key_press)
    {
        keyboard_listeners.key_press(key_map[key], key, time, active_mods);
    }
    else if (state == WL_KEYBOARD_KEY_STATE_RELEASED &&
        keyboard_listeners.key_release)
    {
        keyboard_listeners.key_release(key_map[key], key, time, active_mods);
    }
}

static void keyboard_modifiers(void* data, struct wl_keyboard* keyboard,
                               uint32_t serial, uint32_t mods_depressed,
                               uint32_t mods_latched, uint32_t mods_locked,
                               uint32_t group)
{
    active_mods = mods_depressed;
}

void setup_keyboard(
    const struct ORWindowListeners* win_listeners,
    const struct ORKeyboardListeners* key_listeners, struct InterWayland* wayland)
{
    wayland->keyboard = wl_seat_get_keyboard(wayland->seat);
    if (!wayland->keyboard)
    {
        fprintf(stderr, "Failed to get keyboard from seat.\n");
        return;
    }
    initialize_key_map();

    if (key_listeners)
    {
        keyboard_listeners = *key_listeners;
    }
    if (win_listeners)
    {
        window_listeners = *win_listeners;
    }
    static struct wl_keyboard_listener wl_listeners = {
        .keymap = keyboard_keymap,
        .enter = keyboard_enter,
        .leave = keyboard_leave,
        .modifiers = keyboard_modifiers,
        .key = keyboard_key,
    };
    wl_keyboard_add_listener(wayland->keyboard, &wl_listeners, 0);
}
