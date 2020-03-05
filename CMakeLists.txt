cmake_minimum_required(VERSION 2.8.3)
project(TCP)

SET( CMAKE_CXX_FLAGS "-std=c++11 -O3")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


find_package(Threads REQUIRED)
include_directories(logger)
include_directories(base)
include_directories(timer)



add_library(parameters src/parameters.cpp)
add_library(logger logger/logger.cpp)
add_library(stringsplit src/stringsplit.cpp)
add_library(server src/server.cpp)
add_library(epollserver src/epollserver.cpp)

add_executable(httpserver src/main.cpp)

target_link_libraries(server logger)
target_link_libraries(parameters logger)
target_link_libraries(epollserver server stringsplit logger)
target_link_libraries(httpserver epollserver server logger parameters)


