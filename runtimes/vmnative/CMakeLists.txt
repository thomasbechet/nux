project(vmnative LANGUAGES C)

file(GLOB VMNATIVE_SOURCES *.c)
add_library(${PROJECT_NAME} ${VMNATIVE_SOURCES})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} Xrandr X11 dl pthread
    )
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif (WIN32)
include_directories(${CMAKE_SOURCE_DIR}/externals)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/runtimes)
