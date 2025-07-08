# Console Specifications

TODO

# ? Load / Save state
# ? Determinism
- floating point patch required => patch lua
# Portability
- play on browser
- single file game
- use lua as script language
- can be compiled natively
# Retrostyle
- nearest filtering
- low res canvas (2D)
- low poly
- fixed viewport resolution
- small cartridge size
- pixel perfect
# Fantasy console
- integrated IDE

## Objects

- An object belongs to an arena which correspond to a lifetime
- Each objects are referenced with and id (using index + version) in a global table
- The object's state must be contained only in a single arena
- Objects pool is possible

## Program

- The runtime execute a single program

## Gui rendering

2D api -> canvas -> render to texture
3D api -> scene  -> render to texture

nux.graphics.set_canvas(c)
nux.graphics.line(0, 0, 50, 50)

local rt = nux.texture.new(...)
local s  = nux.scene.new(...)
local c  = nux.canvas.new(...)

nux.scene.render(s, rt)
nux.canvas.render(c, rt)

- render targets => texture or screen
- canvas => texture
- scene => render target
