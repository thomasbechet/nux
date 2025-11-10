#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <nux.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define MAX_COMMAND   64
#define PATH_MAX_LEN  255
#define PATH_BUF_LEN  256
#define ARRAY_LEN(ar) (nux_u32_t)(sizeof(ar) / sizeof(ar[0]))
#define CHECK(cond, action) \
    if (!(cond))            \
    {                       \
        action;             \
    }

typedef struct
{
    bool        debug;
    const char *path;
} config_t;

typedef struct
{
    nux_gpu_pipeline_type_t type;
    GLboolean               enable_blend;
    GLboolean               enable_depth_test;
    GLuint                  primitive;
    GLuint                  program;
    GLuint                  indices[NUX_GPU_DESC_MAX];
    GLuint                  locations[NUX_GPU_DESC_MAX];
    GLuint                  units[NUX_GPU_DESC_MAX];
} pipeline_t;

typedef struct
{
    GLuint handle;
    GLuint width;
    GLuint height;
} framebuffer_t;

typedef struct
{
    GLuint                 handle;
    nux_gpu_texture_info_t info;
    GLuint                 internal_format;
    GLuint                 format;
    GLuint                 filtering;
} texture_t;

typedef struct
{
    GLuint handle;
    GLuint buffer_type;
} buffer_t;

typedef struct
{
    int x;
    int y;
} v2i_t;

typedef struct
{
    // instance

    int  fps;
    char path[PATH_BUF_LEN];
    bool running;

    // renderer

    pipeline_t     pipelines[NUX_GPU_PIPELINE_MAX];
    framebuffer_t  framebuffers[NUX_GPU_FRAMEBUFFER_MAX];
    texture_t      textures[NUX_GPU_TEXTURE_MAX];
    buffer_t       buffers[NUX_GPU_BUFFER_MAX];
    GLuint         empty_vao;
    pipeline_t    *active_pipeline;
    framebuffer_t *active_framebuffer;

    // io

    FILE *files[NUX_FILE_MAX];

    // window

    bool        fullscreen;
    bool        focused;
    bool        switch_fullscreen;
    bool        reload;
    GLFWwindow *win;
    v2i_t       size;
    double      prev_time;
    v2i_t       prev_position;
    v2i_t       prev_size;
    nux_f32_t   scroll;
    nux_v2_t    cursor_position;
    nux_v2_t    prev_cursor_position;
    double      prev_left_click;
    bool        double_click;
} runtime_t;

extern runtime_t runtime;

nux_status_t runtime_run(const config_t *config);
nux_status_t runtime_open(const char *path);
void         runtime_reset(void);

nux_status_t renderer_init(void);
void         renderer_free(void);
void         renderer_begin(void);
void         renderer_end(void);

nux_status_t window_init(void);
void         window_free(void);
void         window_begin_frame(void);
int          window_end_frame(void);

void io_init(void);
void io_free(void);

nux_status_t hotreload_init(void);
void         hotreload_free(void);

#endif
