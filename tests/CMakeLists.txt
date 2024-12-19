# tests/CMakeLists.txt
enable_testing()

# Collect test sources for orwindow
file(GLOB_RECURSE ORWINDOW_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orwindow_tests/*.c"
)
add_executable(orwindow_test
        ${ORWINDOW_TEST_SOURCES}
)
target_include_directories(orwindow_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Collect test sources for orarena
file(GLOB_RECURSE orarena_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orarena_tests/*.c"
        ${CMAKE_SOURCE_DIR}/src/orarena/valloc_internal.h
)
add_executable(orarena_test
        ${orarena_TEST_SOURCES}
)
target_include_directories(orarena_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(orwindow_test PRIVATE orwindow orarena)
add_test(NAME orwindow_test COMMAND orwindow_test)

target_link_libraries(orarena_test PRIVATE orarena)
add_test(NAME orarena_test COMMAND orarena_test)
