cmake_minimum_required(VERSION 3.23)

project(huffman)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/ files)

add_library(files ${files})

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} files)
