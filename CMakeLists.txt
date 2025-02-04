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

option(NUX_BUILD_SDK "Build nux with sdk tools" ON)

add_subdirectory(runtimes/native)
if (NUX_BUILD_SDK)
    add_definitions(-DNUX_BUILD_SDK)
    add_subdirectory(sdk)
    message("NUX_BUILD_SDK enable")
else ()
    message("NUX_BUILD_SDK disable")
endif ()
add_subdirectory(cli)
