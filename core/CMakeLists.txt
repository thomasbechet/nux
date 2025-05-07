project(core LANGUAGES C)

# set (WAMR_BUILD_PLATFORM "linux")
# set (WAMR_BUILD_TARGET "X86_64")
#
# if (NUX_BUILD_WASM_DEBUG)
# set (WAMR_BUILD_INTERP 1)
# set (WAMR_BUILD_FAST_INTERP 0)
# set (WAMR_BUILD_DEBUG_INTERP 1)
# set (WAMR_DISABLE_HW_BOUND_CHECK 0)
# set (WAMR_DISABLE_STACK_HW_BOUND_CHECK 0)
# elseif (NUX_BUILD_WASM_JIT)
# set (WAMR_BUILD_INTERP 1)
# set (WAMR_BUILD_JIT 0) # requires LLVM
# set (WAMR_BUILD_FAST_JIT 1)
# set (WAMR_BUILD_SIMD 1)
# else ()
# set (WAMR_BUILD_INTERP 1)
# set (WAMR_BUILD_FAST_INTERP 1)
# set (WAMR_DISABLE_HW_BOUND_CHECK 1)
# set (WAMR_DISABLE_STACK_HW_BOUND_CHECK 1)
# endif ()
#
# set (WAMR_BUILD_GLOBAL_HEAP_POOL 0)
# set (WAMR_BUILD_ALLOC_WITH_USAGE 1)
# set (WAMR_BUILD_ALLOC_WITH_USER_DATA 1)
#
# set (WAMR_BUILD_LIBC_BUILTIN 1)
# set (WAMR_BUILD_LIBC_WASI 0)
# set (WAMR_BUILD_LIBC_UVWASI 0)
#
# set (WAMR_BUILD_REF_TYPES 1) # Prevent wamr error for wasi
#
# # set (WAMR_DISABLE_APP_ENTRY 1)
# # set (WAMR_BUILD_AOT_INTRINSICS 0)
# # set (WAMR_BUILD_LIB_PTHREAD 0)
# # set (WAMR_BUILD_TAIL_CALL 1)
#
# include (${CMAKE_SOURCE_DIR}/externals/wamr/build-scripts/runtime_lib.cmake)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/runtimes/native)
include_directories(${CMAKE_SOURCE_DIR}/externals/wasm3/source)
include_directories(${CMAKE_SOURCE_DIR}/externals/tiny-fixedpoint-c/src)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_compile_options(-DNU_STDLIB)

# Add constants
if (NUX_BUILD_TRACE)
    add_definitions(-DNUX_BUILD_TRACE)
endif ()

file(GLOB SOURCES *.c)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbulk-memory -mnontrapping-fptoint -msign-ext -mtail-call") TODO: support wasm build
set(WASM3_SOURCES
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_api_libc.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_api_wasi.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_api_uvwasi.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_api_meta_wasi.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_api_tracer.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_bind.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_code.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_compile.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_core.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_env.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_exec.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_function.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_info.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_module.c"
    "${CMAKE_SOURCE_DIR}/externals/wasm3/source/m3_parse.c"
)
set(FP_SOURCES
    "${CMAKE_SOURCE_DIR}/externals/tiny-fixedpoint-c/src/fixedpoint.c"
)
# file(GLOB WASM3_SOURCES ${CMAKE_SOURCE_DIR}/externals/wasm3/source/*.c)
# add_library(${PROJECT_NAME} ${SOURCES} ${WAMR_RUNTIME_LIB_SOURCE})
# target_link_libraries(${PROJECT_NAME} m3)
add_library(${PROJECT_NAME} ${SOURCES} ${WASM3_SOURCES} ${FP_SOURCES})
