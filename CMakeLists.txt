cmake_minimum_required(VERSION 3.14)
project(chip8 VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

add_subdirectory(glfw)
add_executable(${PROJECT_NAME} 
				src/main.cpp
				src/glad.c
				src/chip8.cpp
				src/Shader.cpp
				src/Screen.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC 
						   ${CMAKE_CURRENT_SOURCE_DIR}/include 
						   glfw)
target_link_libraries(${PROJECT_NAME} glfw GL)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders DESTINATION /etc/${PROJECT_NAME})
