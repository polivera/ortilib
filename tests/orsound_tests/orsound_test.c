//
// Created by pablo on 1/1/25.
//

#include "orsound/orsound.h"

#include <stdio.h>

double
my_generator(double phase) {
    printf("Phase %f\n", phase);
    return 0.0f;
}

int
main() {
    struct ORArena *arena = arena_create(1024 * 1024);
    const struct ORSoundConfig sound_config = {.sample_rate = 44100,
                                               .buffer_size = 1024,
                                               .priority = OR_ROLE_BACKGROUND,
                                               .format = OR_FORMAT_S16};

    or_system_init(&sound_config, arena);
}