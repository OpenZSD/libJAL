cmake_minimum_required(VERSION 3.16)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_static)
add_compile_definitions(LIB_STR_EXT_VER=$ENV{STR_EXT_VER})

project (StrExt VERSION $ENV{STR_EXT_VER})
add_library(StrExt SHARED
    src/StrExt.cpp
    src/StrListExt.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
target_include_directories(StrExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(StrExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

project (StrExt_static VERSION 1.0.0)
add_library(StrExt_static STATIC
    src/StrExt.cpp
    src/StrListExt.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib_static)
target_include_directories(StrExt_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(StrExt_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


