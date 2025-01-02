//
// Created by pablo on 1/1/25.
//

#include "sound_pipewire.h"
#include <math.h>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>

// If this is made configurable at some point it should be between
// 44100 and 48000
#define SAMPLE_RATE 44100
#define M_PI 3.14159265358979323846

struct sound_data {
    struct pw_main_loop *loop;
    struct pw_stream *stream;
    or_phase_generator phase_generator;
    double phase;
    double frequency;
    double amplitude;
};

static void
on_process(void *data) {
    struct sound_data *user_data = data;
    struct pw_buffer *pipewire_buff;
    if ((pipewire_buff = pw_stream_dequeue_buffer(user_data->stream)) == NULL) {
        fprintf(stderr, "pipewire buffer empty\n");
        return;
    }

    const struct spa_buffer *spa_buff = pipewire_buff->buffer;
    // Even though user_data fields are double to reduce the accumulation of
    // errors, this is float because it is more than enough for amplitude and
    // its industry standard
    float *ptr_sample_writer = spa_buff->datas[0].data;
    const uint32_t needed_samples = spa_buff->datas[0].maxsize / sizeof(float);

    // Generate samples
    for (uint32_t i = 0; i < needed_samples; i++) {
        ptr_sample_writer[i] =
            user_data->amplitude * user_data->phase_generator(user_data->phase);
        user_data->phase += 2.0 * M_PI * user_data->frequency / SAMPLE_RATE;
        // Wraps the phase around when it exceeds 2π to prevent floating-point
        // errors from accumulating
        if (user_data->phase >= 2.0 * M_PI)
            user_data->phase -= 2.0 * M_PI;
    }

    spa_buff->datas[0].chunk->offset = 0;
    spa_buff->datas[0].chunk->stride = sizeof(float);
    spa_buff->datas[0].chunk->size = needed_samples * sizeof(float);

    pw_stream_queue_buffer(user_data->stream, pipewire_buff);
}

static const struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .process = on_process,
};

static double
generate_square_wave(double phase) {
    return phase < M_PI ? 1.0f : -1.0f;
}

static double
generate_triangle_wave(double phase) {
    const double value = (2.0 * phase / (2.0 * M_PI)) - 0.5;
    return 2.0 * (fabs(value) - 0.25);
}

static double
generate_sawtooth_wave(double phase) {
    return (phase / M_PI) - 1.0;
}

static double
generate_sine_wave(double phase) {
    return sin(phase);
}

// Example of a composite wave function
static double
composite_wave(double phase) {
    // Combine fundamental frequency with harmonics
    double fundamental = sin(phase); // First harmonic
    double second_harmonic =
        0.5 * sin(2 * phase); // Second harmonic at half amplitude
    double third_harmonic =
        0.25 * sin(3 * phase); // Third harmonic at quarter amplitude

    return fundamental + second_harmonic + third_harmonic;
}

// Example of how to create a custom wave shape using Fourier series
static double
bell_like_tone(double phase) {
    return sin(phase) +            // fundamental
           0.5 * sin(2 * phase) +  // 2nd harmonic
           0.33 * sin(3 * phase) + // 3rd harmonic
           0.25 * sin(4 * phase) + // 4th harmonic
           0.16 * sin(5 * phase) + // 5th harmonic
           0.13 * sin(6 * phase);  // 6th harmonic
}

void
inter_pw_play_tone(double frequency, double amplitude,
                   enum ORSoundWaveType wave_type, double duration) {
    struct sound_data data = {0};
    struct pw_properties *props;
    uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod *params[1];
    struct spa_audio_info_raw info = {
        .format = SPA_AUDIO_FORMAT_F32,
        .channels = 1,
        .rate = SAMPLE_RATE,
    };

    data.frequency = frequency;
    data.amplitude = amplitude;
    data.phase_generator = composite_wave;

    pw_init(NULL, NULL);

    data.loop = pw_main_loop_new(NULL);
    if (data.loop == NULL) {
        fprintf(stderr, "Failed to create main loop\n");
        return;
    }

    props = pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY,
                              "Playback", PW_KEY_MEDIA_ROLE, "Music", NULL);

    data.stream =
        pw_stream_new_simple(pw_main_loop_get_loop(data.loop), "audio-tone",
                             props, &stream_events, &data);

    params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &info);

    if (pw_stream_connect(data.stream, PW_DIRECTION_OUTPUT, PW_ID_ANY,
                          PW_STREAM_FLAG_AUTOCONNECT |
                              PW_STREAM_FLAG_MAP_BUFFERS |
                              PW_STREAM_FLAG_RT_PROCESS,
                          params, 1) < 0) {
        fprintf(stderr, "Failed to connect stream\n");
        return;
    }

    pw_main_loop_run(data.loop);

    pw_stream_destroy(data.stream);
    pw_main_loop_destroy(data.loop);
    pw_deinit();
}