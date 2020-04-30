

find_package(Catch2 REQUIRED)

add_executable(test test.cpp 
                test-SimpleQueue.cpp
                test-ImageFifo.cpp)
target_include_directories(test PRIVATE ../src)
target_link_libraries(test Catch2::Catch2 rxStatic)
set_target_properties(test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )