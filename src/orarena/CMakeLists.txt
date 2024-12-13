file(GLOB_RECURSE ORARENA_PUBLIC_HEADERS
        ${CMAKE_SOURCE_DIR}/include/orarena/*.h
)

file(GLOB_RECURSE ORARENA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orarena.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/valloc_internal.h"
)

if (UNIX AND NOT APPLE)
    file(GLOB_RECURSE LINUX_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.h
    )
    list(APPEND ORARENA_SOURCES ${LINUX_SOURCES})
endif ()

add_library(orarena SHARED
        ${ORARENA_PUBLIC_HEADERS}
        ${ORARENA_SOURCES}
)

# Specify where to find headers
target_include_directories(orarena
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against wayland-client and decor libraries
target_link_libraries(orarena
        PRIVATE
)

install(TARGETS orarena
        EXPORT orarena-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/orarena
)