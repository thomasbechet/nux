#ifndef NUX_API_H
#define NUX_API_H

#ifdef NUX_INCLUDE_FIXED_TYPES
#else
typedef unsigned char nux_u8_t;
typedef char          nux_c8_t;
typedef int           nux_i16_t;
typedef unsigned int  nux_u16_t;
typedef int           nux_i32_t;
typedef unsigned int  nux_u32_t;
typedef long          nux_i64_t;
typedef unsigned long nux_u64_t;
typedef float         nux_f32_t;
typedef double        nux_f64_t;
#endif

typedef struct nux_env *nux_env_t;

typedef enum
{
    // 16:9
    // NUX_SCREEN_WIDTH  = 1920,
    // NUX_SCREEN_HEIGHT = 1080,

    // 16:10, too high ?
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 400,

    // 4:3
    // NUX_SCREEN_WIDTH  = 640,
    // NUX_SCREEN_HEIGHT = 480,

    // 16:10
    // NUX_SCREEN_WIDTH  = 512,
    // NUX_SCREEN_HEIGHT = 320,

    // 16:10
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 300,

    // 4:3
    // NUX_SCREEN_WIDTH  = 480,
    // NUX_SCREEN_HEIGHT = 360,

    // 16:10
    // NUX_SCREEN_WIDTH  = 360,
    // NUX_SCREEN_HEIGHT = 225,

    // 4:3
    NUX_SCREEN_WIDTH  = 320,
    NUX_SCREEN_HEIGHT = 240,

    NUX_TEXTURE_MIN_SIZE = 32,
    NUX_TEXTURE_MAX_SIZE = 256,
    NUX_PLAYER_MAX       = 4,
    NUX_BUTTON_MAX       = 10,
    NUX_AXIS_MAX         = 6,
    NUX_NODE_MAX         = (1 << 16) - 1,
    NUX_NAME_MAX         = 64,
    NUX_PALETTE_LENGTH   = 256,
} nux_constants_t;

typedef enum
{
    NUX_ERROR_NONE                 = 0,
    NUX_ERROR_OUT_OF_MEMORY        = 1,
    NUX_ERROR_INVALID_TEXTURE_SIZE = 4,
    NUX_ERROR_WASM_RUNTIME         = 8,
    NUX_ERROR_CART_EOF             = 10,
    NUX_ERROR_CART_MOUNT           = 11,
} nux_error_t;

typedef enum
{
    NUX_SUCCESS = 1,
    NUX_FAILURE = 0
} nux_status_t;

typedef enum
{
    NUX_VERTEX_POSITION = 1 << 0,
    NUX_VERTEX_UV       = 1 << 1,
    NUX_VERTEX_COLOR    = 1 << 2,
    NUX_VERTEX_INDICES  = 1 << 3,
} nux_vertex_attribute_t;

typedef enum
{
    NUX_PRIMITIVE_TRIANGLES = 0,
    NUX_PRIMITIVE_LINES     = 1,
    NUX_PRIMITIVE_POINTS    = 2,
} nux_primitive_t;

typedef enum
{
    NUX_BUTTON_A     = 1 << 0,
    NUX_BUTTON_X     = 1 << 1,
    NUX_BUTTON_Y     = 1 << 2,
    NUX_BUTTON_B     = 1 << 3,
    NUX_BUTTON_UP    = 1 << 4,
    NUX_BUTTON_DOWN  = 1 << 5,
    NUX_BUTTON_LEFT  = 1 << 6,
    NUX_BUTTON_RIGHT = 1 << 7,
    NUX_BUTTON_LB    = 1 << 8,
    NUX_BUTTON_RB    = 1 << 9,
} nux_button_t;

typedef enum
{
    NUX_AXIS_LEFTX  = 0,
    NUX_AXIS_LEFTY  = 1,
    NUX_AXIS_RIGHTX = 2,
    NUX_AXIS_RIGHTY = 3,
    NUX_AXIS_RT     = 4,
    NUX_AXIS_LT     = 5,
} nux_axis_t;

typedef enum
{
    NUX_CONSOLE_MEMORY_CAPACITY = 0,
    NUX_CONSOLE_MEMORY_USAGE    = 1,
} nux_console_info_t;

typedef enum
{
    NUX_MAP_FRAMEBUFFER = 0x0,
    NUX_MAP_PALETTE
    = NUX_MAP_FRAMEBUFFER + (NUX_SCREEN_WIDTH * NUX_SCREEN_HEIGHT),
    NUX_MAP_GAMEPAD_BUTTONS
    = NUX_MAP_PALETTE + NUX_PALETTE_LENGTH * sizeof(nux_u32_t),
    NUX_MAP_GAMEPAD_AXIS
    = NUX_MAP_GAMEPAD_BUTTONS + NUX_PLAYER_MAX * sizeof(nux_u32_t),
    NUX_MAP_TIME
    = NUX_MAP_GAMEPAD_AXIS + NUX_PLAYER_MAX * sizeof(nux_f32_t) * NUX_AXIS_MAX,
    NUX_MAP_FRAME_INDEX = NUX_MAP_TIME + sizeof(nux_f32_t),
} nux_map_t;

// Debug
void      nux_trace(nux_env_t env, const nux_c8_t *text);
void      nux_inspect_i32(nux_env_t env, const nux_c8_t *name, nux_i32_t *p);
void      nux_inspect_f32(nux_env_t env, const nux_c8_t *name, nux_f32_t *p);
nux_u32_t nux_console_info(nux_env_t env, nux_console_info_t info);
nux_f32_t nux_global_time(nux_env_t env);
nux_f32_t nux_delta_time(nux_env_t env);

// Memory
void nux_palset(nux_env_t env, nux_u8_t index, nux_u32_t color);
void nux_clear(nux_env_t env, nux_u32_t color);
void nux_fill(nux_env_t env,
              nux_i32_t x0,
              nux_i32_t y0,
              nux_i32_t x1,
              nux_i32_t y1,
              nux_u8_t  color);
void nux_point(nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t color);
void nux_line(nux_env_t env,
              nux_i32_t x0,
              nux_i32_t y0,
              nux_i32_t x1,
              nux_i32_t y1,
              nux_u8_t  color);
void nux_triangle(nux_env_t env,
                  nux_i32_t x0,
                  nux_i32_t y0,
                  nux_i32_t x1,
                  nux_i32_t y1,
                  nux_i32_t x2,
                  nux_i32_t y2,
                  nux_u8_t  color);
void nux_begin3d(nux_env_t env, nux_primitive_t primitive);
void nux_end3d(nux_env_t env);
void nux_vertex(nux_env_t env, nux_f32_t x, nux_f32_t y, nux_f32_t z);
void nux_uv(nux_env_t env, nux_f32_t u, nux_f32_t v);

// Input
nux_u32_t nux_button(nux_env_t env, nux_u32_t player);
nux_f32_t nux_axis(nux_env_t env, nux_u32_t player, nux_axis_t axis);

#endif
