project(vmcore LANGUAGES C)

set (WAMR_BUILD_PLATFORM "linux")
set (WAMR_BUILD_TARGET "X86_64")

set (WAMR_BUILD_INTERP 1)
set (WAMR_BUILD_FAST_INTERP 0)
set (WAMR_BUILD_AOT 0)

set (WAMR_BUILD_GLOBAL_HEAP_POOL 1)
set (WAMR_BUILD_ALLOC_WITH_USAGE 0)

set (WASM_BUILD_LIBC_BUILTIN 0)
set (WAMR_BUILD_LIBC_WASI 0)
set (WAMR_BUILD_LIBC_UVWASI 0)

set (WAMR_BUILD_REF_TYPES 1) # Prevent wamr error for wasi

# set (WAMR_DISABLE_APP_ENTRY 1)
# set (WAMR_BUILD_SIMD 0)
# set (WAMR_BUILD_AOT_INTRINSICS 0)
# set (WAMR_BUILD_LIB_PTHREAD 0)
# set (WAMR_BUILD_TAIL_CALL 0)
# set (WAMR_DISABLE_HW_BOUND_CHECK 0)
# set (WAMR_DISABLE_STACK_HW_BOUND_CHECK 0)

include (${CMAKE_SOURCE_DIR}/externals/wasm-micro-runtime/build-scripts/runtime_lib.cmake)

file(GLOB VMCORE_SOURCES *.c)
add_library(${PROJECT_NAME} ${VMCORE_SOURCES} ${WAMR_RUNTIME_LIB_SOURCE}) 
target_link_libraries(${PROJECT_NAME} m)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
