cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if ($ENV{LRHNET_PICO_ENABLED})
    set(PICO_CXX_ENABLE_EXCEPTIONS 1)
    set(PICO_CXX_ENABLE_RTTI 1)

    # Initialise pico_sdk from installed location
    # (note this can come from environment, CMake cache etc)
    #set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/lib/pico-sdk")
    set(PICO_SDK_FETCH_FROM_GIT "true")  # Not sure if "true" is the correct value, but it works

    set(PICO_BOARD pico CACHE STRING "Board type")

    # Pull in Raspberry Pi Pico SDK (must be before project)
    include(pico_sdk_import.cmake)

    if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
      message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
    endif()
endif()

project(NetworkTester C CXX ASM)

if ($ENV{LRHNET_PICO_ENABLED})
    pico_sdk_init()
endif()

add_library(lrhnet-core INTERFACE)

target_sources(lrhnet-core INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/core/Frame.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/Frame.h
        ${CMAKE_CURRENT_LIST_DIR}/core/Packet.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/Packet.h
        ${CMAKE_CURRENT_LIST_DIR}/core/util.h
        ${CMAKE_CURRENT_LIST_DIR}/core/util.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/NetworkInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/NetworkInterface.h
)

add_library(lrhnet-drivers-dummy INTERFACE)

target_sources(lrhnet-drivers-dummy INTERFACE
        dummy/DummyNetworkInterface.cpp
        dummy/DummyNetworkInterface.h
)

add_library(lrhnet-drivers-rp2040 INTERFACE
        rp2040/SerialNetworkInterface.cpp
        rp2040/SerialNetworkInterface.h
)

target_sources(lrhnet-drivers-rp2040 INTERFACE
        pico_stdlib
        hardware_uart
        hardware_exception
)

add_executable(NetworkTester main.cpp)

target_link_libraries(NetworkTester
        lrhnet-core
        lrhnet-drivers-dummy
)

#pico_enable_stdio_usb(${PROJECT_NAME} 1)
#pico_enable_stdio_uart(${PROJECT_NAME} 0)

add_executable(rp2040-tester rp2040tester.cpp)

target_link_libraries(rp2040-tester
        pico_stdlib
)

target_include_directories(rp2040-tester PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

if ($ENV{LRHNET_PICO_ENABLED})
    pico_add_extra_outputs(rp2040-tester)
endif()