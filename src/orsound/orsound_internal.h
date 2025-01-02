//
// Created by pablo on 12/31/24.
//

#ifndef ORSOUND_INTERNAL_H
#define ORSOUND_INTERNAL_H

#include "orsound/orsound.h"

void
inter_play_tone(double frequency, double amplitude,
                enum ORSoundWaveType wave_type, double duration);

void
inter_play_custom_tone(double frequency, double amplitude, double duration,
                       or_phase_generator wave_function);

#endif // ORSOUND_INTERNAL_H
