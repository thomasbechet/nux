project(runtime LANGUAGES C)

set (CMAKE_C_STANDARD 99)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} runtime.c)

add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})
target_link_libraries(${PROJECT_NAME} vmlib m)

include_directories(${CMAKE_SOURCE_DIR})
