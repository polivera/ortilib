//
// Created by pablo on 12/20/24.
//

#ifndef ORWINDOW_GAMEPAD_H
#define ORWINDOW_GAMEPAD_H

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
    OR_GAMEPAD_UNKNOWN,
};

enum ORGamepadStick {
    OR_RIGHT_STICK,
    OR_LEFT_STICK,
};

enum ORGamepadStickAxis {
    OR_AXIS_X,
    OR_AXIS_Y,
};

enum ORGamepadTrigger {
    OR_RIGHT_TRIGGER,
    OR_LEFT_TRIGGER,
};

struct ORGamepadListeners {
    void (*button_press)(uint8_t gamepad_id, enum ORGamepadButton button,
                         uint16_t os_button, time_t time);
    void (*button_release)(uint8_t gamepad_id, enum ORGamepadButton button,
                           uint16_t os_button, time_t time);
    void (*axis_motion)(uint8_t gamepad_id, enum ORGamepadStick stick,
                        enum ORGamepadStickAxis axis, uint16_t value,
                        time_t time);
    void (*trigger_motion)(uint8_t gamepad_id, enum ORGamepadTrigger trigger,
                           uint16_t value, time_t time);
    void (*connected)(uint8_t gamepad_id);
    void (*disconnected)(uint8_t gamepad_id);
};

#endif // ORWINDOW_GAMEPAD_H
