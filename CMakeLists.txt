cmake_minimum_required(VERSION 3.27)
project(NetworkTester)

set(CMAKE_CXX_STANDARD 17)

add_executable(NetworkTester main.cpp
        layers/Frame.cpp
        layers/Frame.h
)
