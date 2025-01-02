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
    play_tone(261.63, 0.5, OR_WT_NONE, 1.0);
    return 0;
}