cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

project(Koaloader)

add_library(Koaloader SHARED
        ${SRC_DIR}/main.cpp
        )
