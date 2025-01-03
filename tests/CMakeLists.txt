# tests/CMakeLists.txt
enable_testing()

# Define the debug memory option at the top
option(OR_DEBUG_MEMORY "Enable arena memory logging" ON)

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

# Collect test sources for orwindow
file(GLOB_RECURSE ORSOUND_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orsound_tests/*.c"
)
add_executable(orsound_test
        ${ORSOUND_TEST_SOURCES}
)
target_include_directories(orsound_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)


# Collect test sources for orarena
file(GLOB_RECURSE orarena_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/orarena_tests/*.c"
        ${CMAKE_SOURCE_DIR}/src/orarena/valloc_internal.h
)
if (OR_DEBUG_MEMORY)
    add_definitions(-DOR_DEBUG_MEMORY)
endif ()

add_executable(orarena_test
        ${orarena_TEST_SOURCES}
)
# Set compile definitions for both the library and test
if (OR_DEBUG_MEMORY)
    target_compile_definitions(orarena_test PRIVATE OR_DEBUG_MEMORY)
endif ()

target_include_directories(orarena_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(orarena_test PRIVATE orarena)
add_test(NAME orarena_test COMMAND orarena_test)

target_link_libraries(orwindow_test PRIVATE orwindow orarena)
add_test(NAME orwindow_test COMMAND orwindow_test)

target_link_libraries(orsound_test PRIVATE orsound orarena)
add_test(NAME orsound_test COMMAND orsound_test)
