project(nuxcli LANGUAGES C)

file(GLOB NATIVE_CLI_SOURCES *.c)
file(GLOB ARGPARSE_SOURCES ${CMAKE_SOURCE_DIR}/externals/argparse/*.c)
add_executable(nux ${NATIVE_CLI_SOURCES} ${ARGPARSE_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/runtimes)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/externals)

target_link_libraries(nux vmnative)
