//
// Created by pablo on 12/31/24.
//

#include "orsound/orsound.h"
#include "orsound_internal.h"

static struct ORArena *inter_arena;

enum ORSoundStatus
or_system_init(const struct ORSoundConfig *config, struct ORArena *arena) {
    inter_arena = arena;
    return inter_or_system_init(config, inter_arena);
}

void
or_system_shutdown(void) {
    inter_or_system_shutdown();
}

struct ORSound *
or_create_tone(const double frequency, const double amplitude,
               const or_wave_generator wave_fn) {
    return inter_or_create_tone(frequency, amplitude, wave_fn, inter_arena);
}

enum ORSoundStatus
or_sound_destroy(struct ORSound *handle) {
    return inter_or_sound_destroy(handle, inter_arena);
}

enum ORSoundStatus
or_sound_play(struct ORSound *handle) {
    return inter_or_sound_play(handle);
}

enum ORSoundStatus
or_sound_stop(struct ORSound *handle) {
    return inter_or_sound_stop(handle);
}

enum ORSoundStatus
or_sound_pause(struct ORSound *handle) {
    return inter_or_sound_pause(handle);
}

// Parameter control
enum ORSoundStatus
or_sound_set_frequency(struct ORSound *handle, const double frequency) {
    return inter_or_sound_set_frequency(handle, frequency);
}

enum ORSoundStatus
or_sound_set_amplitude(struct ORSound *handle, const double amplitude) {
    return inter_or_sound_set_amplitude(handle, amplitude);
}

enum ORSoundStatus
or_sound_set_wave_function(struct ORSound *handle,
                           const or_wave_generator wave_fn) {
    return inter_or_sound_set_wave_function(handle, wave_fn);
}

enum ORSoundStatus
or_sound_add_effect(struct ORSound *handle, const or_effect_generator effect_fn,
                    void *effect_data) {
    return inter_or_sound_add_effect(handle, effect_fn, effect_data);
}