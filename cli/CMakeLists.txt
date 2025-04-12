project(cli LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/sdk)
include_directories(${CMAKE_SOURCE_DIR}/runtimes/native)

# add_compile_options(-DNU_STDLIB)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

file(GLOB SOURCES *.c)
file(GLOB ARGPARSE_SOURCES ${CMAKE_SOURCE_DIR}/externals/argparse/*.c)

add_executable(${PROJECT_NAME} ${SOURCES} ${ARGPARSE_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "nux")
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} native core native sdk ${OPENGL_LIBRARIES} Xrandr X11 dl pthread m)
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} native core native sdk ${OPENGL_LIBRARIES} m)
endif (WIN32)
install(TARGETS ${PROJECT_NAME})
