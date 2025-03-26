cmake_minimum_required(VERSION 3.10.0)

project(nux VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCCompilerFlag)
function(enable_c_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_c_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_c_compiler_flag_if_supported("-Wall")
enable_c_compiler_flag_if_supported("-Wextra")
enable_c_compiler_flag_if_supported("-Werror")
enable_c_compiler_flag_if_supported("-Wno-unused-function")
enable_c_compiler_flag_if_supported("-Wno-unused-parameter")
enable_c_compiler_flag_if_supported("-Wno-unused-variable")
enable_c_compiler_flag_if_supported("-fsanitize=address")

# Options
option(NUX_BUILD_SDK "Build nux with sdk tools" ON)
option(NUX_BUILD_WASM_JIT "Build nux with runtime jit" OFF)
option(NUX_BUILD_WASM_DEBUG "Build nux with runtime debug server" OFF)

# Show options
if (NUX_BUILD_SDK)
    message("NUX_BUILD_SDK enable")
else ()
    message("NUX_BUILD_SDK disable")
endif ()
if (NUX_BUILD_WASM_JIT)
    message("NUX_BUILD_WASM_JIT enable")
else ()
    message("NUX_BUILD_WASM_JIT disable")
endif ()
if (NUX_BUILD_WASM_DEBUG)
    message("NUX_BUILD_WASM_DEBUG enable")
else ()
    message("NUX_BUILD_WASM_DEBUG disable")
endif ()

# Add constants
if (NUX_BUILD_SDK)
    add_definitions(-DNUX_BUILD_SDK)
endif ()
if (NUX_BUILD_WASM_DEBUG)
    add_definitions(-DNUX_BUILD_WASM_DEBUG)
elseif (NUX_BUILD_WASM_JIT)
    add_definitions(-DNUX_BUILD_WASM_JIT)
endif ()

# Add subdirectories
# add_subdirectory(runtimes/native)
# add_subdirectory(externals/wasm3/source)
add_subdirectory(core)
add_subdirectory(runtimes/mini)
