cmake_minimum_required(VERSION 3.6)
project(lab13)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/graphics.cpp)
add_executable(game ${SOURCE_FILES})