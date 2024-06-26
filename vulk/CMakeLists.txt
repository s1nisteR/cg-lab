cmake_minimum_required(VERSION 3.15)

project(vulk)

set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)

#add_subdirectory(vendor/glfw)
#include_directories(vendor/glfw/include vendor/glad/include)

#find_package(OpenGL REQUIRED)

add_subdirectory(vendor/glfw)

include_directories(vendor/glfw/include)

add_executable(vulk main.cpp)
target_link_libraries(vulk glfw Vulkan::Vulkan)
#target_link_libraries(cse420-lab6 glfw)