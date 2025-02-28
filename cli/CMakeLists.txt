project(nuxcli LANGUAGES C)

file(GLOB CLI_SOURCES *.c)
file(GLOB ARGPARSE_SOURCES ${CMAKE_SOURCE_DIR}/externals/argparse/*.c)
add_executable(nux ${CLI_SOURCES} ${ARGPARSE_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/runtimes/native)
include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/sdk)

if (NUX_BUILD_SDK)
target_link_libraries(nux sdk vmnative)
else ()
target_link_libraries(nux vmnative)
endif ()
install(TARGETS nux)
