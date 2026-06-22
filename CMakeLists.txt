cmake_minimum_required(VERSION 3.15)

project(A_star)

set(CMAKE_CXX_STANDARD 17)

add_executable(A_star src/main.cpp)

target_include_directories(A_star PRIVATE include)

target_link_directories(A_star PRIVATE lib)

target_link_libraries(A_star
    raylib
    gdi32
    winmm
    opengl32
)
