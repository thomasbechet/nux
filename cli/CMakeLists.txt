project(nuxcli LANGUAGES C)

file(GLOB CLI_SOURCES *.c)
file(GLOB ARGPARSE_SOURCES ${CMAKE_SOURCE_DIR}/externals/argparse/*.c)
add_executable(nux ${CLI_SOURCES} ${ARGPARSE_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/runtimes)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/sdk)

target_link_libraries(nux sdk vmnative)
install(TARGETS nux)
