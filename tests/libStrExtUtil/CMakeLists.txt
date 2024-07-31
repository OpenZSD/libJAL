cmake_minimum_required(VERSION 3.16)

add_compile_definitions(TEST_LIB_STR_EXT_VER=$ENV{STR_EXT_VER})

project (test)
include_directories(${PROJECT_BINARY_DIR})

add_executable(test test.cpp)
target_link_directories(test PUBLIC "${PROJECT_BINARY_DIR}/lib")
target_link_libraries(test StrExt_static)
