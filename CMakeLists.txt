cmake_minimum_required(VERSION 3.15)
project(chip8_emulator CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# raylib via vcpkg
find_package(raylib CONFIG REQUIRED)

# Add the executable and its sources
add_executable(chip8_emulator
    main.cpp
    chip.cpp
)

# Link raylib
target_link_libraries(chip8_emulator PRIVATE raylib)

# warnings
target_compile_options(chip8_emulator PRIVATE -Wall -Wextra -pedantic)
