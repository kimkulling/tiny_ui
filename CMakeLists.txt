cmake_minimum_required(VERSION 3.2)
include("cmake/HunterGate.cmake")

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.111.tar.gz"
    SHA1 "4189bbd869abe4eb69b5303d3f916c03d34dc154"
)

PROJECT(tiny_ui)

hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)
include_directories(./ ${SDL2_INCLUDE_DIRS})

ADD_EXECUTABLE( tiny_ui
    src/main.cpp
    src/sdl2_renderer.cpp
    src/sdl2_renderer.h
)

target_link_libraries(tiny_ui ${SDL2_LIBRARIES})
