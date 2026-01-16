cmake_minimum_required(VERSION 3.15)

# Project name
project(RaylibProject LANGUAGES CXX)

# Set Clang as the compiler
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")

# Enable Link-Time Optimization (LTO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Optionally, enable LTO for Release mode
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Specify the location of the Raylib installation
find_package(raylib REQUIRED)

# Add source files from the `src` directory
file(GLOB SOURCES "src/*.cpp")

# Add executable target
add_executable(raylib_app ${SOURCES})

# Link the Raylib library to the target
target_link_libraries(raylib_app PRIVATE raylib)

# Include Raylib headers
target_include_directories(raylib_app PRIVATE ${raylib_INCLUDE_DIRS})

# If Raylib is not installed system-wide, you can add the following:
# set(RAYLIB_DIR "/path/to/raylib")
# add_subdirectory(${RAYLIB_DIR})

