//
// Created by pablo on 12/31/24.
//

#include "orsound_internal.h"

#include "sound_pipewire.h"

void
inter_play_tone(double frequency, double amplitude,
                enum ORSoundWaveType wave_type, double duration) {
    inter_pw_play_tone(frequency, amplitude, wave_type, duration);
}

void
inter_play_custom_tone(double frequency, double amplitude, double duration,
                       or_phase_generator wave_function) {}
