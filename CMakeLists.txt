cmake_minimum_required(VERSION 3.21)
project(udp_server C)

set(CMAKE_C_STANDARD 17)

add_executable(test test.c)
TARGET_LINK_LIBRARIES(test pthread)


add_executable(udp_server server.c)


TARGET_LINK_LIBRARIES(udp_server pthread)