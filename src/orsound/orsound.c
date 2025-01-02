//
// Created by pablo on 12/31/24.
//

#include "orsound/orsound.h"

#include "orsound_internal.h"

#include <stdio.h>

void
play_tone(double frequency, double amplitude, enum ORSoundWaveType wave_type,
          double duration) {
    inter_play_tone(frequency, amplitude, wave_type, duration);
}

void
play_tone_custom(double frequency, double amplitude, double duration,
                 or_phase_generator wave_function) {
    printf("Playing custom tone %f Hz\n", frequency);
}