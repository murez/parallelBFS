cmake_minimum_required(VERSION 3.10)

project(BFS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS "-O3")

find_package(OpenMP)

add_executable(pbfs main.cpp csrgraph.h sliding_q.h bitmap.h timer.h)

target_link_libraries(pbfs OpenMP::OpenMP_CXX)