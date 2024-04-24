cmake_minimum_required(VERSION 3.28)
project(B_Plus_Tree)

set(CMAKE_CXX_STANDARD 20)

add_executable(code test/main.cpp
        src/BPT.hpp
        src/Database.hpp
        STLite-baihua/utility.hpp
        STLite-baihua/constantLengthString.hpp
        STLite-baihua/vector.hpp
        STLite-baihua/exceptions.hpp)
