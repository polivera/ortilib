//
// Created by pablo on 12/31/24.
//

#ifndef ORSOUND_H
#define ORSOUND_H

enum ORSoundWaveType {
    OR_WT_NONE,
};

void
play_tone(double frequency, double amplitude, enum ORSoundWaveType wave_type,
          double duration);

typedef double (*or_phase_generator)(double phase);

void
play_tone_custom(double frequency, double amplitude, double duration,
                 or_phase_generator wave_function);

#endif // ORSOUND_H