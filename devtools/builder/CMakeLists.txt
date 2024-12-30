project(builder LANGUAGES C)

file(GLOB BUILDER_SOURCES *.c)
add_library(${PROJECT_NAME} ${BUILDER_SOURCES}) 

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
include_directories(${vmcore_SOURCE_DIR})
