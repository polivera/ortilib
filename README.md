# ortilib

A lightweight C library collection for low-level Linux system programming, featuring a custom memory arena allocator and Wayland-based windowing.

## Libraries

### orarena - Memory Arena Allocator

A bump allocator providing fast, predictable memory allocation with support for shared memory regions. Designed for applications that benefit from arena-style memory management patterns.

**Features:**
- Stack-based allocation with O(1) allocation time
- Shared memory support for IPC scenarios
- Sub-arena creation for hierarchical memory scopes
- Platform-specific virtual memory backend (currently Linux via mmap)
- Optional allocation logging for debugging (`OR_DEBUG_MEMORY`)

### orwindow - Wayland Window Manager

A minimal windowing library for Wayland compositors with comprehensive input handling.

**Features:**
- Direct Wayland protocol integration
- Keyboard, mouse/pointer, and gamepad input support
- Window decorations via libdecor
- Fullscreen toggling
- Gamepad haptic feedback (rumble)
- Event-driven callback architecture
- Direct pixel buffer access for custom rendering

## Building

**Requirements:**
- CMake 3.26+
- C99 compiler (GCC or Clang)
- Wayland client libraries
- libdecor-0

```bash
# Standard build
cmake -S . -B build
cmake --build build

# With memory allocation logging
cmake -S . -B build -DOR_DEBUG_MEMORY=ON
cmake --build build

# Run tests
ctest --test-dir build
```

## Example Usage

```c
#include "orarena/orarena.h"
#include "orwindow/orwindow.h"

void draw(const struct ORBitmap *bitmap) {
    // Direct access to pixel buffer (BGRA format)
    uint8_t *pixel = bitmap->mem;
    for (int i = 0; i < bitmap->width * bitmap->height; i++) {
        pixel[0] = 255;  // Blue
        pixel[1] = 128;  // Green
        pixel[2] = 0;    // Red
        pixel[3] = 255;  // Alpha
        pixel += 4;
    }
}

void key_press(enum ORKeys code, uint8_t os_code, time_t time, uint8_t mod) {
    if (code == OR_KEY_ESC) {
        // Handle escape key
    }
}

int main() {
    struct ORArena *arena = arena_create_shared(100 * 1024 * 1024);

    or_create_window(800, 600, "My Window", "myapp", arena);

    struct ORWindowListeners window_listeners = { .draw = draw };
    struct ORKeyboardListeners keyboard_listeners = { .key_press = key_press };

    or_surface_setup(&window_listeners, &keyboard_listeners, NULL, NULL);
    or_start_main_loop();

    or_destroy_window();
    return 0;
}
```

## Architecture

The library follows a layered design:

- **orwindow** depends on **orarena** for all memory allocations
- Wayland buffer sharing leverages orarena's shared memory capabilities
- Platform abstraction layer supports future backends (X11, Windows)
- Modular input subsystems (keyboard, pointer, gamepad) can be enabled independently

See [doc/architecture.md](doc/architecture.md) for detailed architecture documentation.

## Platform Support

Currently Linux-only with Wayland compositor support. The codebase is structured to accommodate additional platforms.

## License

MIT License - see [LICENSE](LICENSE) for details.
