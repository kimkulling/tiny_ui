cmake_minimum_required(VERSION 3.2)
include("cmake/HunterGate.cmake")

option(TINY_UI_SAMPLES "The build will create the samples." ON)

# Todo: we need to solve this more cleaner
IF (WIN32)
    HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
        SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
    )
ELSE()
    HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.111.tar.gz"
        SHA1 "4189bbd869abe4eb69b5303d3f916c03d34dc154"
    )
ENDIF()

PROJECT(tiny_ui)

INCLUDE_DIRECTORIES( BEFORE
    ${CMAKE_HOME_DIRECTORY}/src
)

hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)

hunter_add_package(SDL_ttf)
find_package(SDL_ttf CONFIG REQUIRED)

SET(CMAKE_CXX_FLAGS "${}")
ADD_LIBRARY(tiny_ui
    src/tinyui.h
    src/tinyui.cpp
    src/widgets.cpp
    src/widgets.h
    src/stb_image.h
    src/sdl2_renderer.cpp
    src/sdl2_renderer.h
)
target_link_libraries(tiny_ui SDL2::SDL2 SDL_ttf::SDL_ttf)

if( TINY_UI_SAMPLES)
    ADD_EXECUTABLE(tiny_ui_sample
        samples/main.cpp
    )
    target_link_libraries(tiny_ui_sample tiny_ui SDL2::SDL2 SDL_ttf::SDL_ttf)
endif()