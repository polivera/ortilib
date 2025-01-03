//
// Created by pablo on 12/31/24.
//

#include "orsound_internal.h"
#include "orarena/orarena.h"

#include <stdio.h>

enum ORSoundStatus
inter_or_system_init(const struct ORSoundConfig *config,
                     struct ORArena *arena) {
    printf("Implement or_system_init\n");
    return OR_SOUND_ERROR_SYSTEM;
}

void
inter_or_system_shutdown(void) {
    printf("Implement or_system_shutdown\n");
}

struct ORSound *
inter_or_create_tone(const double frequency, const double amplitude,
                     const or_wave_generator wave_fn, struct ORArena *arena) {
    struct ORSound *sound = arena_alloc(arena, sizeof(struct ORSound));
    printf("Implement or_create_tone\n");
    return sound;
}

enum ORSoundStatus
inter_or_sound_destroy(struct ORSound *handle, struct ORArena *arena) {
    printf("Implement or_sound_destroy\n");
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_play(struct ORSound *handle) {
    printf("Implement or_sound_play\n");
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_stop(struct ORSound *handle) {
    printf("Implement or_sound_stop\n");
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_pause(struct ORSound *handle) {
    printf("Implement or_sound_pause\n");
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_set_frequency(struct ORSound *handle, double frequency) {
    printf("Implement or_sound_set_frequency\n");
    handle->frequency = frequency;
    // TODO: Restart Sound?
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_set_amplitude(struct ORSound *handle, double amplitude) {
    printf("Implement or_sound_set_amplitude\n");
    handle->amplitude = amplitude;
    // TODO: Restart Sound?
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_set_wave_function(struct ORSound *handle,
                                 or_wave_generator wave_fn) {
    printf("Implement or_sound_set_wave_function\n");
    return OR_SOUND_ERROR_SYSTEM;
}

enum ORSoundStatus
inter_or_sound_add_effect(struct ORSound *handle, or_effect_generator effect_fn,
                          void *effect_data) {
    printf("Implement or_sound_add_effect\n");
    return OR_SOUND_ERROR_SYSTEM;
}