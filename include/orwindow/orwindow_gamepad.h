//
// Created by pablo on 12/20/24.
//

#ifndef ORWINDOW_GAMEPAD_H
#define ORWINDOW_GAMEPAD_H

#include <orwindow/orwindow_gamepad.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/**
 * Maximum number of gamepads that can be simultaneously connected
 * This limits gamepad detection and event handling to 4 controllers
 */
#define OR_MAX_GAMEPADS 4

/**
 * @enum ORGamepadID
 * @brief Identifies individual gamepad slots/ports
 *
 * Used to track and identify up to 4 different connected gamepads
 */
enum ORGamepadID {
    OR_GAMEPAD_A,
    OR_GAMEPAD_B,
    OR_GAMEPAD_C,
    OR_GAMEPAD_D,
};

/**
 * @enum ORGamepadButton
 * @brief Standard gamepad button mappings
 *
 * Maps physical gamepad buttons to logical button identifiers.
 * Follows a standard Xbox-style controller layout.
 */
enum ORGamepadButton {
    OR_GAMEPAD_BUTTON_A,
    OR_GAMEPAD_BUTTON_B,
    OR_GAMEPAD_BUTTON_X,
    OR_GAMEPAD_BUTTON_Y,
    OR_GAMEPAD_BUTTON_LB,
    OR_GAMEPAD_BUTTON_RB,
    OR_GAMEPAD_BUTTON_SELECT,
    OR_GAMEPAD_BUTTON_START,
    OR_GAMEPAD_BUTTON_HOME,
    OR_GAMEPAD_BUTTON_L3,
    OR_GAMEPAD_BUTTON_R3,
    OR_GAMEPAD_DPAD_UP,
    OR_GAMEPAD_DPAD_DOWN,
    OR_GAMEPAD_DPAD_LEFT,
    OR_GAMEPAD_DPAD_RIGHT,
    OR_GAMEPAD_BUTTON_UNKNOWN,
};

/**
 * @enum ORGamepadStick
 * @brief Identifies analog sticks
 *
 * Used to differentiate between left and right analog sticks
 * for stick motion events
 */
enum ORGamepadStick {
    OR_GAMEPAD_RIGHT_STICK,
    OR_GAMEPAD_LEFT_STICK,
};

/**
 * @enum ORGamepadStickAxis
 * @brief Defines analog stick axes
 *
 * Identifies which axis of motion is being reported for analog sticks
 */
enum ORGamepadStickAxis {
    OR_GAMEPAD_AXIS_X,
    OR_GAMEPAD_AXIS_Y,
};

/**
 * @enum ORGamepadTrigger
 * @brief Identifies analog triggers
 *
 * Used to differentiate between left and right analog triggers
 * for trigger motion events
 */
enum ORGamepadTrigger {
    OR_GAMEPAD_RIGHT_TRIGGER,
    OR_GAMEPAD_LEFT_TRIGGER,
};

/**
 * @struct ORGamepadListeners
 * @brief Function pointers for gamepad event callbacks
 *
 * This structure defines the interface for gamepad event handling.
 * Applications can register callbacks for button, stick, trigger,
 * and connection events.
 */
struct ORGamepadListeners {
    /**
     * @brief Called when a gamepad button is pressed
     * @param gamepad_id Identifier for which gamepad triggered the event
     * @param button The logical button identifier
     * @param os_button The raw OS-specific button code
     * @param time Timestamp of the event
     */
    void (*button_press)(enum ORGamepadID gamepad_id,
                         enum ORGamepadButton button, uint16_t os_button,
                         time_t time);

    /**
     * @brief Called when a gamepad button is released
     * @param gamepad_id Identifier for which gamepad triggered the event
     * @param button The logical button identifier
     * @param os_button The raw OS-specific button code
     * @param time Timestamp of the event
     */
    void (*button_release)(enum ORGamepadID gamepad_id,
                           enum ORGamepadButton button, uint16_t os_button,
                           time_t time);

    /**
     * @brief Called when an analog stick position changes
     * @param gamepad_id Identifier for which gamepad triggered the event
     * @param stick Which analog stick moved
     * @param axis Which axis the stick moved along
     * @param value New position value, normalized to [-1.0, 1.0]
     * @param time Timestamp of the event
     */
    void (*stick_motion)(enum ORGamepadID gamepad_id, enum ORGamepadStick stick,
                         enum ORGamepadStickAxis axis, float value,
                         time_t time);

    /**
     * @brief Called when an analog trigger position changes
     * @param gamepad_id Identifier for which gamepad triggered the event
     * @param trigger Which trigger moved
     * @param value New position value, normalized to [0.0, 1.0]
     * @param time Timestamp of the event
     */
    void (*trigger_motion)(enum ORGamepadID gamepad_id,
                           enum ORGamepadTrigger trigger, float value,
                           time_t time);

    /**
     * @brief Called when a gamepad is connected
     * @param gamepad_id Identifier assigned to the newly connected gamepad
     */
    void (*connected)(enum ORGamepadID gamepad_id);

    /**
     * @brief Called when a gamepad is disconnected
     * @param gamepad_id Identifier of the disconnected gamepad
     */
    void (*disconnected)(enum ORGamepadID gamepad_id);
};

bool
or_gamepad_set_rumble(enum ORGamepadID gamepad_id, float strong_magnitude,
                      float weak_magnitude);

#endif // ORWINDOW_GAMEPAD_H
