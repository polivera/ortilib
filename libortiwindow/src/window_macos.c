#include <stdint.h>

typedef struct {
  uint64_t window_id;
  uint64_t display_id;
} OrtiWindow;

// Declare the Objective-C functions
extern void swift_open_window();

OrtiWindow orti_open_window() {
  swift_open_window(); // Open the window
  OrtiWindow window;
  window.display_id = 0;
  window.window_id = 0;
  return window;
}
