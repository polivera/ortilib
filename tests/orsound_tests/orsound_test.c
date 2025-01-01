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
    play_tone_custom(2.4, 3.4, 2.2, my_generator);
    return 0;
}