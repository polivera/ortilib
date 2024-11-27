# tests/CMakeLists.txt
enable_testing()

# Collect test sources
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

target_link_libraries(orwindow_test PRIVATE orwindow)
add_test(NAME orwindow_test COMMAND orwindow_test)