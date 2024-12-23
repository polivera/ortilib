//
// Created by pablo on 12/20/24.
//

#ifndef ORWINDOW_GAMEPAD_H
#define ORWINDOW_GAMEPAD_H

#include <stdint.h>
#include <time.h>

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
    OR_GAMEPAD_R3
};

struct ORGamepadListeners {
    void (*button_press)(uint8_t gamepad_id, uint16_t button, time_t time);
    void (*button_release)(uint8_t gamepad_id, uint16_t button, time_t time);
    void (*axis_motion)(uint8_t gamepad_id, uint8_t axis, int16_t value,
                        time_t time);
    void (*connected)(uint8_t gamepad_id);
    void (*disconnected)(uint8_t gamepad_id);
};

#endif // ORWINDOW_GAMEPAD_H
