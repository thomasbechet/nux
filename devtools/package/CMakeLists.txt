project(package LANGUAGES C)

file(GLOB PACKAGE_SOURCES *.c)
file(GLOB CJSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/cJSON/*.c)
add_library(${PROJECT_NAME} ${PACKAGE_SOURCES} ${CJSON_SOURCES}) 

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
