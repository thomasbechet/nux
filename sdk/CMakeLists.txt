project(sdk LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/core)

file(GLOB SOURCES *.c)
file(GLOB PARSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/parson/*.c)
add_library(${PROJECT_NAME} ${SOURCES} ${PARSON_SOURCES})
