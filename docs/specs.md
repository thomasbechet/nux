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


- use lua as config files

## Objects

- An object belongs to an arena which correspond to a lifetime
- Each objects are referenced with and id (using index + version) in a global table
- The object's state must be contained only in a single arena
- Objects pool is possible

## Program

- The runtime execute a single program
