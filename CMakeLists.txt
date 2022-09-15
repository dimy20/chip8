cmake_minimum_required(VERSION 3.14)
project(chip8 VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(glfw)
add_executable(main src/main.cpp src/chip8.cpp)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include glfw)
target_link_libraries(main glfw)
									   
