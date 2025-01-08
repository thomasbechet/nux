project(project LANGUAGES C)

file(GLOB PROJECT_SOURCES *.c)
file(GLOB CJSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/cJSON/*.c)
add_library(${PROJECT_NAME} ${PROJECT_SOURCES} ${CJSON_SOURCES}) 

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
