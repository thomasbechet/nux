project(vmnative LANGUAGES C)

file(GLOB VMNATIVE_SOURCES *.c)
include(${CMAKE_SOURCE_DIR}/runtimes/vmcore/runtime.cmake)
add_library(${PROJECT_NAME} ${VMNATIVE_SOURCES} ${VMCORE_SOURCES})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} m Xrandr X11 dl pthread)
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} m)
endif (WIN32)

include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
