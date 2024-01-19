cmake_minimum_required(VERSION 3.26)
project(NetworkTester)

set(CMAKE_CXX_STANDARD 17)

add_library(lrhnet-core INTERFACE)

target_sources(lrhnet-core INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/layers/Frame.cpp
        ${CMAKE_CURRENT_LIST_DIR}/layers/Frame.h
        ${CMAKE_CURRENT_LIST_DIR}/layers/Packet.cpp
        ${CMAKE_CURRENT_LIST_DIR}/layers/Packet.h
        ${CMAKE_CURRENT_LIST_DIR}/layers/util.h
        ${CMAKE_CURRENT_LIST_DIR}/layers/util.cpp
        ${CMAKE_CURRENT_LIST_DIR}/layers/NetworkInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/layers/NetworkInterface.h
)

add_library(lrhnet-drivers-dummy INTERFACE)

target_sources(lrhnet-drivers-dummy INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/layers/DummyNetworkInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/layers/DummyNetworkInterface.h
)

add_library(lrhnet-drivers-rp2040 INTERFACE)

target_sources(lrhnet-drivers-rp2040 INTERFACE
)

add_executable(NetworkTester main.cpp)

target_link_libraries(NetworkTester
        lrhnet-core
        lrhnet-drivers-dummy
)
