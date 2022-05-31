cmake_minimum_required(VERSION 3.21)
project(bigNum)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(bigNum
        big_num.cpp
        big_num.h
        tests.cpp)
