cmake_minimum_required(VERSION 3.10.0)

project(nux VERSION 0.0.1)

include_directories(${CMAKE_SOURCE_DIR}/core)

# If compiling with wasi-sdk
add_definitions(-D_WASI_EMULATED_SIGNAL)
add_link_options(-lwasi-emulated-signal)
add_definitions(-D_WASI_EMULATED_PROCESS_CLOCKS)
# add_compile_options(-mllvm)
# add_compile_options(-wasm-enable-sjlj)
# add_link_options(-flto)
add_link_options(-mexec-model=reactor)
add_link_options(-Wl,--no-entry,--allow-undefined,--export=start,--export=malloc,--export=free)
# add_link_options(-nostdlib)
add_link_options(-nostartfiles)
add_link_options(-lsetjmp)
# add_link_options(-Wl,--trace-symbol=__wasi_fd_write) # https://github.com/WebAssembly/wasi-sdk/issues/190

file(GLOB SOURCES *.c)
set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} core m)

# cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=~/.local
# cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Documents/wasi-sdk/wasi-sdk-25.0-x86_64-linux/share/cmake/wasi-sdk.cmake
# wasm-opt --translate-to-exnref -all -o nux.patch.wasm nux.wasm
# wasm-decompile nux.wasm > dump.txt
