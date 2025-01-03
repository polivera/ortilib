//
// Created by pablo on 12/31/24.
//

#ifndef ORSOUND_INTERNAL_H
#define ORSOUND_INTERNAL_H

#include "orarena/orarena.h"
#include "orsound/orsound.h"

enum ORSoundStatus
inter_or_system_init(const struct ORSoundConfig *config, struct ORArena *arena);

void
inter_or_system_shutdown(void);

struct ORSound *
inter_or_create_tone(double frequency, double amplitude,
                     or_wave_generator wave_fn, struct ORArena *arena);

enum ORSoundStatus
inter_or_sound_destroy(struct ORSound *handle, struct ORArena *arena);

enum ORSoundStatus
inter_or_sound_play(struct ORSound *handle);
enum ORSoundStatus
inter_or_sound_stop(struct ORSound *handle);
enum ORSoundStatus
inter_or_sound_pause(struct ORSound *handle);

enum ORSoundStatus
inter_or_sound_set_frequency(struct ORSound *handle, double frequency);
enum ORSoundStatus
inter_or_sound_set_amplitude(struct ORSound *handle, double amplitude);
enum ORSoundStatus
inter_or_sound_set_wave_function(struct ORSound *handle,
                                 or_wave_generator wave_fn);
enum ORSoundStatus
inter_or_sound_add_effect(struct ORSound *handle, or_effect_generator effect_fn,
                          void *effect_data);

#endif // ORSOUND_INTERNAL_H
