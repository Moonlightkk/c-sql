cmake_minimum_required(VERSION 3.22)
project(c_sql)

set(CMAKE_CXX_STANDARD 17)

add_executable(c_sql src/utest.cpp include/U8String.h include/FileIO.h src/U8String.cpp src/FileIO.cpp)
