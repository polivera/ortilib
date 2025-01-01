//
// Created by pablo on 1/1/25.
//

#include "orsound/orsound.h"
#include <math.h>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <stdint.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2

struct pw_data {
    struct pw_main_loop *loop;
    struct pw_stream *stream;
    double phase;
    double frequency;
    double amplitude;
    or_wave_generator wave_fn;
    double duration;
    double elapsed_time;
};

static void
on_process(void *userdata) {
    struct pw_data *data = userdata;
    struct pw_buffer *pipewire_buffer;

    if ((pipewire_buffer = pw_stream_dequeue_buffer(data->stream)) == NULL)
        return;

    struct spa_buffer *sound_buffer = pipewire_buffer->buffer;
    float *dst = sound_buffer->datas[0].data;
    uint32_t n_frames =
        sound_buffer->datas[0].maxsize / sizeof(float) / CHANNELS;

    for (uint32_t i = 0; i < n_frames; i++) {
        float sample;
        if (data->elapsed_time >= data->duration) {
            sample = 0.0f;
        } else {
            if (data->wave_fn) {
                sample = data->amplitude * data->wave_fn(data->phase);
            } else {
                sample = data->amplitude * sin(data->phase);
            }
            data->phase += 2.0 * M_PI * data->frequency / SAMPLE_RATE;
            if (data->phase >= 2.0 * M_PI)
                data->phase -= 2.0 * M_PI;
        }

        for (int c = 0; c < CHANNELS; c++)
            dst[i * CHANNELS + c] = sample;

        data->elapsed_time += 1.0 / SAMPLE_RATE;
    }

    pw_stream_queue_buffer(data->stream, pipewire_buffer);
}

static const struct pw_stream_events stream_events = {
    .process = on_process,
};

static void
play_tone_internal(double frequency, double amplitude, double duration,
                   or_wave_generator wave_function) {
    struct pw_data data = {0};
    data.frequency = frequency;
    data.amplitude = amplitude;
    data.wave_fn = wave_function;
    data.duration = duration;

    pw_init(NULL, NULL);

    data.loop = pw_main_loop_new(NULL);

    struct pw_properties *props =
        pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY,
                          "Playback", PW_KEY_MEDIA_ROLE, "Music", NULL);

    data.stream =
        pw_stream_new_simple(pw_main_loop_get_loop(data.loop), "audio-player",
                             props, &stream_events, &data);

    uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod *params[1];
    params[0] = spa_format_audio_raw_build(
        &b, SPA_PARAM_EnumFormat,
        &SPA_AUDIO_INFO_RAW_INIT(.format = SPA_AUDIO_FORMAT_F32,
                                 .channels = CHANNELS, .rate = SAMPLE_RATE));

    pw_stream_connect(data.stream, PW_DIRECTION_OUTPUT, PW_ID_ANY,
                      PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS |
                          PW_STREAM_FLAG_RT_PROCESS,
                      params, 1);

    pw_main_loop_run(data.loop);

    pw_stream_destroy(data.stream);
    pw_main_loop_destroy(data.loop);
    pw_deinit();
}

void
play_tone(double frequency, double amplitude, enum ORSoundWaveType wave_type,
          double duration) {
    play_tone_internal(frequency, amplitude, duration, NULL);
}

void
play_tone_custom(double frequency, double amplitude, double duration,
                 or_wave_generator wave_function) {
    play_tone_internal(frequency, amplitude, duration, wave_function);
}