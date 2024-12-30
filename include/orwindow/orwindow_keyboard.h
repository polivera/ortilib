//
// Created by pablo on 11/28/24.
//

#ifndef ORTILIB_ORWINDOW_KEYBOARD_H
#define ORTILIB_ORWINDOW_KEYBOARD_H

#include <stdint.h>
#include <time.h>

/**
 * @enum ORKeys
 * @brief Enumeration of keyboard key codes for cross-platform keyboard handling
 *
 * This enum maps physical keyboard keys to logical key codes, providing a
 * platform-independent way to handle keyboard input. The values are arranged
 * in groups following a typical keyboard layout:
 * - Special keys (ESC, Function keys)
 * - Number row
 * - Letter rows (QWERTY layout)
 * - Modifier keys (Shift, Ctrl, etc.)
 * - Navigation keys (arrows, etc.)
 */
enum ORKeys {
    OR_KEY_UNKNOWN = 0,
    OR_KEY_ESC,
    OR_KEY_F1,
    OR_KEY_F2,
    OR_KEY_F3,
    OR_KEY_F4,
    OR_KEY_F5,
    OR_KEY_F6,
    OR_KEY_F7,
    OR_KEY_F8,
    OR_KEY_F9,
    OR_KEY_F10,
    OR_KEY_F11,
    OR_KEY_F12,
    OR_KEY_TILDA,
    OR_KEY_1,
    OR_KEY_2,
    OR_KEY_3,
    OR_KEY_4,
    OR_KEY_5,
    OR_KEY_6,
    OR_KEY_7,
    OR_KEY_8,
    OR_KEY_9,
    OR_KEY_0,
    OR_KEY_DASH,
    OR_KEY_PLUS,
    OR_KEY_BACKSPACE,
    OR_KEY_INSERT,
    OR_KEY_HOME,
    OR_KEY_PAGE_UP,
    OR_KEY_TAB,
    OR_KEY_Q,
    OR_KEY_W,
    OR_KEY_E,
    OR_KEY_R,
    OR_KEY_T,
    OR_KEY_Y,
    OR_KEY_U,
    OR_KEY_I,
    OR_KEY_O,
    OR_KEY_P,
    OR_KEY_OPEN_BRACKET,
    OR_KEY_CLOSE_BRACKET,
    OR_KEY_BACKSLASH,
    OR_KEY_DELETE,
    OR_KEY_END,
    OR_KEY_PAGE_DOWN,
    OR_KEY_CAPS_LOCK,
    OR_KEY_A,
    OR_KEY_S,
    OR_KEY_D,
    OR_KEY_F,
    OR_KEY_G,
    OR_KEY_H,
    OR_KEY_J,
    OR_KEY_K,
    OR_KEY_L,
    OR_KEY_SEMICOLON,
    OR_KEY_QUOTE,
    OR_KEY_ENTER,
    OR_KEY_LSHIFT,
    OR_KEY_Z,
    OR_KEY_X,
    OR_KEY_C,
    OR_KEY_V,
    OR_KEY_B,
    OR_KEY_N,
    OR_KEY_M,
    OR_KEY_COMMA,
    OR_KEY_DOT,
    OR_KEY_SLASH,
    OR_KEY_RSHIFT,
    OR_KEY_ARROW_UP,
    OR_KEY_LCTRL,
    OR_KEY_LSUPER,
    OR_KEY_LMETA,
    OR_KEY_SPACEBAR,
    OR_KEY_RMETA,
    OR_KEY_RSUPER,
    OR_KEY_RMENU,
    OR_KEY_RCTRL,
    OR_KEY_ARROW_LEFT,
    OR_KEY_ARROW_DOWN,
    OR_KEY_ARROW_RIGHT,
};

/**
 * @struct ORKeyboardListeners
 * @brief Function pointers for keyboard event callbacks
 *
 * This structure defines the interface for keyboard event handling.
 * Applications can register callbacks for key press and release events.
 *
 * @param code The logical key code from ORKeys enum
 * @param os_code The raw OS-specific key code
 * @param time Timestamp of the event in milliseconds
 * @param mod Modifier key bit flags (following Wayland's modifier mask):
 *        1 = Shift
 *        2 = Caps Lock
 *        4 = Ctrl
 *        8 = Alt
 *        16 = Num Lock
 *        32 = Meta
 *        64 = Super
 *        128 = Hyper
 *        (Values add up when multiple modifiers are pressed)
 */
struct ORKeyboardListeners {
    /**
     * @brief Called on keyboard listener initialization.
     */
    void (*load)();
    /**
     * @brief Callback function for key press events
     * Called when a key is initially pressed down
     */
    void (*key_press)(enum ORKeys code, uint8_t os_code, time_t time,
                      uint8_t mod);

    /**
     * @brief Callback function for key release events
     * Called when a previously pressed key is released
     */
    void (*key_release)(enum ORKeys code, uint8_t os_code, time_t time,
                        uint8_t mod);
};

#endif // ORTILIB_ORWINDOW_KEYBOARD_H
