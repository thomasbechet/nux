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

## Inputs

Controller inputs :
- keys
- mouse buttons
- gamepad buttons
- gamepad axes
- text

Virtual Cursor :
- from joystick
- from mouse

## Scene Format

## Viewport

Describe where to render a source to a target.
Type of source :
- Camera
- Texture

