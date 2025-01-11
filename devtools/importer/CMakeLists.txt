project(importer LANGUAGES C)

file(GLOB IMPORTER_SOURCES *.c)
add_library(${PROJECT_NAME} ${IMPORTER_SOURCES}) 
target_link_libraries(${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
