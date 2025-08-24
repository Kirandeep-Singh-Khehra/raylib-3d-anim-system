cmake_minimum_required(VERSION 3.10)
project(kanim)

add_library(tiny-regex-c STATIC
    external/tiny-regex-c/re.c
)

add_library(kanim INTERFACE)

target_include_directories(kanim INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/tiny-regex-c>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(kanim INTERFACE tiny-regex-c)
