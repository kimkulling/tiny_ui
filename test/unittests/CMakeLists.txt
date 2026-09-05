cmake_minimum_required(VERSION 3.10)

# Add the ImageCache tests
add_executable(tinyui_unittests
    imagecache_test.cpp
)

# Link libraries
target_link_libraries(tinyui_unittests
    tiny_ui
    GTest::GTest
    GTest::Main
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

# Add test
gtest_add_tests(
    TARGET tinyui_unittests
    TEST_PREFIX "TinyUI."
    TEST_LIST "All"
)

# Ensure tiny_ui library is built before tests
add_dependencies(tinyui_unittests tiny_ui)
