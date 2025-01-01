//
// Created by pablo on 12/31/24.
//

#include "orsound/orsound.h"
#include <stdio.h>

void
play_tone(double frequency, double amplitude, enum ORSoundWaveType wave_type,
          double duration) {
    printf("Playing tone %f Hz\n", frequency);
}

void
play_tone_custom(double frequency, double amplitude, double duration,
                 or_wave_generator wave_function) {
    printf("Playing custom tone %f Hz\n", frequency);
}