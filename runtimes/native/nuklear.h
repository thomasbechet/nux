#ifndef NUKLEAR_H
#define NUKLEAR_H

#ifdef NUX_BUILD_GUI

#include <nulib/nulib.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear/nuklear.h>

struct RGFW_window;

nu_status_t gui_init(struct RGFW_window *win);
void        gui_free(void);

void               gui_font_stash_begin(struct nk_font_atlas **atlas);
void               gui_font_stash_end(void);
struct nk_context *gui_new_frame(void);
void               gui_render(enum nk_anti_aliasing aa,
                              int                   max_vertex_buffer,
                              int                   max_element_buffer);

void gui_device_destroy(void);
void gui_device_create(void);

void gui_char_callback(struct RGFW_window *win, unsigned int codepoint);
void gui_scroll_callback(struct RGFW_window *win, double xoff, double yoff);
void gui_mouse_button_callback(struct RGFW_window *win,
                               int                 button,
                               double              scroll,
                               int                 pressed);

#endif

#endif
