cmake_minimum_required(VERSION 3.14)
project(urecv)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()

option(RX_USE_SANITIZER "Sanitizers for debugging" OFF)
if(RX_USE_SANITIZER)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
endif(RX_USE_SANITIZER)

find_package(fmt REQUIRED)
add_executable(urecv src/main.cpp src/Receiver.cpp src/UdpSocket.cpp src/Streamer.cpp src/utils.cpp)
target_compile_options(urecv PRIVATE 
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


target_link_libraries(urecv PUBLIC fmt pthread zmq)

