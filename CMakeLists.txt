cmake_minimum_required(VERSION 3.22)
include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
    SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
)

PROJECT( tiny_ui )

hunter_add_package(sdl2)
find_package(SDL2 CONFIG REQUIRED)

ADD_EXECUTABLE( sdl_ui
    src/main.cpp
    src/sdl2_renderer.cpp
    src/sdl2_renderer.h
)

target_link_libraries(sdl_ui SDL2::SDL2)
