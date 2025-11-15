project(core LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_compile_options(-pg)
# add_link_options(-pg)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

# Add constants
if (NUX_BUILD_TRACE)
    add_definitions(-DNUX_BUILD_TRACE)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/core)
file(GLOB_RECURSE SOURCES *.c)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/core/externals/lua/liolib.c)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/core/externals/lua/loslib.c)

if (NUX_BUILD_WASI)
    add_definitions(-D_WASI_EMULATED_SIGNAL)
    add_definitions(-D_WASI_EMULATED_PROCESS_CLOCKS)
    add_compile_options(-mllvm)
    add_compile_options(-wasm-enable-sjlj)
    add_link_options(-lwasi-emulated-signal)
    add_link_options(-mexec-model=reactor)
    add_link_options(-lsetjmp)
endif (NUX_BUILD_WASI)

add_library(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 23)
