cmake_minimum_required(VERSION 3.15)
project(chip8_emulator CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

# raylib via vcpkg
find_package(raylib CONFIG REQUIRED)
# glfw via vcpkg
find_package(glfw3 CONFIG REQUIRED)

# Add the executable and its sources
add_executable(
    chip8_emulator
    main.cpp
    chip8.cpp
    chip8.h
)

# Link glfw
target_link_libraries(chip8_emulator PRIVATE glfw)

# Link raylib
target_link_libraries(chip8_emulator PRIVATE raylib)

# warnings
target_compile_options(chip8_emulator PRIVATE -Wall -Wextra -pedantic -g)
