cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
add_compile_definitions(TEST_LIB_JAL_VER=$ENV{JAL_VER})

project (test)
include_directories(${PROJECT_BINARY_DIR})

add_executable(test test.cpp)
target_link_directories(test PUBLIC "${PROJECT_BINARY_DIR}/lib")
target_link_libraries(test JAL_static)
target_link_libraries(test StrExt_static)
