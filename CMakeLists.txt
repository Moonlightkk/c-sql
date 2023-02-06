cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

project(c_sql)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)


# 生成dll
aux_source_directory(./src SOURCE)
aux_source_directory(./include INCLUDE)
add_library(${PROJECT_NAME} STATIC ${SOURCE} ${INCLUDE})
install(TARGETS ${PROJECT_NAME} DESTINATION  ${PROJECT_SOURCE_DIR}/lib)
#add_executable(terminal src/terminal.cpp)

aux_source_directory(./lib LIBS)
add_executable(main demo/main.cpp)
target_link_libraries(main
        ${PROJECT_NAME}
        ${LIBS}
        )
