//
// Created by pablo on 12/31/24.
//

#ifndef ORSOUND_H
#define ORSOUND_H

#include "orarena/orarena.h"
#include <stdint.h>

// Function types for wave and effect generation
typedef double (*or_wave_generator)(double phase);
typedef double (*or_effect_generator)(double sample, void *effect_data);

// Audio format options
enum ORAudioFormat { OR_FORMAT_F32, OR_FORMAT_F32_BE, OR_FORMAT_S16 };

// Additional state flags
enum ORSoundState {
    OR_STATE_UNINITIALIZED,
    OR_STATE_READY,
    OR_STATE_PLAYING,
    OR_STATE_PAUSED,
    OR_STATE_BUFFERING,
    OR_STATE_ERROR
};

// Channel configuration
enum ORChannelConfig { OR_CHANNEL_MONO = 1, OR_CHANNEL_STEREO = 2 };

enum ORSoundPriority {
    OR_ROLE_BACKGROUND, // Lowest priority (background music)
    OR_ROLE_FX,         // Medium priority (game sounds)
    OR_ROLE_ALERT,      // High priority (UI sounds, alerts)
    OR_ROLE_CRITICAL    // Highest (emergency alerts, accessibility)
};

struct ORSound {
    // Basic properties
    uint32_t sample_rate;
    uint32_t buffer_size;
    uint8_t priority;
    enum ORChannelConfig channel_config;

    // Wave generation
    double frequency; // In Hz
    double amplitude; // 0.0 to 1.0
    double phase;     // Current phase
    or_wave_generator wave_function;

    // Effect chain
    or_effect_generator effect_function;
    void *effect_data;

    // State tracking
    enum ORSoundState sound_state;
};

// System configuration
struct ORSoundConfig {
    uint32_t sample_rate;          // Default: 44100
    uint32_t buffer_size;          // In frames
    enum ORSoundPriority priority; // System-specific priority
    enum ORAudioFormat format;
};

// Status codes for operations
enum ORSoundStatus {
    OR_SOUND_SUCCESS = 0,
    OR_SOUND_ERROR_INVALID_HANDLE,
    OR_SOUND_ERROR_INVALID_PARAM,
    OR_SOUND_ERROR_SYSTEM,
    OR_SOUND_ERROR_MEMORY
};

// System configuration
enum ORSoundStatus
or_system_init(const struct ORSoundConfig *config, struct ORArena *arena);
void
or_system_shutdown(void);

// Creation and destruction
struct ORSound *
or_create_tone(double frequency, double amplitude, or_wave_generator wave_fn);
enum ORSoundStatus
or_sound_destroy(struct ORSound *handle);

// Playback control
enum ORSoundStatus
or_sound_play(struct ORSound *handle);
enum ORSoundStatus
or_sound_stop(struct ORSound *handle);
enum ORSoundStatus
or_sound_pause(struct ORSound *handle);

// Parameter control
enum ORSoundStatus
or_sound_set_frequency(struct ORSound *handle, double frequency);
enum ORSoundStatus
or_sound_set_amplitude(struct ORSound *handle, double amplitude);
enum ORSoundStatus
or_sound_set_wave_function(struct ORSound *handle, or_wave_generator wave_fn);
enum ORSoundStatus
or_sound_add_effect(struct ORSound *handle, or_effect_generator effect_fn,
                    void *effect_data);

#endif // ORSOUND_H