cmake_minimum_required(VERSION 3.28)
project(B_Plus_Tree)

set(CMAKE_CXX_STANDARD 20)

add_executable(code test/main.cpp
        src/BPT.h
        STLite-baihua/utility.hpp)
