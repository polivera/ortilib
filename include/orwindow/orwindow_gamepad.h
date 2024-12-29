//
// Created by pablo on 12/20/24.
//

#ifndef ORWINDOW_GAMEPAD_H
#define ORWINDOW_GAMEPAD_H

#include <orwindow/orwindow_gamepad.h>
#include <stdint.h>
#include <time.h>

#define OR_MAX_GAMEPADS 4

enum ORGamepadID {
    OR_GAMEPAD_0,
    OR_GAMEPAD_1,
    OR_GAMEPAD_2,
    OR_GAMEPAD_3,
};

enum ORGamepadButton {
    OR_GAMEPAD_A,
    OR_GAMEPAD_B,
    OR_GAMEPAD_X,
    OR_GAMEPAD_Y,
    OR_GAMEPAD_LB,
    OR_GAMEPAD_RB,
    OR_GAMEPAD_SELECT,
    OR_GAMEPAD_START,
    OR_GAMEPAD_HOME,
    OR_GAMEPAD_L3,
    OR_GAMEPAD_R3,
    OR_GAMEPAD_DPAD_UP,
    OR_GAMEPAD_DPAD_DOWN,
    OR_GAMEPAD_DPAD_LEFT,
    OR_GAMEPAD_DPAD_RIGHT,
    OR_GAMEPAD_UNKNOWN,
};

enum ORGamepadStick {
    OR_GAMEPAD_RIGHT_STICK,
    OR_GAMEPAD_LEFT_STICK,
};

enum ORGamepadStickAxis {
    OR_AXIS_X,
    OR_AXIS_Y,
};

enum ORGamepadTrigger {
    OR_GAMEPAD_RIGHT_TRIGGER,
    OR_GAMEPAD_LEFT_TRIGGER,
};

struct ORGamepadListeners {
    void (*button_press)(uint8_t gamepad_id, enum ORGamepadButton button,
                         uint16_t os_button, time_t time);
    void (*button_release)(uint8_t gamepad_id, enum ORGamepadButton button,
                           uint16_t os_button, time_t time);
    void (*stick_motion)(uint8_t gamepad_id, enum ORGamepadStick stick,
                         enum ORGamepadStickAxis axis, float value,
                         time_t time);
    void (*trigger_motion)(uint8_t gamepad_id, enum ORGamepadTrigger trigger,
                           float value, time_t time);

    // TODO: can I get information about the connected controller?
    void (*connected)(uint8_t gamepad_id);
    void (*disconnected)(uint8_t gamepad_id);

    // TODO: Implement
    void (*set_rumble)(uint8_t gamepad_id, uint16_t strong_magnitude,
                       uint16_t weak_magnitude, uint16_t duration_ms);
};

#endif // ORWINDOW_GAMEPAD_H
