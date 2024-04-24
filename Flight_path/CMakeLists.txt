cmake_minimum_required(VERSION 3.28)
project(RK)

add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE eigen)

set(CMAKE_CXX_STANDARD 17)

add_executable(RK main.cpp)
