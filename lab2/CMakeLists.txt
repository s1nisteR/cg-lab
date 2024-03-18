cmake_minimum_required(VERSION 3.15)

project(cse420-lab2)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/glfw)
include_directories(vendor/glfw/include vendor/glad/include)

find_package(OpenGL REQUIRED)

add_executable(cse420-lab2 src/main.cpp vendor/glad/src/glad.c)
target_link_libraries(cse420-lab2 glfw)