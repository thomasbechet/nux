project(core LANGUAGES C)
# project(core LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_EXECUTABLE_SUFFIX ".wasm") # TODO: support wasm build
# add_definitions(-D_WASI_EMULATED_SIGNAL)
# add_definitions(-D_WASI_EMULATED_PROCESS_CLOCKS)
# add_link_options(-lwasi-emulated-signal)
# add_compile_options(-mllvm -wasm-enable-sjlj)

include_directories(${CMAKE_SOURCE_DIR}/externals/lua)
include_directories(${CMAKE_SOURCE_DIR}/externals/cgltf)
include_directories(${CMAKE_SOURCE_DIR}/externals/stb)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_compile_options(-pg)
# add_link_options(-pg)

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
list(REMOVE_ITEM LUA_SOURCES ${CMAKE_SOURCE_DIR}/externals/lua/liolib.c)
list(REMOVE_ITEM LUA_SOURCES ${CMAKE_SOURCE_DIR}/externals/lua/loslib.c)
file(GLOB SOURCES *.c)
add_library(${PROJECT_NAME} ${SOURCES} ${LUA_SOURCES})
# add_executable(${PROJECT_NAME} ${SOURCES} ${LUA_SOURCES})

# cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Documents/wasi-sdk/wasi-sdk-25.0-x86_64-linux/share/cmake/wasi-sdk-p1.cmake
