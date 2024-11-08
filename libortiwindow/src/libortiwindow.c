#include "./window.h"

void open_window() {

  struct orti_bitmap bitmap = new_orti_window(1024, 768);
  orti_open_window(&bitmap);
}
