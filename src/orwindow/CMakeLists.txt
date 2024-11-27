add_library(orwindow
        orwindow.c
        ${CMAKE_SOURCE_DIR}/include/orwindow/orwindow.h
        # other source files
)

# Specify where to find headers
target_include_directories(orwindow
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include/orwindow/orwindow.h
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