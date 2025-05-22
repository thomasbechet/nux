project(core LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/runtimes/native)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_compile_options(-DNU_STDLIB)

# Add constants
if (NUX_BUILD_TRACE)
    add_definitions(-DNUX_BUILD_TRACE)
endif ()

file(GLOB SOURCES *.c)
add_library(${PROJECT_NAME} ${SOURCES})
