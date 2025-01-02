//
// Created by pablo on 1/2/25.
//

#ifndef SOUND_PIPEWIRE_H
#define SOUND_PIPEWIRE_H

#include "orsound/orsound.h"

void
inter_pw_play_tone(double frequency, double amplitude,
                   enum ORSoundWaveType wave_type, double duration);

#endif // SOUND_PIPEWIRE_H
