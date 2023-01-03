cmake_minimum_required(VERSION 3.1)
project(MIMUW-FORK CXX)

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-sign-compare")

function(add_example_program target_name)
  add_executable(${target_name} "${target_name}.cpp")
  target_link_libraries(${target_name} utils err task pthread)
endfunction()

add_library(err err.cpp)
add_library(utils utils.cpp)
add_library(task task.cpp)
add_example_program(executor)
