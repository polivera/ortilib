//
// Created by pablo on 11/28/24.
//

#ifndef ORTILIB_INCLUDE_ORWINDOW_ORWINDOW_POINTER_H
#define ORTILIB_INCLUDE_ORWINDOW_ORWINDOW_POINTER_H

#include <stdint.h>

enum ORPointerButton {
  OR_CLICK_UNKNOWN = 0,
  OR_CLICK_LEFT,
  OR_CLICK_RIGHT,
  OR_CLICK_MIDDLE,
  OR_CLICK_CUSTOM_A,
  OR_CLICK_CUSTOM_B,
  OR_CLICK_CUSTOM_C,
};

struct ORPointerListeners {
/**
   * Called when the pointer enters the window area.
   *
   * @param point_x X coordinate of the pointer when entering.
   * @param point_y Y coordinate of the pointer when entering.
   */
  void (*enter)(uint16_t point_x, uint16_t point_y);

  /**
   * Called when the pointer leaves the window area.
   */
  void (*leave)();

  /**
   * Called when the pointer moves within the window area.
   *
   * @param point_x Current X coordinate of the pointer.
   * @param point_y Current Y coordinate of the pointer.
   */
  void (*move)(uint16_t point_x, uint16_t point_y);

  /**
   * Called when a pointer button is pressed.
   *
   * @param button Button identifier (see button definitions above).
   */
  void (*button_press)(enum ORPointerButton button, uint32_t time);

  /**
   * Called when a pointer button is released.
   *
   * @param button Button identifier (see button definitions above).
   */
  void (*button_release)(enum ORPointerButton button, uint32_t time);

  /**
   * Called when an X scroll
   */
  void (*scroll_x)(int32_t value, uint32_t time);

  /**
   * Called when an Y scroll
   */
  void (*scroll_y)(int32_t value, uint32_t time);
};

#endif //ORTILIB_INCLUDE_ORWINDOW_ORWINDOW_POINTER_H