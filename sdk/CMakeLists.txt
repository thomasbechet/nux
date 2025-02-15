project(sdk LANGUAGES C)

file(GLOB SDK_SOURCES *.c)
file(GLOB PARSON_SOURCES ${CMAKE_SOURCE_DIR}/externals/parson/*.c)
add_library(${PROJECT_NAME} ${SDK_SOURCES} ${PARSON_SOURCES}) 
target_link_libraries(${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
include_directories(${CMAKE_SOURCE_DIR}/sdk)
