add_executable(lrhnet-examples-calibrator calibrator/calibrator.cpp)
target_include_directories(lrhnet-examples-calibrator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(lrhnet-examples-calibrator
    lrhnet-core
    lrhnet-drivers-rlc
)

add_executable(lrhnet-examples-listener listener/listener.cpp)
target_include_directories(lrhnet-examples-listener PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(lrhnet-examples-listener
    lrhnet-core
    lrhnet-drivers-rlc
)
