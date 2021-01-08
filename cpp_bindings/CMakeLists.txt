cmake_minimum_required(VERSION 3.2)

project(cpp_bindings VERSION 0.0.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors")

include_directories(
    "src/"
)

file(GLOB all_cpp_bindings_src
    "src/*.cpp"
)

add_library(fast_boxes SHARED ${all_cpp_bindings_src})
