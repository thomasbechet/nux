project(vmnative LANGUAGES C)

file(GLOB NATIVE_RUNTIME_SOURCES *.c)
add_library(${PROJECT_NAME} ${NATIVE_RUNTIME_SOURCES})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} vmcore ${OPENGL_LIBRARIES} m Xrandr X11 dl pthread)
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} vmcore ${OPENGL_LIBRARIES} m)
endif (WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "nux")

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
include_directories(${vmcore_SOURCE_DIR})
