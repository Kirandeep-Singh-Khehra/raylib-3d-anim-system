cmake_minimum_required(VERSION 3.10)
project(kanim)

add_library(kanim INTERFACE)

target_include_directories(kanim INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

