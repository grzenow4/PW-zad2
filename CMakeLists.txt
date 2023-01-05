cmake_minimum_required(VERSION 3.1)
project(MIMUW-FORK C CXX)

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-sign-compare")
set(CMAKE_C_FLAGS "-g -Wall -Wextra -Wno-sign-compare")

function(add_example_program target_name)
  add_executable(${target_name} "${target_name}.cpp")
  target_link_libraries(${target_name} utils err task pthread)
endfunction()

add_library(err err.c)
add_library(utils utils.c)
add_library(task task.cpp)
add_example_program(executor)
