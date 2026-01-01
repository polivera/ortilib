# Architecture

This document describes the design and architecture of ortilib's two main components: orarena and orwindow.

## Design Philosophy

ortilib is built around **arena-based memory management** as a foundational principle. All components use arena allocation to provide:

- Predictable memory usage patterns
- Fast O(1) allocations without malloc overhead
- Simplified memory lifetimes tied to application phases
- Zero fragmentation within arenas
- Efficient cleanup through arena destruction

## orarena: Memory Arena Allocator

### Core Concept

orarena implements a **bump allocator** (also called a linear allocator or region allocator). Memory is allocated sequentially from a contiguous block, moving a pointer forward with each allocation. This design trades flexibility for performance and simplicity.

### Memory Layout

```
┌─────────────────────────────────────────────────────────┐
│  ORArena Struct  │         Allocatable Memory           │
│  (metadata)      │         (total_size bytes)           │
│                  │                                       │
│  - total_size    │  [allocated][allocated][  free   ]   │
│  - used_size ────┼──────────────────────^               │
│  - previous_size │                                       │
│  - parent        │                                       │
│  - *memory ──────┼──►                                    │
└─────────────────────────────────────────────────────────┘
```

### Allocation Types

**Regular Arena** (`arena_create`):
- Uses `mmap` with `MAP_ANONYMOUS | MAP_PRIVATE`
- Memory is process-local
- Suitable for general-purpose allocations

**Shared Arena** (`arena_create_shared`):
- Uses `mmap` with `MAP_SHARED | MAP_ANONYMOUS`
- Memory can be shared between processes
- Critical for Wayland buffer sharing (requires shared memory for compositor access)

### Sub-Arenas

Sub-arenas are arenas allocated *within* a parent arena's memory space:

```
Parent Arena:
┌────────────────────────────────────────────────┐
│ [data][sub-arena][data]                        │
│         │                                       │
│         └──► ┌─────────────────────┐           │
│              │ Sub-Arena Struct    │           │
│              │ [allocated][free]   │           │
│              └─────────────────────┘           │
└────────────────────────────────────────────────┘
```

**Properties:**
- Inherits the parent's memory type (shared/private)
- Useful for temporary allocations with scoped lifetimes
- Parent cannot be reset while sub-arenas exist (protection mechanism)
- Destruction zeroes the memory but doesn't free it from parent

### Virtual Memory Abstraction

The `valloc_internal.h` interface abstracts platform-specific virtual memory operations:

```c
void *virtual_alloc(size_t size);          // Regular mmap
void *virtual_shared_alloc(size_t size);   // Shared mmap
int virtual_free(void *ptr, size_t size);  // munmap
```

This abstraction enables future ports to Windows (`VirtualAlloc`) or other platforms.

### Debug Logging

When compiled with `OR_DEBUG_MEMORY`, all arena operations log to stdout:

```
[ORArena 0x7f8a...] Created with size 104857600
[ORArena 0x7f8a...] Allocating 32 bytes (aligned: 8). Used 32
Sub [ORArena 0x7f8b...] Created with size 1024
```

This is invaluable for tracking memory usage patterns and diagnosing allocation issues.

## orwindow: Wayland Window Manager

### Three-Layer Architecture

orwindow uses a layered abstraction to separate platform concerns:

```
┌──────────────────────────────────────────────┐
│  Public API (orwindow.h / orwindow.c)        │
│  - or_create_window()                        │
│  - or_surface_setup()                        │
│  - or_start_main_loop()                      │
└────────────────┬─────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────┐
│  Internal Layer (orwindow_internal.h)        │
│  - inter_create_window()                     │
│  - inter_surface_setup()                     │
│  - Platform dispatching                      │
└────────────────┬─────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────┐
│  Wayland Implementation (linux/)             │
│  - wayland_client.c                          │
│  - wayland_decorator.c                       │
│  - wayland_keyboard.c                        │
│  - wayland_pointer.c                         │
│  - wayland_gamepad.c                         │
└──────────────────────────────────────────────┘
```

**Layer 1: Public API**
- Manages global state (arena, bitmap, listener aggregation)
- Provides platform-agnostic interface to applications
- Handles arena lifecycle (creates default 100MB shared arena if none provided)

**Layer 2: Internal Abstraction**
- Platform detection point (designed for Wayland vs X11 selection)
- Currently dispatches to Wayland-specific implementations
- Placeholder for future multi-platform support

**Layer 3: Platform Implementation**
- Direct Wayland protocol handling
- Modular subsystems for different concerns

### Wayland Subsystem Breakdown

**wayland_client.c**: Core Wayland Connection
- Establishes connection to Wayland display server
- Implements registry listener for global object discovery
- Manages shared memory pool and buffer creation
- Frame callback loop for rendering
- Device registration (compositor, shared memory, seat)

