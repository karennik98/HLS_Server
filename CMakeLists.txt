cmake_minimum_required(VERSION 3.1)

set(project_name streamapp) 
project(${project_name})

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2")

add_subdirectory(src)

