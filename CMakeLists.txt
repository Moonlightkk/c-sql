cmake_minimum_required(VERSION 3.22)
project(c_sql)

set(CMAKE_CXX_STANDARD 17)

add_executable(c_sql main.cpp include/U8String.h src/U8String.cpp)
