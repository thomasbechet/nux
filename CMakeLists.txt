cmake_minimum_required(VERSION 3.10.0)

project(nux LANGUAGES C VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(NUX_BUILD_SDK "Build nux with sdk tools" ON)
option(NUX_BUILD_WASI "Build nux using wasi-sdk toolchain" ON)

# Show options
if (NUX_BUILD_SDK)
    message("NUX_BUILD_SDK enable")
else ()
    message("NUX_BUILD_SDK disable")
endif ()

# Add constants
if (NUX_BUILD_SDK)
    add_definitions(-DNUX_BUILD_SDK)
endif ()

# Add subdirectories
add_subdirectory(core)
# add_subdirectory(runtimes/native)
# add_subdirectory(runtimes/minimal)
add_subdirectory(runtimes/web)
