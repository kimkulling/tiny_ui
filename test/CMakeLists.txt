cmake_minimum_required(VERSION 3.10)

# Find GoogleTest package (should be found in main CMakeLists.txt)
find_package(GTest CONFIG REQUIRED)

# Enable testing
enable_testing()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/stb_image
    ${GTest_INCLUDE_DIRS}
)

# Add the unittests subdirectory
add_subdirectory(unittests)
