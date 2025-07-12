cmake_minimum_required(VERSION 3.10.0)

project(nux VERSION 0.0.1)

include_directories(${CMAKE_SOURCE_DIR}/core)

file(GLOB SOURCES *.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} core m)
install(TARGETS ${PROJECT_NAME})
