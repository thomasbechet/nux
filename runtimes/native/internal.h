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
#include <nux.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>
#include <glad/gl.h>

#define MAX_COMMAND   64
#define PATH_MAX_LEN  256
#define ARRAY_LEN(ar) (sizeof(ar) / sizeof(ar[0]))
#define CHECK(cond, action) \
    if (!(cond))            \
    {                       \
        action;             \
    }

typedef enum
{
    VIEWPORT_HIDDEN,
    VIEWPORT_FIXED,
    VIEWPORT_FIXED_BEST_FIT,
    VIEWPORT_STRETCH_KEEP_ASPECT,
    VIEWPORT_STRETCH,
} viewport_mode_t;

typedef enum
{
    VIEW_GAME     = 0,
    VIEW_OPEN     = 1,
    VIEW_CONTROLS = 2,
    VIEW_SETTINGS = 3,
} view_t;

typedef struct
{
    enum
    {
        COMMAND_EXIT,
        COMMAND_SAVE_STATE,
        COMMAND_LOAD_STATE,
        COMMAND_CHANGE_VIEW,
    } type;
    view_t view;
} command_t;

typedef struct
{
    GLuint handle;
} pipeline_t;

typedef struct
{
    GLuint handle;
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
    char                  path[PATH_MAX_LEN];
    bool                  active;
    nux_instance_config_t config;
    nux_instance_t       *instance;
    bool                  pause;
    struct nk_rect        viewport_ui;
    viewport_mode_t       viewport_mode;
    struct nk_rect        viewport;
    pipeline_t            pipelines[NUX_GPU_PIPELINE_MAX];
    framebuffer_t         framebuffers[NUX_GPU_FRAMEBUFFER_MAX];
    texture_t             textures[NUX_GPU_TEXTURE_MAX];
    buffer_t              buffers[NUX_GPU_BUFFER_MAX];
} instance_t;

typedef struct
{
    bool        debug;
    const char *path;
} config_t;

void *native_malloc(size_t n);
void  native_free(void *p);
void *native_realloc(void *p, size_t n);

void logger_log(nux_log_level_t level, const char *fmt, ...);
void logger_vlog(nux_log_level_t level, const char *fmt, va_list args);

nux_status_t runtime_run(const config_t *config);
nux_status_t runtime_open(int index, const char *path);
void         runtime_close(int index);
void         runtime_reset(int index);
instance_t  *runtime_instance(void);
void         runtime_quit(void);

nux_status_t renderer_init(void);
void         renderer_free(void);
void renderer_clear(struct nk_rect viewport, struct nk_vec2i window_size);
void renderer_render_begin(instance_t *inst, struct nk_vec2i window_size);

nux_status_t       window_init(void);
void               window_free(void);
void               window_begin_frame(void);
int                window_end_frame(void);
struct nk_vec2i    window_get_size(void);
struct nk_context *window_nk_context(void);
bool               window_is_double_click(void);
bool               window_is_fullscreen(void);

void command_push(command_t cmd);
bool command_poll(command_t *cmd);

void view_game(struct nk_context *ctx, struct nk_rect bounds);
void view_controls(struct nk_context *ctx, struct nk_rect bounds);
void view_settings(struct nk_context *ctx, struct nk_rect bounds);
void view_open(struct nk_context *ctx, struct nk_rect bounds);

void gui_update(void);
void gui_set_view(view_t view);

void loop_init(nux_env_t *env);
void loop_update(nux_env_t *env);

void load_blk_colormap(nux_env_t *env);

#endif
