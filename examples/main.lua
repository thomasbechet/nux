local inspect = require("inspect")

local gfx = nux.graphics

local a = nux.newArena()
local gui = gfx.newRenderTarget(a, 360, 200)

gfx.setRenderTarget(gui)

gfx.setRenderTarget(0) -- default render target
gfx.blit(gui:texture())

local rt = gfx.newRenderTarget(500, 500)
local tex = gfx.newTexture(500, 500)
