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

## Resources

- A resource belongs to an arena which define to a lifetime
- Each resources are referenced by an handle (using index + version) in a global table
- The resource's state must be contained only in a single arena

## Program

1) nux.conf(config)

Define program configuration with access to the config table. The API is
not available.

2) nux.init()

Call once the program is initialized.

3) nux.tick()

Call each frame.

## Gui rendering

2D api -> canvas -> render to texture
3D api -> ecs    -> render to texture

## Inputs

Platform inputs :
- axis
- text
- cursor
- buttons

Virtual Cursor :
- from joystick
- from mouse

- Buttons

- Axis

- Cursor
