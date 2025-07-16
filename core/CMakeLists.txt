project(core LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

file(GLOB SOURCES *.c)
file(GLOB LUA_SOURCES luavm/*.c)

if (NUX_BUILD_WASI)
    add_definitions(-D_WASI_EMULATED_SIGNAL)
    add_definitions(-D_WASI_EMULATED_PROCESS_CLOCKS)
    add_compile_options(-mllvm)
    add_compile_options(-wasm-enable-sjlj)
    add_link_options(-lwasi-emulated-signal)
    add_link_options(-mexec-model=reactor)
    add_link_options(-lsetjmp)
    list(REMOVE_ITEM LUA_SOURCES ${CMAKE_SOURCE_DIR}/externals/lua/liolib.c)
    list(REMOVE_ITEM LUA_SOURCES ${CMAKE_SOURCE_DIR}/externals/lua/loslib.c)
endif (NUX_BUILD_WASI)

add_library(${PROJECT_NAME} ${SOURCES} ${LUA_SOURCES})

