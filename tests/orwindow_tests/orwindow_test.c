//
// Created by pablo on 11/27/24.
//

#include <stdio.h>
#include "orwindow/orwindow.h"

int
main() {
    printf("something only we know\n");
    or_create_window(1024, 768, "Window Name", "com.my.window.app");
    or_add_listeners(NULL, NULL,NULL);
    return 0;
}