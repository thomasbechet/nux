project(project LANGUAGES C)

file(GLOB PROJECT_SOURCES *.c)
file(GLOB PARSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/parson/*.c)
add_library(${PROJECT_NAME} ${PROJECT_SOURCES} ${PARSON_SOURCES}) 
target_link_libraries(${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
