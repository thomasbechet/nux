file(GLOB RUNTIME_SOURCES *.c)
add_executable(${PROJECT_NAME} ${RUNTIME_SOURCES})

target_link_libraries(${PROJECT_NAME} vmcore)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "nux")

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${vmcore_SOURCE_DIR})
