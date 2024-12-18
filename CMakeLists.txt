cmake_minimum_required(VERSION 3.2)

option(TINY_UI_SAMPLES "The build will create the samples." ON)

PROJECT(tiny_ui)

INCLUDE_DIRECTORIES( BEFORE
    ${CMAKE_HOME_DIRECTORY}/src
)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

SET(CMAKE_CXX_FLAGS "${}")
ADD_LIBRARY(tiny_ui
    src/tinyui.h
    src/tinyui.cpp
    src/widgets.cpp
    src/widgets.h
    src/stb_image.h
    src/backends/sdl2_renderer.cpp
    src/backends/sdl2_renderer.h
    src/backends/sdl2_iodevice.h    
    src/backends/sdl2_iodevice.cpp
)
target_link_libraries(tiny_ui PRIVATE
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

if( TINY_UI_SAMPLES)
    ADD_EXECUTABLE(tiny_ui_sample
        samples/main.cpp
    )
    target_link_libraries(tiny_ui_sample
     tiny_ui
     $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
     $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
     $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

endif()