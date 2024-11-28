//
// Created by pablo on 11/27/24.
//

#ifndef ORTILIB_ORWINDOW_INTERNAL_H
#define ORTILIB_ORWINDOW_INTERNAL_H

#include "orwindow/orwindow.h"

struct ORBitmap
inter_create_bitmap(uint16_t width, uint16_t height);

int8_t
inter_create_window(struct ORBitmap *bitmap, const char *window_name, const char *process_name);

#endif //ORTILIB_ORWINDOW_INTERNAL_H
