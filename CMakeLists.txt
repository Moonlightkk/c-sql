cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

<<<<<<< HEAD
project(c_sql)

include_directories(./include)

add_executable(terminal src/terminal.cpp)
=======
add_executable(c_sql main.cpp include/U8String.h src/U8String.cpp)
>>>>>>> 7d941ce1e5dbe3020b88f179195399b313f8c3f8
