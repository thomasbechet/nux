project(native LANGUAGES C)

# set (WAMR_BUILD_PLATFORM "linux")
# set (WAMR_BUILD_TARGET "X86_64")

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

# set (WAMR_DISABLE_APP_ENTRY 1)
# set (WAMR_BUILD_AOT_INTRINSICS 0)
# set (WAMR_BUILD_LIB_PTHREAD 0)
# set (WAMR_BUILD_TAIL_CALL 1)

# include (${CMAKE_SOURCE_DIR}/externals/wamr/build-scripts/runtime_lib.cmake)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/sdk)

# add_compile_options(-DNU_STDLIB)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

file(GLOB SOURCES *.c)

# add_executable(${PROJECT_NAME} ${RUNTIME_SOURCES} ${CLI_SOURCES} ${SDK_SOURCES} ${WAMR_RUNTIME_LIB_SOURCE} ${PARSON_SOURCES} ${ARGPARSE_SOURCES})
add_library(${PROJECT_NAME} ${SOURCES})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} core ${OPENGL_LIBRARIES} Xrandr X11 dl pthread m)
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} core ${OPENGL_LIBRARIES} m)
endif (WIN32)
