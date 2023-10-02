cmake_minimum_required(VERSION 3.26)
project(H3)

set(CMAKE_CXX_STANDARD 17)

add_executable(H3 main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")