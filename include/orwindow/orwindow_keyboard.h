//
// Created by pablo on 11/28/24.
//

#ifndef ORTILIB_ORWINDOW_KEYBOARD_H
#define ORTILIB_ORWINDOW_KEYBOARD_H

#include <stdint.h>

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
  KEY_UNKNOWN = 0,
  KEY_ESC,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_TILDA,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,
  KEY_DASH,
  KEY_PLUS,
  KEY_BACKSPACE,
  KEY_INSERT,
  KEY_HOME,
  KEY_PAGE_UP,
  KEY_TAB,
  KEY_Q,
  KEY_W,
  KEY_E,
  KEY_R,
  KEY_T,
  KEY_Y,
  KEY_U,
  KEY_I,
  KEY_O,
  KEY_P,
  KEY_OPEN_BRACKET,
  KEY_CLOSE_BRACKET,
  KEY_BACKSLASH,
  KEY_DELETE,
  KEY_END,
  KEY_PAGE_DOWN,
  KEY_CAPS_LOCK,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_SEMICOLON,
  KEY_QUOTE,
  KEY_ENTER,
  KEY_LSHIFT,
  KEY_Z,
  KEY_X,
  KEY_C,
  KEY_V,
  KEY_B,
  KEY_N,
  KEY_M,
  KEY_COMMA,
  KEY_DOT,
  KEY_SLASH,
  KEY_RSHIFT,
  KEY_ARROW_UP,
  KEY_LCTRL,
  KEY_LSUPER,
  KEY_LMETA,
  KEY_SPACEBAR,
  KEY_RMETA,
  KEY_RSUPER,
  KEY_RMENU,
  KEY_RCTRL,
  KEY_ARROW_LEFT,
  KEY_ARROW_DOWN,
  KEY_ARROW_RIGHT,
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
   * @brief Callback function for key press events
   * Called when a key is initially pressed down
   */
  void (*key_press)(enum ORKeys code, uint8_t os_code, uint16_t time, uint8_t mod);

  /**
   * @brief Callback function for key release events
   * Called when a previously pressed key is released
   */
  void (*key_release)(enum ORKeys code, uint8_t os_code, uint16_t time, uint8_t mod);
};

#endif //ORTILIB_ORWINDOW_KEYBOARD_H
