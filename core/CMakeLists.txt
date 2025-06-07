project(core LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/externals/lua)
include_directories(${CMAKE_SOURCE_DIR}/runtimes/native)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# Add constants
if (NUX_BUILD_TRACE)
    add_definitions(-DNUX_BUILD_TRACE)
endif ()

# Constants for lua
if (UNIX)
    add_definitions(-DLUA_USE_LINUX)
endif (UNIX)
if (WIN32)
endif (WIN32)

file(GLOB LUA_SOURCES ${CMAKE_SOURCE_DIR}/externals/lua/*.c)
file(GLOB SOURCES *.c)
add_library(${PROJECT_NAME} ${SOURCES} ${LUA_SOURCES})
