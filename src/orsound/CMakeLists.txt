file(GLOB_RECURSE ORSOUND_PUBLIC_HEADERS
        ${CMAKE_SOURCE_DIR}/include/orsound/*.h
)

file(GLOB_RECURSE ORSOUND_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orsound.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/orsound_internal.h"
)

if (UNIX AND NOT APPLE)
    file(GLOB_RECURSE LINUX_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.h
    )
    list(APPEND ORSOUND_SOURCES ${LINUX_SOURCES})
endif ()

add_library(orsound SHARED
        ${ORSOUND_PUBLIC_HEADERS}
        ${ORSOUND_SOURCES}
)

if (OR_DEBUG_MEMORY)
    add_definitions(-DOR_DEBUG_MEMORY)
endif ()

# Set compile definitions for the library
if (OR_DEBUG_MEMORY)
    target_compile_definitions(orsound PUBLIC OR_DEBUG_MEMORY)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(PIPEWIRE REQUIRED libpipewire-0.3>=0.3)

# Specify where to find headers
target_include_directories(orsound
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PIPEWIRE_INCLUDE_DIRS}
)

# Link against wayland-client and decor libraries
target_link_libraries(orsound
        PRIVATE
        orarena
        pipewire-0.3
        m # Math library
)

install(TARGETS orsound
        EXPORT orsound-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/orsound
)