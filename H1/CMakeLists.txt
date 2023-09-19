cmake_minimum_required(VERSION 3.26)
project(H1)

set(CMAKE_CXX_STANDARD 17)

add_executable(H1
        main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")