cmake_minimum_required(VERSION 3.14)
project(urecv)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()


find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(slsDetectorPackage REQUIRED)

add_executable(urecv main.cpp UdpSocket.cpp Streamer.cpp utils.cpp)
target_compile_options(urecv PRIVATE 
                            -Wall
                            -Wextra
                            -Wno-unused-parameter #Needs to be slowly mitigated
                            -Wold-style-cast
                            -Wnon-virtual-dtor
                            -Woverloaded-virtual
                            -Wdouble-promotion
                            -Wformat=2
                            -Wredundant-decls
                            # -Wconversion
                            -Wdouble-promotion
                            -Werror=return-type
)

target_link_libraries(urecv fmt pthread slsSupportLib)
target_include_directories(urecv PRIVATE ${Boost_INCLUDE_DIRS} )

