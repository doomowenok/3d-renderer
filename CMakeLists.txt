cmake_minimum_required(VERSION 3.30)
project(3d_renderer C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

file(GLOB src "src/*.h" "src/*.c")
add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})