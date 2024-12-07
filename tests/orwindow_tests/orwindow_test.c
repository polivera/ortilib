//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"

int
main() {
    printf("Starting window test from the main test file\n");
    or_create_window(1024, 768, "Window Name", "com.my.window.app");
    or_add_listeners(NULL, NULL, NULL);
    or_start_main_loop();
    or_destroy_window();
    return 0;
}
