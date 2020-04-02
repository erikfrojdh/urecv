cmake_minimum_required(VERSION 3.14)
project(urecv)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()
option(RX_USE_TESTS "Build tests" ON)
option(RX_USE_SANITIZER "Sanitizers for debugging" OFF)
if(RX_USE_SANITIZER)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  set(CMAKE_BUILD_TYPE "Debug")
endif(RX_USE_SANITIZER)

find_package(fmt REQUIRED)

set(SRC_FILES
src/Receiver.cpp src/UdpSocket.cpp 
src/Streamer.cpp 
src/utils.cpp 
src/DirectWriter.cpp
src/FwriteWriter.cpp
# src/FileWriter.cpp 
# src/FileWriterDirect.cpp


)

add_library(rxStatic STATIC ${SRC_FILES})
target_compile_options(rxStatic INTERFACE 
                            -Wall
                            -Wextra
                            -Wshadow
                            -Wunused
                            -Wno-unused-parameter
                            -Wold-style-cast
                            -Wcast-align
                            -Wnon-virtual-dtor
                            -Woverloaded-virtual
                            -Wdouble-promotion
                            -Wformat=2
                            -Wredundant-decls
                            # -Wconversion
                            -Wnull-dereference
                            -Wdouble-promotion
                            -Werror=return-type
                            -Wpedantic
)

target_link_libraries(rxStatic PUBLIC fmt pthread zmq)

add_executable(urecv src/main.cpp)
target_link_libraries(urecv PUBLIC rxStatic)

add_executable(ftest src/ftest.cpp)
target_link_libraries(ftest PUBLIC rxStatic)

set( PYTHON_FILES
    read_binary.py
    read_raw.py
    zmqtest.py
)
foreach(FILE ${PYTHON_FILES})
    configure_file( python/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE}  )

endforeach(FILE ${PYTHON_FILES})


if(RX_USE_TESTS)
  add_subdirectory(tests)
endif(RX_USE_TESTS)