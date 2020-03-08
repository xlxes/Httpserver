cmake_minimum_required(VERSION 3.10)
project(httpserver)

add_compile_options(-std=c++11)

find_package(Threads REQUIRED)

include_directories(base logger timer)


#add_library(httpcondition base/httpcondition.cpp)
add_library(httpthread base/httpthread.cpp)

add_library(logger logger/logger.cpp)

add_library(parameters src/parameters.cpp)
add_library(threadpool src/threadpool.cpp)
add_library(server src/server.cpp)
add_library(stringsplit src/stringsplit.cpp)
add_library(epollserver src/epollserver.cpp)


add_executable(httpserver src/main.cpp)
target_link_libraries(parameters logger)
target_link_libraries(server logger)
#target_link_libraries(threadpool parameters epollserver httpcondition httpthread ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(epollserver server stringsplit threadpool httpcondition httpthread ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(httpserver  httpcondition httpthread logger parameters threadpool server stringsplit epollserver ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(threadpool parameters epollserver httpthread ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(epollserver server stringsplit threadpool  httpthread ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(httpserver httpthread logger parameters threadpool server stringsplit epollserver ${CMAKE_THREAD_LIBS_INIT})
