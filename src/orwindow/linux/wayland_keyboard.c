//
// Created by pablo on 12/9/24.
//

#include "orwindow/orwindow.h"
#include "wayland_client.h"
#include <stdio.h>
#include <wayland-client-protocol.h>

struct ORKeyboardListeners keyboard_listeners;
struct ORWindowListeners window_listeners;

static uint8_t active_mods = 0;
static uint8_t key_map[255] = {OR_KEY_UNKNOWN};

static void
initialize_key_map() {
    key_map[1] = OR_KEY_ESC;
    key_map[2] = OR_KEY_1;
    key_map[3] = OR_KEY_2;
    key_map[4] = OR_KEY_3;
    key_map[5] = OR_KEY_4;
    key_map[6] = OR_KEY_5;
    key_map[7] = OR_KEY_6;
    key_map[8] = OR_KEY_7;
    key_map[9] = OR_KEY_8;
    key_map[10] = OR_KEY_9;
    key_map[11] = OR_KEY_0;
    key_map[12] = OR_KEY_DASH;
    key_map[13] = OR_KEY_PLUS;
    key_map[14] = OR_KEY_BACKSPACE;
    key_map[15] = OR_KEY_TAB;
    key_map[16] = OR_KEY_Q;
    key_map[17] = OR_KEY_W;
    key_map[18] = OR_KEY_E;
    key_map[19] = OR_KEY_R;
    key_map[20] = OR_KEY_T;
    key_map[21] = OR_KEY_Y;
    key_map[22] = OR_KEY_U;
    key_map[23] = OR_KEY_I;
    key_map[24] = OR_KEY_O;
    key_map[25] = OR_KEY_P;
    key_map[26] = OR_KEY_OPEN_BRACKET;
    key_map[27] = OR_KEY_CLOSE_BRACKET;
    key_map[28] = OR_KEY_ENTER;
    key_map[29] = OR_KEY_LCTRL;
    key_map[30] = OR_KEY_A;
    key_map[31] = OR_KEY_S;
    key_map[32] = OR_KEY_D;
    key_map[33] = OR_KEY_F;
    key_map[34] = OR_KEY_G;
    key_map[35] = OR_KEY_H;
    key_map[36] = OR_KEY_J;
    key_map[37] = OR_KEY_K;
    key_map[38] = OR_KEY_L;
    key_map[39] = OR_KEY_SEMICOLON;
    key_map[40] = OR_KEY_QUOTE;
    key_map[41] = OR_KEY_TILDA;
    key_map[42] = OR_KEY_LSHIFT;
    key_map[43] = OR_KEY_BACKSLASH;
    key_map[44] = OR_KEY_Z;
    key_map[45] = OR_KEY_X;
    key_map[46] = OR_KEY_C;
    key_map[47] = OR_KEY_V;
    key_map[48] = OR_KEY_B;
    key_map[49] = OR_KEY_N;
    key_map[50] = OR_KEY_M;
    key_map[51] = OR_KEY_COMMA;
    key_map[52] = OR_KEY_DOT;
    key_map[53] = OR_KEY_SLASH;
    key_map[54] = OR_KEY_RSHIFT;
    key_map[56] = OR_KEY_LMETA;
    key_map[57] = OR_KEY_SPACEBAR;
    key_map[58] = OR_KEY_CAPS_LOCK;
    key_map[59] = OR_KEY_F1;
    key_map[60] = OR_KEY_F2;
    key_map[61] = OR_KEY_F3;
    key_map[62] = OR_KEY_F4;
    key_map[63] = OR_KEY_F5;
    key_map[64] = OR_KEY_F6;
    key_map[65] = OR_KEY_F7;
    key_map[66] = OR_KEY_F8;
    key_map[67] = OR_KEY_F9;
    key_map[68] = OR_KEY_F10;
    key_map[87] = OR_KEY_F11;
    key_map[88] = OR_KEY_F12;
    key_map[97] = OR_KEY_RCTRL;
    key_map[102] = OR_KEY_HOME;
    key_map[103] = OR_KEY_ARROW_UP;
    key_map[104] = OR_KEY_PAGE_UP;
    key_map[105] = OR_KEY_ARROW_LEFT;
    key_map[106] = OR_KEY_ARROW_RIGHT;
    key_map[107] = OR_KEY_END;
    key_map[108] = OR_KEY_ARROW_DOWN;
    key_map[109] = OR_KEY_PAGE_DOWN;
    key_map[110] = OR_KEY_INSERT;
    key_map[111] = OR_KEY_DELETE;
    // TODO: Finish with the rest of the keys
}

static void
keyboard_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format,
                int fd, uint32_t size) {
    if (keyboard_listeners.load) {
        keyboard_listeners.load();
    }
}

static void
keyboard_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial,
               struct wl_surface *surface, struct wl_array *keys) {
    if (window_listeners.enter) {
        window_listeners.enter();
    }
}

static void
keyboard_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial,
               struct wl_surface *surface) {
    if (window_listeners.leave) {
        window_listeners.leave();
    }
}

static void
keyboard_key(void *data, struct wl_keyboard *keyboard, uint32_t serial,
             uint32_t time, uint32_t key, uint32_t state) {
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED &&
        keyboard_listeners.key_press) {
        keyboard_listeners.key_press(key_map[key], key, time, active_mods);
    } else if (state == WL_KEYBOARD_KEY_STATE_RELEASED &&
               keyboard_listeners.key_release) {
        keyboard_listeners.key_release(key_map[key], key, time, active_mods);
    }
}

static void
keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                   uint32_t mods_depressed, uint32_t mods_latched,
                   uint32_t mods_locked, uint32_t group) {
    active_mods = mods_depressed;
}

void
setup_keyboard(const struct ORWindowListeners *win_listeners,
               const struct ORKeyboardListeners *key_listeners,
               struct InterWayland *wayland) {
    wayland->keyboard = wl_seat_get_keyboard(wayland->seat);
    if (!wayland->keyboard) {
        fprintf(stderr, "Failed to get keyboard from seat.\n");
        return;
    }
    initialize_key_map();

    if (key_listeners) {
        keyboard_listeners = *key_listeners;
    }
    if (win_listeners) {
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
