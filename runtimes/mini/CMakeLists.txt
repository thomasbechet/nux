cmake_minimum_required(VERSION 3.10.0)

project(nux-mini VERSION 0.0.1)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_EXECUTABLE_SUFFIX ".wasm") TODO: support wasm build

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

include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/externals)
file(GLOB SOURCES *.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} core sdk m)
install(TARGETS ${PROJECT_NAME})
