#ifndef NUX_H
#define NUX_H

#include <core/platform.h>
#include <input/platform.h>
#include <graphics/platform.h>
#include <input/platform.h>

#include <core/core.h>
#include <input/input.h>
#include <scene/scene.h>
#include <lua/lua.h>
#include <physics/physics.h>
#include <graphics/graphics.h>
#include <ui/ui.h>

NUX_API nux_status_t nux_core_init(void);
NUX_API void         nux_core_free(void);
NUX_API void         nux_core_update(void);

#endif
