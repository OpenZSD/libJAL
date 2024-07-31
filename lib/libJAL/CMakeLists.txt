cmake_minimum_required(VERSION 3.16)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_static)
add_compile_definitions(LIB_JAL_VER=$ENV{JAL_VER})

project (JAL VERSION $ENV{JAL_VER})
include_directories("${PROJECT_BINARY_DIR}/res")
add_library(JAL SHARED
    src/Error.cpp
)
target_link_directories(JAL PRIVATE "${PROJECT_BINARY_DIR}/res/lib")
target_link_libraries(JAL StrExt)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
target_include_directories(JAL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(JAL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

project (JAL_static VERSION 1.0.0)
include_directories("${PROJECT_BINARY_DIR}/res")
add_library(JAL_static STATIC
    src/Error.cpp
)
target_link_directories(JAL_static PRIVATE "${PROJECT_BINARY_DIR}/res/lib")
target_link_libraries(JAL_static StrExt_static)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib_static)
target_include_directories(JAL_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(JAL_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


