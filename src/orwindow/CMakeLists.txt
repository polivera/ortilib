file(GLOB_RECURSE ORWINDOW_PUBLIC_HEADERS
        ${CMAKE_SOURCE_DIR}/include/orwindow/*.h
)

file(GLOB_RECURSE ORWINDOW_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orwindow.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/orwindow_internal.h"
)

if (UNIX AND NOT APPLE)
    file(GLOB_RECURSE LINUX_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linux/*.h
    )
    list(APPEND ORWINDOW_SOURCES ${LINUX_SOURCES})
endif ()

add_library(orwindow SHARED
        ${ORWINDOW_PUBLIC_HEADERS}
        ${ORWINDOW_SOURCES}
)

# Specify where to find headers
target_include_directories(orwindow
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS orwindow
        EXPORT orwindow-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/orwindow
)