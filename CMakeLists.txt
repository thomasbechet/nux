cmake_minimum_required(VERSION 3.10.0)

project(nux VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(runtimes/vmnative)
add_subdirectory(devtools/project)
add_subdirectory(cli)
