cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

project(c_sql)

include_directories(./include)

add_executable(terminal src/terminal.cpp)