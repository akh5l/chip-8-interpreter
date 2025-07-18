cmake_minimum_required(VERSION 3.10.0)
project(chip8 VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

find_package(SDL3 REQUIRED)

add_executable(chip8 main.c)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)
