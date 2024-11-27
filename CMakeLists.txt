cmake_minimum_required(VERSION 3.26)
project(ortilib C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src/orwindow)
add_subdirectory(tests)
