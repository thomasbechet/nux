clang --target=wasm32 -nostdlib -Wl,--no-entry,--export=square,--allow-undefined square.c -o square.wasm
clang --target=wasm32 -nostdlib -Wl,--no-entry,--export=hello lib.c -o lib.wasm

