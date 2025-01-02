project(builder LANGUAGES C)

file(GLOB BUILDER_SOURCES *.c)
file(GLOB CJSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/cJSON/*.c)
add_library(${PROJECT_NAME} ${BUILDER_SOURCES} ${CJSON_SOURCES}) 

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