**wayland_decorator.c**: Window Decorations
- Integrates libdecor for client-side decorations
- Handles window titlebar, borders, resize grips
- Manages fullscreen state transitions
- Processes decoration events (close, resize, configure)

**wayland_keyboard.c**: Keyboard Input
- Maps Wayland keycodes to portable `ORKeys` enum
- Tracks modifier state (shift, ctrl, alt, etc.)
- Dispatches key press/release callbacks
- Handles keyboard focus enter/leave events

**wayland_pointer.c**: Mouse/Pointer Input
- Pointer motion tracking with surface-relative coordinates
- Button press/release events
- Pointer enter/leave surface detection
- Cursor support

**wayland_gamepad.c**: Gamepad Support
- Linux evdev-based gamepad detection
- Supports up to 4 simultaneous controllers
- Button mapping to standard Xbox-style layout
- Analog stick and trigger motion with normalized values [-1.0, 1.0]
- Force-feedback (rumble) via evdev `FF_RUMBLE`

### Event Flow

```
Wayland Compositor
       │
       ▼
wl_display_dispatch() ──► Registry Listener
       │                         │
       │                         ▼
       │                  Device Registration
       │                  (keyboard, pointer, etc.)
       │
       ▼
Frame Callback ──► inter_draw() ──► Application draw()
       │                                   │
       │                                   ▼
       │                          Direct Bitmap Access
       │                          (BGRA pixel buffer)
       │
       ▼
Input Events ──► Wayland Input Listeners ──► Application Callbacks
```

### Memory Management in orwindow

All orwindow allocations use the provided (or internally created) arena:

1. **Bitmap structure**: Allocated from arena during window creation
2. **Wayland client structures**: Allocated from arena
3. **Pixel buffer**: Allocated via shared memory pool (backed by `arena_create_shared`)

The shared memory requirement is imposed by Wayland's architecture: the compositor must directly access the pixel buffer for rendering. This is why orwindow uses `arena_create_shared` by default.

### Bitmap Format

```c
struct ORBitmap {
    uint16_t width;       // Pixel width
    uint16_t height;      // Pixel height
    int32_t mem_size;     // Total bytes (width * height * 4)
    uint16_t stride;      // Bytes per row (width * 4)
    uint8_t *mem;         // Pixel data pointer
};
```

Pixel format is **BGRA** (32-bit per pixel):
```
[Blue][Green][Red][Alpha] [Blue][Green][Red][Alpha] ...
  0     1      2     3       4     5      6     7
```

Applications write directly to `bitmap->mem` in their `draw()` callback.

### Listener Pattern

orwindow uses a callback-driven event model:

```c
struct ORWindowListeners {
    void (*draw)(const struct ORBitmap *bitmap);
    void (*enter)();
    void (*leave)();
    void (*resize)();
    void (*fullscreen)();
    void (*close)();
};
```

Similar structures exist for keyboard, pointer, and gamepad. Applications populate these structures with function pointers during `or_surface_setup()`, and the main loop invokes them when events occur.

## Inter-Library Dependencies

```
┌──────────────────────────────────────┐
│           Application                │
└─────────────┬────────────────────────┘
              │
    ┌─────────┴─────────┐
    ▼                   ▼
┌─────────┐      ┌──────────────┐
│ orarena │◄─────┤   orwindow   │
└─────────┘      └──────────────┘
                        │
                        ▼
              ┌──────────────────┐
              │ Wayland Protocol │
              │ (wayland-client) │
              │ (libdecor)       │
              └──────────────────┘
```

**Key coupling:**
- orwindow depends on orarena (CMakeLists.txt: `target_link_libraries(orwindow PRIVATE orarena)`)
- orwindow's Wayland buffer requires orarena's shared memory capability
- Applications can use orarena independently, but orwindow requires it

## Platform Portability Strategy

The codebase is structured for future multi-platform support:

```
src/
├── orarena/
│   ├── orarena.c          # Platform-agnostic
│   └── linux/
│       └── valloc_internal.c   # Linux mmap implementation
│   └── windows/           # Future: VirtualAlloc
│   └── macos/             # Future: mach_vm_allocate
│
└── orwindow/
    ├── orwindow.c         # Platform-agnostic
    └── linux/
        └── orwindow_internal.c  # Wayland dispatcher
        └── wayland_*.c
    └── windows/           # Future: Win32 API
    └── x11/               # Future: X11 protocol
```

Adding a new platform requires:
1. Implementing `valloc_internal.h` for that platform's virtual memory API
2. Implementing `orwindow_internal.h` for that platform's windowing system
3. Adding conditional compilation in CMakeLists.txt

## Build System

CMake is configured to:
- Build both libraries as shared objects (`.so` on Linux)
- Automatically include platform-specific sources via `GLOB_RECURSE` in platform directories
- Link orwindow against orarena and external dependencies (wayland-client, decor-0)
- Generate separate test executables for each library

The `OR_DEBUG_MEMORY` option is a compile definition that enables conditional logging macros in orarena.c.
