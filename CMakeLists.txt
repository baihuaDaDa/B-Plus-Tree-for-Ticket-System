cmake_minimum_required(VERSION 3.22)
project(B_Plus_Tree)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O2  -Wall")

include_directories(src)
include_directories(STLite-baihua)

add_executable(code test/main.cpp)
